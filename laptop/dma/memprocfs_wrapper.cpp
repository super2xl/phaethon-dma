#include "memprocfs_wrapper.h"
#include <algorithm>
#include <sstream>

namespace phaethon {
namespace dma {

// ScatterHandle implementation

void ScatterHandle::AddRead(uintptr_t address, void* buffer, size_t size, uint32_t* bytes_read) {
    if (!m_hScatter || m_executed) {
        return;
    }

    // Prepare the read in MemProcFS
    VMMDLL_Scatter_PrepareEx(m_hScatter, address, (DWORD)size, (PBYTE)buffer, (PDWORD)bytes_read);

    // Store request for tracking
    m_requests.push_back({ address, buffer, size, bytes_read });
}

bool ScatterHandle::Execute() {
    if (!m_hScatter || m_executed) {
        return false;
    }

    // Execute all prepared reads in one DMA transaction
    bool success = VMMDLL_Scatter_ExecuteRead(m_hScatter);
    m_executed = true;

    return success;
}

void ScatterHandle::Clear() {
    if (m_hScatter) {
        VMMDLL_Scatter_Clear(m_hScatter, m_pid, VMMDLL_FLAG_NOCACHE);
        m_requests.clear();
        m_executed = false;
    }
}

// MemProcFS implementation

MemProcFS::MemProcFS()
    : m_hVMM(nullptr)
    , m_pid(0)
    , m_module_cache_valid(false) {
}

MemProcFS::~MemProcFS() {
    Close();
}

bool MemProcFS::Initialize(const std::string& device_args) {
    if (m_hVMM) {
        Close();
    }

    // Build argv for VMMDLL_Initialize
    // Format: -device <device_args>
    const char* argv[] = {
        "",
        "-device",
        device_args.c_str()
    };

    m_hVMM = VMMDLL_Initialize(3, argv);

    return m_hVMM != nullptr;
}

bool MemProcFS::AttachToProcess(const std::string& process_name) {
    if (!m_hVMM) {
        return false;
    }

    DWORD pid = 0;
    if (!VMMDLL_PidGetFromName(m_hVMM, process_name.c_str(), &pid)) {
        return false;
    }

    m_pid = pid;
    m_process_name = process_name;
    m_module_cache_valid = false;

    // Refresh module cache
    RefreshModuleCache();

    return true;
}

void MemProcFS::Close() {
    if (m_hVMM) {
        VMMDLL_Close(m_hVMM);
        m_hVMM = nullptr;
    }
    m_pid = 0;
    m_process_name.clear();
    m_module_cache.clear();
    m_module_cache_valid = false;
}

std::vector<uint32_t> MemProcFS::GetAllPIDs() {
    std::vector<uint32_t> pids;

    if (!m_hVMM) {
        return pids;
    }

    SIZE_T cPIDs = 0;
    if (!VMMDLL_PidList(m_hVMM, nullptr, &cPIDs)) {
        return pids;
    }

    pids.resize(cPIDs);
    if (!VMMDLL_PidList(m_hVMM, (PDWORD)pids.data(), &cPIDs)) {
        pids.clear();
    }

    return pids;
}

ModuleInfo MemProcFS::GetModule(const std::string& module_name) {
    if (!m_module_cache_valid) {
        RefreshModuleCacheInternal();
    }

    auto it = m_module_cache.find(module_name);
    if (it != m_module_cache.end()) {
        return it->second;
    }

    // Not in cache, try to query directly
    PVMMDLL_MAP_MODULEENTRY pModuleEntry = nullptr;
    if (VMMDLL_Map_GetModuleFromNameU(m_hVMM, m_pid, module_name.c_str(), &pModuleEntry, 0)) {
        ModuleInfo info;
        info.name = module_name;
        info.full_path = pModuleEntry->uszFullName ? pModuleEntry->uszFullName : "";
        info.base_address = pModuleEntry->vaBase;
        info.size = pModuleEntry->cbImageSize;
        info.is_wow64 = pModuleEntry->fWoW64;

        // Add to cache
        m_module_cache[module_name] = info;
        return info;
    }

    return ModuleInfo();
}

uintptr_t MemProcFS::GetModuleBase(const std::string& module_name) {
    if (!m_hVMM || m_pid == 0) {
        return 0;
    }

    // Check cache first
    if (m_module_cache_valid) {
        auto it = m_module_cache.find(module_name);
        if (it != m_module_cache.end()) {
            return it->second.base_address;
        }
    }

    // Query MemProcFS
    ULONG64 base = VMMDLL_ProcessGetModuleBaseU(m_hVMM, m_pid, module_name.c_str());

    if (base != 0) {
        // Update cache
        ModuleInfo info = GetModule(module_name);
    }

    return base;
}

uint32_t MemProcFS::GetModuleSize(const std::string& module_name) {
    ModuleInfo info = GetModule(module_name);
    return info.size;
}

std::vector<ModuleInfo> MemProcFS::GetAllModules() {
    std::vector<ModuleInfo> modules;

    if (!m_hVMM || m_pid == 0) {
        return modules;
    }

    PVMMDLL_MAP_MODULE pModuleMap = nullptr;
    if (!VMMDLL_Map_GetModuleU(m_hVMM, m_pid, &pModuleMap, 0)) {
        return modules;
    }

    for (DWORD i = 0; i < pModuleMap->cMap; i++) {
        auto& entry = pModuleMap->pMap[i];
        ModuleInfo info;
        info.name = entry.uszText ? entry.uszText : "";
        info.full_path = entry.uszFullName ? entry.uszFullName : "";
        info.base_address = entry.vaBase;
        info.size = entry.cbImageSize;
        info.is_wow64 = entry.fWoW64;

        modules.push_back(info);
    }

    VMMDLL_MemFree(pModuleMap);

    return modules;
}

void MemProcFS::RefreshModuleCache() {
    m_module_cache.clear();
    m_module_cache_valid = RefreshModuleCacheInternal();
}

bool MemProcFS::RefreshModuleCacheInternal() {
    if (!m_hVMM || m_pid == 0) {
        return false;
    }

    PVMMDLL_MAP_MODULE pModuleMap = nullptr;
    if (!VMMDLL_Map_GetModuleU(m_hVMM, m_pid, &pModuleMap, 0)) {
        return false;
    }

    for (DWORD i = 0; i < pModuleMap->cMap; i++) {
        auto& entry = pModuleMap->pMap[i];

        ModuleInfo info;
        info.name = entry.uszText ? entry.uszText : "";
        info.full_path = entry.uszFullName ? entry.uszFullName : "";
        info.base_address = entry.vaBase;
        info.size = entry.cbImageSize;
        info.is_wow64 = entry.fWoW64;

        if (!info.name.empty()) {
            m_module_cache[info.name] = info;
        }
    }

    VMMDLL_MemFree(pModuleMap);

    return true;
}

bool MemProcFS::ReadMemory(uintptr_t address, void* buffer, size_t size) {
    if (!m_hVMM || m_pid == 0) {
        return false;
    }

    return VMMDLL_MemRead(m_hVMM, m_pid, address, (PBYTE)buffer, (DWORD)size);
}

bool MemProcFS::WriteMemory(uintptr_t address, const void* buffer, size_t size) {
    if (!m_hVMM || m_pid == 0) {
        return false;
    }

    return VMMDLL_MemWrite(m_hVMM, m_pid, address, (PBYTE)buffer, (DWORD)size);
}

bool MemProcFS::ReadMemoryEx(uintptr_t address, void* buffer, size_t size, uint32_t flags) {
    if (!m_hVMM || m_pid == 0) {
        return false;
    }

    DWORD bytes_read = 0;
    return VMMDLL_MemReadEx(m_hVMM, m_pid, address, (PBYTE)buffer, (DWORD)size, &bytes_read, (DWORD)flags);
}

std::unique_ptr<ScatterHandle> MemProcFS::CreateScatter(uint32_t flags) {
    if (!m_hVMM || m_pid == 0) {
        return nullptr;
    }

    VMMDLL_SCATTER_HANDLE hScatter = VMMDLL_Scatter_Initialize(m_hVMM, m_pid, (DWORD)flags);
    if (!hScatter) {
        return nullptr;
    }

    return std::unique_ptr<ScatterHandle>(new ScatterHandle(m_hVMM, m_pid, hScatter));
}

std::string MemProcFS::GetLastError() const {
    // MemProcFS doesn't provide detailed error messages
    // We can return generic status
    if (!m_hVMM) {
        return "MemProcFS not initialized";
    }
    if (m_pid == 0) {
        return "No process attached";
    }
    return "Unknown error";
}

} // namespace dma
} // namespace phaethon

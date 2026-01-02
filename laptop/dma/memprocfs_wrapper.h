#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <vmmdll.h>

namespace phaethon {
namespace dma {

// Module information
struct ModuleInfo {
    std::string name;
    std::string full_path;
    uintptr_t base_address;
    uint32_t size;
    bool is_wow64;

    ModuleInfo()
        : base_address(0), size(0), is_wow64(false) {}
};

// Scatter-gather handle for batched reads
class ScatterHandle {
friend class MemProcFS;

private:
    VMM_HANDLE m_hVMM;
    VMMDLL_SCATTER_HANDLE m_hScatter;
    uint32_t m_pid;
    bool m_executed;

    struct ReadRequest {
        uintptr_t address;
        void* buffer;
        size_t size;
        uint32_t* bytes_read;
    };

    std::vector<ReadRequest> m_requests;

    ScatterHandle(VMM_HANDLE hVMM, uint32_t pid, VMMDLL_SCATTER_HANDLE hScatter)
        : m_hVMM(hVMM), m_hScatter(hScatter), m_pid(pid), m_executed(false) {}

public:
    ~ScatterHandle() {
        if (m_hScatter) {
            VMMDLL_Scatter_CloseHandle(m_hScatter);
        }
    }

    // Disable copy, enable move
    ScatterHandle(const ScatterHandle&) = delete;
    ScatterHandle& operator=(const ScatterHandle&) = delete;
    ScatterHandle(ScatterHandle&& other) noexcept
        : m_hVMM(other.m_hVMM)
        , m_hScatter(other.m_hScatter)
        , m_pid(other.m_pid)
        , m_executed(other.m_executed)
        , m_requests(std::move(other.m_requests)) {
        other.m_hScatter = nullptr;
    }

    // Add a read to the batch
    void AddRead(uintptr_t address, void* buffer, size_t size, uint32_t* bytes_read = nullptr);

    // Execute all batched reads
    bool Execute();

    // Clear and prepare for reuse
    void Clear();

    bool IsValid() const { return m_hScatter != nullptr; }
};

// Main MemProcFS interface
class MemProcFS {
private:
    VMM_HANDLE m_hVMM;
    uint32_t m_pid;
    std::string m_process_name;

    std::unordered_map<std::string, ModuleInfo> m_module_cache;
    bool m_module_cache_valid;

public:
    MemProcFS();
    ~MemProcFS();

    // Disable copy
    MemProcFS(const MemProcFS&) = delete;
    MemProcFS& operator=(const MemProcFS&) = delete;

    // Initialization
    bool Initialize(const std::string& device_args = "fpga");
    bool AttachToProcess(const std::string& process_name);
    void Close();

    // Process operations
    uint32_t GetPID() const { return m_pid; }
    std::string GetProcessName() const { return m_process_name; }
    std::vector<uint32_t> GetAllPIDs();

    // Module operations
    ModuleInfo GetModule(const std::string& module_name);
    uintptr_t GetModuleBase(const std::string& module_name);
    uint32_t GetModuleSize(const std::string& module_name);
    std::vector<ModuleInfo> GetAllModules();
    void RefreshModuleCache();

    // Memory operations (simple)
    template<typename T>
    T Read(uintptr_t address) {
        T buffer{};
        ReadMemory(address, &buffer, sizeof(T));
        return buffer;
    }

    template<typename T>
    bool Write(uintptr_t address, const T& value) {
        return WriteMemory(address, &value, sizeof(T));
    }

    bool ReadMemory(uintptr_t address, void* buffer, size_t size);
    bool WriteMemory(uintptr_t address, const void* buffer, size_t size);

    // Read with cache flags
    bool ReadMemoryEx(uintptr_t address, void* buffer, size_t size, uint32_t flags = 0);

    // Scatter-gather operations (high performance)
    std::unique_ptr<ScatterHandle> CreateScatter(uint32_t flags = VMMDLL_FLAG_NOCACHE);

    // Utility
    bool IsValid() const { return m_hVMM != nullptr && m_pid != 0; }
    VMM_HANDLE GetHandle() const { return m_hVMM; }

    // Error information
    std::string GetLastError() const;

private:
    bool RefreshModuleCacheInternal();
};

} // namespace dma
} // namespace phaethon

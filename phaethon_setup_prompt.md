# Claude Code Agent: __phaethon__ - CS2 DMA Framework Setup

## Project Context
This is a development framework for **SofaLeague** - a private CS2 HvH (Hack vs Hack) competitive league similar to BananaShooter. SofaLeague is launching publicly soon with expected high applicant volume. This framework is for legitimate competitive gameplay within a sandboxed league environment where all participants use similar tools.

## Project Philosophy
**"Know your limits"** - A reference to pushing technical boundaries in controlled competitive environments. This framework demonstrates advanced hardware-level memory access techniques for educational and competitive HvH purposes.

## Project Overview
Two-PC DMA development framework with:
- Visual Studio 2022 solution (clean build system)
- Integrated MemProcFS source (single executable deployment)
- Andromeda CS2 base as reference architecture
- Laptop handles memory operations, desktop runs game client

## Workspace Structure

```
C:\Users\forgery\Documents\
├── phaethon\                           # Main project
│   ├── __phaethon__.sln               # VS2022 solution
│   ├── .git\
│   ├── .gitignore
│   ├── README.md
│   │
│   ├── shared\                        # Shared code (both laptop & desktop)
│   │   ├── shared.vcxproj
│   │   ├── network\
│   │   │   ├── protocol.h            # Packet structures
│   │   │   ├── socket.h/cpp          # UDP wrapper
│   │   │   └── encryption.h/cpp      # Optional packet encryption
│   │   ├── math\
│   │   │   ├── vector.h              # Vector2/Vector3/Vector4
│   │   │   ├── matrix.h              # Matrix math
│   │   │   └── geometry.h            # WorldToScreen, angles
│   │   ├── offsets\
│   │   │   ├── offsets.h             # Game offsets (updated)
│   │   │   └── schema.h              # Schema system offsets
│   │   └── utils\
│   │       ├── config.h/cpp          # JSON config
│   │       ├── logger.h/cpp          # Logging
│   │       └── memory.h              # Memory utils
│   │
│   ├── laptop\                        # Laptop project (DMA logic)
│   │   ├── laptop.vcxproj
│   │   ├── main.cpp
│   │   ├── dma\
│   │   │   ├── dma_manager.h/cpp     # MemProcFS wrapper
│   │   │   ├── scatter.h/cpp         # Scatter-gather optimized reads
│   │   │   └── cache.h/cpp           # Memory caching layer
│   │   ├── game\
│   │   │   ├── cs2.h/cpp             # CS2 interface
│   │   │   ├── entity.h/cpp          # Entity management
│   │   │   ├── player.h/cpp          # Player class
│   │   │   └── world.h/cpp           # World/level data
│   │   ├── features\
│   │   │   ├── visualization.h/cpp    # Data visualization
│   │   │   ├── aimbot.h/cpp          # Aim assistance calculations
│   │   │   ├── triggerbot.h/cpp      # Trigger timing logic
│   │   │   ├── rcs.h/cpp             # Recoil pattern analysis
│   │   │   └── misc.h/cpp            # Misc utilities
│   │   ├── network\
│   │   │   └── sender.h/cpp          # Send to desktop
│   │   └── config\
│   │       └── config.json
│   │
│   ├── desktop\                       # Desktop project (overlay)
│   │   ├── desktop.vcxproj
│   │   ├── main.cpp
│   │   ├── overlay\
│   │   │   ├── overlay.h/cpp         # D3D11 overlay base
│   │   │   ├── renderer.h/cpp        # Drawing primitives
│   │   │   ├── menu.h/cpp            # ImGui menu
│   │   │   └── esp.h/cpp             # ESP rendering
│   │   ├── network\
│   │   │   └── receiver.h/cpp        # Receive from laptop
│   │   └── hooks\
│   │       └── d3d_hook.h/cpp        # Hook D3D for overlay
│   │
│   ├── references\                    # Reference materials
│   │   ├── Andromeda-CS2-Base\       # Your Andromeda clone (reference only)
│   │   └── MemProcFS\                # MemProcFS source (to integrate)
│   │
│   └── tools\
│       ├── offset_updater\
│       │   └── update_offsets.py
│       └── schema_dumper\
│           └── dump_schema.cpp
```

## Visual Studio Solution Structure

**__phaethon__.sln contains:**
1. **shared** (Static Library) - Common code
2. **laptop** (Console Application) - DMA reader/writer
3. **desktop** (Windows Application) - Overlay renderer

## Integration Strategy

### MemProcFS Source Integration
Instead of linking against DLLs, we'll compile MemProcFS directly into the laptop project.

**From C:\Users\forgery\Documents\phaethon\references\MemProcFS:**
1. Identify core source files (vmm, leechcore)
2. Add to laptop project as source files
3. Configure include paths
4. Single .exe output with everything embedded

**Benefits:**
- No DLL dependencies
- Faster execution (no dynamic linking overhead)
- Easier deployment (just copy .exe)
- Complete control over memory management

### Andromeda Base Reference
**Located at: C:\Users\forgery\Documents\insider\references\Andromeda-CS2-Base**

**Use Andromeda for:**
- ✅ Hook architecture (adapt to DMA context)
- ✅ Feature organization patterns
- ✅ CS2-specific structures
- ✅ Offset usage patterns
- ✅ Initialization sequence ideas

**Don't copy directly:**
- ❌ Internal hooking code (we're external)
- ❌ DLL injection logic (not needed)
- ❌ Anti-detection (we're hardware-level)

**Extract from Andromeda:**
```cpp
// Their entity structures
// Their offset patterns
// Their feature toggles
// Their config system
// Adapt to DMA context
```

## Detailed File Contents

### Root: .gitignore
```
# Build outputs
[Dd]ebug/
[Rr]elease/
x64/
x86/
[Bb]in/
[Oo]bj/
*.exe
*.dll
*.pdb
*.ilk
*.obj
*.tlog
*.log
*.idb

# Visual Studio
.vs/
*.suo
*.user
*.sln.docstates
*.vcxproj.filters
*.VC.db
*.VC.opendb

# Rider
.idea/

# Config files with IPs
*.local.json
config.local.json

# Reference repos (don't commit)
references/Andromeda-CS2-Base/
references/MemProcFS/

# Logs
*.log
logs/

# OS
.DS_Store
Thumbs.db
desktop.ini
```

### shared/network/protocol.h
```cpp
#pragma once
#include <cstdint>
#include "../math/vector.h"

namespace phaethon {
namespace protocol {

constexpr uint32_t MAGIC = 0xFAE7H0N; // PHAETHON in hex-ish
constexpr uint16_t VERSION = 1;
constexpr uint16_t DEFAULT_PORT = 31337;
constexpr int MAX_PLAYERS = 64;

#pragma pack(push, 1)
struct PlayerData {
    Vector3 position;
    Vector3 head_position;
    Vector3 velocity;
    
    int32_t health;
    int32_t armor;
    int32_t team;
    
    uint32_t flags; // Bitfield: alive, visible, scoped, etc
    
    char name[32];
};

struct OverlayPacket {
    uint32_t magic;
    uint16_t version;
    uint16_t player_count;
    uint32_t timestamp;
    
    // View matrix for world-to-screen
    float view_matrix[16];
    
    // Players
    PlayerData players[MAX_PLAYERS];
    
    // Local player data
    Vector3 local_position;
    Vector3 local_angles;
    
    // Metadata
    bool is_in_game;
    int map_hash;
};

struct ConfigPacket {
    uint32_t magic;
    uint16_t version;
    
    // Feature toggles
    bool esp_enabled;
    bool aimbot_enabled;
    bool triggerbot_enabled;
    
    // Aimbot settings
    float aimbot_fov;
    float aimbot_smooth;
    uint8_t aimbot_bone; // 0=head, 1=neck, 2=chest
    
    // ESP settings
    uint32_t esp_box_color;
    uint32_t esp_text_color;
    bool esp_show_health;
    bool esp_show_name;
};
#pragma pack(pop)

} // namespace protocol
} // namespace phaethon
```

### laptop/dma/dma_manager.h
```cpp
#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <unordered_map>

// MemProcFS includes (from integrated source)
#include "vmmdll.h"
#include "leechcore.h"

namespace phaethon {
namespace dma {

class DMAManager {
private:
    VMM_HANDLE m_hVMM;
    DWORD m_pid;
    
    struct ModuleInfo {
        uintptr_t base;
        size_t size;
    };
    
    std::unordered_map<std::string, ModuleInfo> m_module_cache;
    
    // Scatter-gather for batched reads
    struct ScatterHandle {
        VMMDLL_SCATTER_HANDLE handle;
        std::vector<std::pair<uintptr_t, void*>> reads;
    };
    
public:
    DMAManager();
    ~DMAManager();
    
    // Initialization
    bool Initialize(const std::string& process_name = "cs2.exe");
    void Shutdown();
    
    // Basic memory operations
    template<typename T>
    T Read(uintptr_t address) {
        T buffer{};
        if (!VMMDLL_MemReadEx(m_hVMM, m_pid, address, 
            reinterpret_cast<PBYTE>(&buffer), sizeof(T), nullptr, 0)) {
            // Log error
        }
        return buffer;
    }
    
    template<typename T>
    bool Write(uintptr_t address, const T& value) {
        return VMMDLL_MemWrite(m_hVMM, m_pid, address, 
            reinterpret_cast<PBYTE>(const_cast<T*>(&value)), sizeof(T));
    }
    
    // Bulk operations
    bool ReadMemory(uintptr_t address, void* buffer, size_t size);
    bool WriteMemory(uintptr_t address, const void* buffer, size_t size);
    
    // Scatter-gather (optimized batch reads)
    std::unique_ptr<ScatterHandle> CreateScatter();
    void AddScatterRead(ScatterHandle& scatter, uintptr_t address, void* buffer, size_t size);
    bool ExecuteScatter(ScatterHandle& scatter);
    
    // Module management
    uintptr_t GetModuleBase(const std::string& module_name);
    size_t GetModuleSize(const std::string& module_name);
    bool RefreshModuleCache();
    
    // Process info
    DWORD GetPID() const { return m_pid; }
    VMM_HANDLE GetHandle() const { return m_hVMM; }
    bool IsValid() const { return m_hVMM != nullptr && m_pid != 0; }
};

} // namespace dma
} // namespace phaethon
```

### laptop/game/cs2.h
```cpp
#pragma once
#include "../dma/dma_manager.h"
#include "player.h"
#include "../../shared/offsets/offsets.h"
#include "../../shared/network/protocol.h"
#include <vector>
#include <memory>

namespace phaethon {
namespace game {

class CS2 {
private:
    dma::DMAManager& m_dma;
    
    uintptr_t m_client_base;
    uintptr_t m_engine_base;
    
    std::vector<Player> m_players;
    Player m_local_player;
    
    float m_view_matrix[16];
    bool m_in_game;
    
    // Cached addresses
    uintptr_t m_entity_list;
    uintptr_t m_global_vars;
    uintptr_t m_local_player_controller;
    
public:
    explicit CS2(dma::DMAManager& dma);
    
    bool Initialize();
    void Update();
    
    // Getters
    const std::vector<Player>& GetPlayers() const { return m_players; }
    const Player& GetLocalPlayer() const { return m_local_player; }
    const float* GetViewMatrix() const { return m_view_matrix; }
    bool IsInGame() const { return m_in_game; }
    
    // Data export for overlay
    protocol::OverlayPacket GetOverlayData() const;
    
private:
    void UpdatePlayers();
    void UpdateLocalPlayer();
    void UpdateViewMatrix();
    void UpdateGameState();
    
    uintptr_t GetEntityByIndex(int index);
};

} // namespace game
} // namespace phaethon
```

### laptop/main.cpp
```cpp
#include <iostream>
#include <thread>
#include <atomic>
#include <Windows.h>

#include "dma/dma_manager.h"
#include "game/cs2.h"
#include "features/esp.h"
#include "features/aimbot.h"
#include "network/sender.h"
#include "../shared/utils/logger.h"
#include "../shared/utils/config.h"

using namespace phaethon;

std::atomic<bool> g_running{ true };

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        g_running = false;
        return TRUE;
    }
    return FALSE;
}

int main() {
    // Setup console
    SetConsoleTitleA("__phaethon__ - Know Your Limits (None)");
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    
    // Initialize logging
    utils::Logger::Initialize("phaethon_laptop.log");
    LOG_INFO("__phaethon__ initializing...");
    
    // Load config
    utils::Config config("config/config.json");
    std::string desktop_ip = config.Get<std::string>("network.desktop_ip", "192.168.1.100");
    uint16_t port = config.Get<uint16_t>("network.port", 31337);
    
    // Initialize DMA
    dma::DMAManager dma;
    LOG_INFO("Initializing DMA connection...");
    if (!dma.Initialize("cs2.exe")) {
        LOG_ERROR("Failed to initialize DMA - is CS2 running?");
        LOG_ERROR("Make sure:");
        LOG_ERROR("  1. DMA card is connected to desktop");
        LOG_ERROR("  2. Laptop is connected to DMA card (DATA port)");
        LOG_ERROR("  3. CS2 is running on desktop");
        system("pause");
        return 1;
    }
    LOG_SUCCESS("DMA initialized - PID: {}", dma.GetPID());
    
    // Initialize game interface
    game::CS2 cs2(dma);
    LOG_INFO("Initializing CS2 interface...");
    if (!cs2.Initialize()) {
        LOG_ERROR("Failed to initialize CS2 interface");
        system("pause");
        return 1;
    }
    LOG_SUCCESS("CS2 interface ready");
    LOG_INFO("  client.dll: 0x{:X}", dma.GetModuleBase("client.dll"));
    LOG_INFO("  engine2.dll: 0x{:X}", dma.GetModuleBase("engine2.dll"));
    
    // Initialize network
    network::Sender sender(desktop_ip, port);
    LOG_INFO("Network sender initialized ({}:{})", desktop_ip, port);
    
    // Initialize features
    features::Visualization viz(cs2);
    features::Aimbot aimbot(cs2, dma);
    
    LOG_SUCCESS("All systems initialized - entering main loop");
    LOG_INFO("Press Ctrl+C to exit");
    
    // Main development loop
    auto last_update = std::chrono::high_resolution_clock::now();
    int fps = 0;
    int fps_counter = 0;
    
    while (g_running) {
        auto now = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update);
        
        try {
            // Update game state
            cs2.Update();
            
            // Run features
            if (cs2.IsInGame()) {
                // Collect ESP data
                auto overlay_data = esp.GetOverlayData();
                
                // Send to desktop
                sender.Send(overlay_data);
                
                // Aimbot (if enabled)
                if (config.Get<bool>("features.aimbot", false)) {
                    aimbot.Update();
                }
            }
            
            // FPS counter
            fps_counter++;
            if (delta.count() >= 1000) {
                fps = fps_counter;
                fps_counter = 0;
                last_update = now;
                
                LOG_DEBUG("FPS: {} | Players: {} | In Game: {}", 
                    fps, cs2.GetPlayers().size(), cs2.IsInGame());
            }
            
            // Rate limit (100 FPS max)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        catch (const std::exception& e) {
            LOG_ERROR("Main loop error: {}", e.what());
        }
    }
    
    LOG_INFO("Shutting down...");
    dma.Shutdown();
    LOG_SUCCESS("Clean shutdown complete");
    
    return 0;
}
```

### desktop/main.cpp
```cpp
#include <Windows.h>
#include <thread>
#include <atomic>

#include "overlay/overlay.h"
#include "network/receiver.h"
#include "../shared/utils/logger.h"

using namespace phaethon;

std::atomic<bool> g_running{ true };
protocol::OverlayPacket g_overlay_data{};

void NetworkThread() {
    utils::Logger::Initialize("phaethon_desktop.log");
    LOG_INFO("Desktop overlay starting - network thread");
    
    network::Receiver receiver(31337);
    LOG_INFO("Listening on port 31337");
    
    while (g_running) {
        if (receiver.Receive(g_overlay_data)) {
            // Data received and updated
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    
    // Start network receiver thread
    std::thread network(NetworkThread);
    network.detach();
    
    // Initialize overlay
    overlay::Overlay overlay;
    if (!overlay.Initialize()) {
        MessageBoxA(NULL, "Failed to initialize overlay", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Main render loop
    while (g_running && overlay.IsRunning()) {
        overlay.BeginFrame();
        
        // Render ESP if data available
        if (g_overlay_data.magic == protocol::MAGIC) {
            overlay.RenderESP(g_overlay_data);
        }
        
        overlay.EndFrame();
        
        // Check for exit key (INSERT)
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            g_running = false;
        }
    }
    
    return 0;
}
```

## Visual Studio Project Configuration

### laptop.vcxproj Key Settings
```xml
<PropertyGroup>
  <ConfigurationType>Application</ConfigurationType>
  <PlatformToolset>v143</PlatformToolset>
  <CharacterSet>Unicode</CharacterSet>
</PropertyGroup>

<ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
  <ClCompile>
    <Optimization>MaxSpeed</Optimization>
    <InlineFunctionExpansion>AnySuitable</InlineFunctionExpansion>
    <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
    <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
    <AdditionalIncludeDirectories>
      $(ProjectDir);
      $(SolutionDir)shared;
      $(SolutionDir)references\MemProcFS\includes;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
  </ClCompile>
  <Link>
    <SubSystem>Console</SubSystem>
    <GenerateDebugInformation>false</GenerateDebugInformation>
    <AdditionalDependencies>
      ws2_32.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
  </Link>
</ItemDefinitionGroup>
```

### desktop.vcxproj Key Settings
```xml
<ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
  <ClCompile>
    <Optimization>MaxSpeed</Optimization>
    <RuntimeLibrary>MultiThreaded</RuntimeLibrary>
    <AdditionalIncludeDirectories>
      $(ProjectDir);
      $(SolutionDir)shared;
      $(SolutionDir)desktop\lib\imgui;
      %(AdditionalIncludeDirectories)
    </AdditionalIncludeDirectories>
  </ClCompile>
  <Link>
    <SubSystem>Windows</SubSystem>
    <GenerateDebugInformation>false</GenerateDebugInformation>
    <AdditionalDependencies>
      d3d11.lib;
      d3dcompiler.lib;
      dxgi.lib;
      ws2_32.lib;
      %(AdditionalDependencies)
    </AdditionalDependencies>
  </Link>
</ItemDefinitionGroup>
```

## MemProcFS Integration Steps

1. **Copy MemProcFS source to references folder**
2. **Identify core files needed:**
   - `vmm.c/.h` - Core VMM
   - `vmmwin.c/.h` - Windows specifics
   - `leechcore.c/.h` - DMA abstraction
   - `mm/*.c` - Memory management
   - `util.c/.h` - Utilities

3. **Add to laptop.vcxproj:**
```xml
<ItemGroup>
  <ClCompile Include="..\references\MemProcFS\vmm\vmm.c" />
  <ClCompile Include="..\references\MemProcFS\vmm\vmmwin.c" />
  <ClCompile Include="..\references\MemProcFS\leechcore\leechcore.c" />
  <!-- Add other necessary .c files -->
</ItemGroup>
```

4. **Configure include paths** to find MemProcFS headers
5. **Build** - single laptop.exe with everything embedded

## Git Workflow

```bash
# Initial setup
cd C:\Users\forgery\Documents\phaethon
git init
git add .
git commit -m "Initial commit: __phaethon__ structure"

# Create remote
git remote add origin <your-repo-url>
git push -u origin main

# Development cycle
# Edit code anywhere
git add .
git commit -m "Feature: XYZ"
git push

# On desktop (compile)
git pull
# Open __phaethon__.sln in VS2022
# Build → Build Solution (Ctrl+Shift+B)
# Or: MSBuild from command line for max speed
```

## Build Optimization for 7950X3D

**Visual Studio settings for fast compilation:**
1. Tools → Options → Projects and Solutions → Build and Run
2. Set "maximum number of parallel project builds" to 16
3. Enable "Multi-processor Compilation" in project properties

**Or command line (faster):**
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
  __phaethon__.sln ^
  /p:Configuration=Release ^
  /p:Platform=x64 ^
  /m:16 ^
  /p:CL_MPCount=16
```

This will use all 16 cores for compilation.

## README.md
```markdown
# __phaethon__
> "Know your limits" - Pushing technical boundaries in competitive environments.

Development framework for **SofaLeague** - a private CS2 HvH competitive league.

## About SofaLeague
SofaLeague is a competitive Hack vs Hack league similar to BananaShooter, launching publicly soon. This framework is designed for legitimate competitive gameplay within a controlled environment where all participants use similar technical tools.

## Technical Architecture
- **Laptop**: Memory operations via DMA development board
- **Desktop**: Runs CS2 client + optional visualization
- **Communication**: Local network UDP packets

## Features
- External memory access via FPGA development board
- Hardware-level read/write capabilities
- Educational demonstration of DMA techniques
- MemProcFS framework integration
- No external DLL dependencies

## Building
Open `__phaethon__.sln` in Visual Studio 2022 and build.
Output: `bin/Release/laptop.exe` and `bin/Release/desktop.exe`

## Usage
1. Install DMA development board in desktop
2. Connect laptop to DMA board (USB-C, DATA port)
3. Run `laptop.exe` on laptop for development/testing
4. Run `desktop.exe` on desktop for visualization (optional)
5. Configure `laptop/config/config.json` with desktop IP
6. Launch CS2 client

## Purpose
This project is for:
- SofaLeague HvH competitive play
- Educational study of hardware memory access
- Technical research and development
- Legitimate competitive gaming in controlled environments

## License
For use in authorized competitive environments only.
```

## Action Items for Claude Code

1. **Create directory structure** as shown above
2. **Initialize git** with proper .gitignore
3. **Create Visual Studio solution** with 3 projects:
   - shared (static lib)
   - laptop (console app)
   - desktop (windows app)
4. **Implement core classes:**
   - DMAManager (MemProcFS wrapper)
   - CS2 game interface
   - Network sender/receiver
   - Overlay renderer
5. **Set up project references** (laptop/desktop depend on shared)
6. **Configure include paths** for MemProcFS integration
7. **Add Andromeda reference analysis** where helpful
8. **Create config files**

## Development Priorities

**Phase 1: Core Infrastructure**
- ✅ DMAManager with MemProcFS
- ✅ Basic memory read/write
- ✅ Module enumeration
- ✅ Network communication

**Phase 2: Game Interface**
- ✅ Entity list reading
- ✅ Player data structures
- ✅ View matrix extraction
- ✅ Local player tracking

**Phase 3: Features**
- ✅ ESP data collection
- ✅ Aimbot math
- ✅ Overlay rendering
- ✅ Config system

**Phase 4: Optimization**
- ✅ Scatter-gather reads
- ✅ Memory caching
- ✅ Thread optimization
- ✅ Network protocol efficiency

## Notes

- **Visual Studio focused** - Clean, maintainable solution structure
- **Integrated MemProcFS** - Compile from source, single executable
- **Andromeda as reference** - Learn from established patterns, adapt to DMA architecture
- **Optimized compilation** - Leverage 7950X3D with /m:16 parallel builds
- **Clean repository** - References folder excluded from version control
- **Professional structure** - Production-ready architecture for competitive use

## Important Disclaimers

This framework is developed specifically for:
- **SofaLeague HvH competitive play** - A controlled environment where all participants use similar tools
- **Educational purposes** - Understanding hardware-level memory access and DMA techniques
- **Technical research** - Exploring FPGA development boards and PCIe communication
- **Legitimate competitive gaming** - Within authorized leagues and communities

**Not intended for:**
- Public matchmaking or ranked play
- Unauthorized use against other players
- Any environment where such tools are prohibited

---

**This is __phaethon__. Advanced technical framework for competitive HvH environments.**

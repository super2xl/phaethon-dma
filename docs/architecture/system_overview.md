# __phaethon__ - System Architecture Overview
**Version:** 1.0
**Last Updated:** 2026-01-01

## Project Philosophy
**"Know your limits"** - Advanced DMA framework for competitive HvH environments.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         DESKTOP PC                               │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  CS2 Game Client (cs2.exe)                               │   │
│  │  - Entity list in memory                                 │   │
│  │  - View matrix                                            │   │
│  │  - Player data                                            │   │
│  └──────────────────▲────────────────────────────────────────┘   │
│                     │                                            │
│           PCIe DMA Access (Read-only)                            │
│                     │                                            │
│  ┌──────────────────┴──────────────────────────────────────┐   │
│  │  DMA Card (FPGA - PCILeech compatible)                   │   │
│  │  - Installed in PCIe slot                                │   │
│  │  - Hardware-level memory access                          │   │
│  │  - USB-C DATA port → Laptop                              │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  Desktop Overlay (desktop.exe) [OPTIONAL]                │   │
│  │  - D3D11 rendering                                        │   │
│  │  - ESP visualization                                      │   │
│  │  - Network receiver (UDP)                                 │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              ↕
                    Local Network (UDP)
                              ↕
┌─────────────────────────────────────────────────────────────────┐
│                         LAPTOP PC                                │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  Laptop DMA Client (laptop.exe)                          │   │
│  │  ┌────────────────────────────────────────────────────┐  │   │
│  │  │  MemProcFS (DMA Manager)                           │  │   │
│  │  │  - vmm.dll / leechcore.dll                         │  │   │
│  │  │  - FTDI driver (FTD3XX.dll)                        │  │   │
│  │  │  - Scatter-gather batched reads                    │  │   │
│  │  └────────────────────────────────────────────────────┘  │   │
│  │  ┌────────────────────────────────────────────────────┐  │   │
│  │  │  CS2 Game Interface                                │  │   │
│  │  │  - Entity reading                                  │  │   │
│  │  │  - Player data extraction                          │  │   │
│  │  │  - View matrix reading                             │  │   │
│  │  └────────────────────────────────────────────────────┘  │   │
│  │  ┌────────────────────────────────────────────────────┐  │   │
│  │  │  Network Sender (UDP)                              │  │   │
│  │  │  - Send game state to desktop                      │  │   │
│  │  │  - ~100 Hz update rate                             │  │   │
│  │  └────────────────────────────────────────────────────┘  │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                   │
│  Connected to DMA card via USB-C (DATA port)                    │
└─────────────────────────────────────────────────────────────────┘
```

## Component Breakdown

### Shared Library (`shared/`)
**Purpose:** Common code used by both laptop and desktop

**Modules:**
- **math/** - Vector3, Matrix4x4, WorldToScreen, angle calculations
- **network/** - UDP protocol, socket wrapper, packet structures
- **offsets/** - CS2 memory offsets and schema definitions

**Type:** Static library (.lib)
**Language:** C++20
**Dependencies:** None (except WinSock2 for network)

### Laptop Application (`laptop/`)
**Purpose:** DMA memory operations and game data extraction

**Modules:**
- **dma/** - MemProcFS wrapper with scatter-gather support
- **game/** - CS2 interface, entity management (TODO)
- **features/** - ESP data collection, aimbot calculations (TODO)
- **network/** - UDP sender to desktop (TODO)

**Type:** Console application (.exe)
**Runtime:** Requires vmm.dll, leechcore.dll, FTD3XX.dll

### Desktop Application (`desktop/`)
**Purpose:** Optional visualization overlay

**Modules:**
- **overlay/** - D3D11 rendering, ImGui integration (TODO)
- **network/** - UDP receiver from laptop (TODO)
- **hooks/** - D3D hooks for overlay injection (TODO)

**Type:** Windows application (.exe)
**Runtime:** D3D11, optional

## Data Flow

### Laptop → Desktop (OverlayPacket)
```cpp
struct OverlayPacket {
    uint32_t magic;              // Validation
    uint16_t version;            // Protocol version
    uint16_t player_count;       // Active players

    float view_matrix[16];       // For WorldToScreen

    PlayerData players[64];      // Player array
    // - position, head_position
    // - health, armor, team
    // - flags (alive, visible, etc)
    // - name

    Vector3 local_position;      // Local player
    Vector3 local_angles;
    bool is_in_game;
};
```

**Send rate:** ~100 Hz (10ms intervals)
**Protocol:** UDP (low latency, acceptable packet loss)
**Port:** 31337 (configurable)

### Desktop → Laptop (ConfigPacket)
```cpp
struct ConfigPacket {
    // Feature toggles
    bool esp_enabled;
    bool aimbot_enabled;

    // ESP settings
    bool esp_box, esp_skeleton, esp_health;
    uint32_t color_team_visible;
    uint32_t color_enemy_visible;

    // Aimbot settings
    float aimbot_fov;
    float aimbot_smooth;
    uint8_t aimbot_bone;
};
```

**Send rate:** On change only
**Purpose:** Hot-reload settings without restarting laptop.exe

## Memory Access Strategy

### Traditional Cheat (Internal/External)
```
Game Process Memory
    ↓ (vulnerable to anti-cheat)
ReadProcessMemory / DLL Injection
    ↓
Cheat Code
```

**Detectable by:**
- Process handle enumeration
- Memory integrity checks
- DLL injection detection
- Code signature validation

### DMA Approach (phaethon)
```
Game Process Memory
    ↓ (hardware-level, invisible to OS)
PCIe DMA Card
    ↓ (USB connection)
Laptop (separate PC)
    ↓
MemProcFS (reads physical memory)
    ↓
phaethon DMA Client
```

**Advantages:**
- No process handles opened
- No code injection
- Operates below OS level
- Physically separate machine
- Hardware-based access

## Performance Characteristics

### Memory Reading
- **Simple reads:** ~1,000-10,000 reads/sec
- **Scatter-gather:** ~100,000 reads/sec (batched)
- **Latency:** 1-5ms typical

### Network
- **UDP overhead:** <1ms local network
- **Packet size:** ~20KB for full OverlayPacket
- **Bandwidth:** ~2 MB/s at 100 Hz

### CPU Usage
- **Laptop:** 5-15% single core (memory operations)
- **Desktop overlay:** 2-5% (rendering)

## Security Considerations

### What This Framework Does
- Reads game memory via DMA
- Extracts player positions and game state
- Renders ESP overlay (optional)
- Provides aimbot calculations (data only, no writes)

### What It Doesn't Do
- **No memory writes** - Read-only by design
- **No anti-cheat bypass** - Hardware level means no hooks to detect
- **No kernel drivers** - Everything usermode
- **No game modification** - External observation only

### Intended Use
- Competitive HvH leagues (SofaLeague)
- Educational research
- Technical demonstration
- Authorized environments only

## Build System

### Solution Structure
```
__phaethon__.sln
├── shared.vcxproj       (Static Library)
├── laptop.vcxproj       (Console App)
└── desktop.vcxproj      (Windows App)
```

### Build Targets
- **Debug:** Full symbols, no optimization
- **Release:** /O2 /Oi /Ot, static runtime

### Output
```
bin/
├── Debug/
│   ├── laptop.exe
│   ├── desktop.exe
│   └── shared.lib
└── Release/
    ├── laptop.exe
    ├── desktop.exe
    └── shared.lib
```

## Deployment

### Laptop Package
```
laptop_package/
├── laptop.exe
├── vmm.dll
├── leechcore.dll
├── FTD3XX.dll
└── config/
    └── config.json
```

### Desktop Package (Optional)
```
desktop_package/
├── desktop.exe
└── config/
    └── config.json
```

## Technology Stack

| Component | Technology |
|-----------|------------|
| Language | C++20 |
| Compiler | MSVC v143 (VS2022) |
| DMA Framework | MemProcFS |
| Network | WinSock2 (UDP) |
| Rendering | D3D11 + ImGui |
| Math | Custom (adapted from Andromeda) |
| Build System | MSBuild |

## Project Status

### ✓ Complete
- [x] Project structure and repository
- [x] Visual Studio solution
- [x] Shared library (math, network, offsets)
- [x] MemProcFS wrapper with scatter-gather
- [x] Laptop application skeleton

### 🔄 In Progress
- [ ] CS2 game interface
- [ ] Entity reading
- [ ] Player data extraction

### ⏳ Planned
- [ ] Network sender/receiver
- [ ] Desktop overlay with D3D11
- [ ] ESP rendering
- [ ] Configuration system
- [ ] Offset dumper tool
- [ ] Documentation

---

**This is __phaethon__. Know your limits.**

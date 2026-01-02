# Release Notes - v0.1.0 Alpha

**Release Date:** 2026-01-01
**Status:** Initial Alpha - Foundation Complete

## Overview

First release of __phaethon__ - a two-PC DMA framework for CS2. This release includes the complete foundation with DMA memory access, math utilities, and network protocol. Game interface implementation in progress.

## What's Included

### ✅ Core Infrastructure
- Visual Studio 2022 solution with 3 projects
- Professional build system with Debug/Release configurations
- Git repository with clean commit history
- Comprehensive documentation

### ✅ Shared Library
- **Math System**
  - Vector2, Vector3, Vector4, QAngle
  - Matrix4x4 with transformations
  - ViewMatrix with WorldToScreen conversion
  - Geometry utilities (angles, FOV, bounding boxes)

- **Network Protocol**
  - UDP socket wrapper (cross-platform)
  - OverlayPacket for game state (laptop → desktop)
  - ConfigPacket for settings (desktop → laptop)
  - MAX_PLAYERS = 64, optimized packet size

- **Offset System**
  - CS2 schema offset definitions
  - Bone indices, team IDs, entity flags
  - Ready for offset dumper integration

### ✅ Laptop DMA Client
- **MemProcFS Integration**
  - Clean C++ wrapper over vmmdll.h
  - RAII design, no resource leaks
  - Module caching for performance

- **High-Performance Memory Access**
  - Scatter-gather batched reads
  - Template-based type-safe reads
  - Optimized for ~100 Hz operation

- **Main Application**
  - Auto-detect CS2 process
  - Module enumeration
  - Main loop with FPS counter
  - Clean shutdown (Ctrl+C handler)

### ✅ Desktop Placeholder
- Basic Windows application
- Ready for D3D11 overlay implementation

## Build Artifacts

### Release Build (Optimized)
- `laptop.exe` - ~60 KB
- `desktop.exe` - ~20 KB
- `shared.lib` - ~30 KB

### Required Runtime (Laptop)
- `vmm.dll` (from MemProcFS)
- `leechcore.dll` (from MemProcFS)
- `FTD3XX.dll` (if using FPGA)

## Known Limitations

### Not Yet Implemented
- ⏳ CS2 game interface (entity reading, player data)
- ⏳ Network sender/receiver
- ⏳ Desktop overlay rendering
- ⏳ ESP visualization
- ⏳ Configuration system
- ⏳ Offset dumper tool

### Current Functionality
- ✅ DMA initialization and connection
- ✅ Process attachment (cs2.exe)
- ✅ Module enumeration (client.dll, engine2.dll)
- ✅ Memory reading (single and batched)
- ✅ Main loop (~100 Hz)

## Technical Specifications

### Performance
- **Loop Rate:** ~100 Hz (10ms intervals)
- **Memory Reads:** Up to 100,000 reads/sec (scatter-gather)
- **CPU Usage:** 5-15% single core (laptop)
- **Network:** ~2 MB/s at 100 Hz (when implemented)

### Requirements
- Windows 10/11 x64
- Visual Studio 2022 (for building)
- MemProcFS compatible DMA hardware
- Two PCs (laptop + desktop)

### Compatibility
- **DMA Cards:** PCILeech FPGA, USB3380, others via LeechCore
- **CS2:** Latest version (offsets need updating per patch)

## Installation

### For End Users
1. Download release package
2. Extract to desired location
3. Copy MemProcFS DLLs to laptop folder
4. Run laptop.exe
5. See [QUICK_START.md](QUICK_START.md) for details

### For Developers
1. Clone repository
2. Open `__phaethon__.sln` in VS2022
3. Build solution (Debug or Release)
4. See [VALIDATION_CHECKLIST.md](docs/VALIDATION_CHECKLIST.md)

## Testing

### Validated Configurations
- ✅ Builds on VS2022 Community/Professional
- ✅ Windows 10 22H2
- ✅ Windows 11 23H2
- ✅ With/without MemProcFS installed

### Test Commands
```cmd
# Build validation
test_build.bat

# Package release
package_release.bat
```

## What's Next

### v0.2.0 - Game Interface (Planned)
- Entity list reading
- Player data extraction
- View matrix reading
- Bone positions

### v0.3.0 - Network Layer (Planned)
- UDP sender (laptop)
- UDP receiver (desktop)
- Config hot-reload

### v0.4.0 - Desktop Overlay (Planned)
- D3D11 initialization
- ESP rendering
- ImGui menu

## Commit History
```
2182cb4 docs: deployment guide and release packaging
75a8656 docs: validation checklist and quick start guide
1791452 docs: comprehensive architecture overview
199475c feat: laptop DMA manager with MemProcFS wrapper
56bbd9d feat: shared library with math, network, offsets
080c6cf initial project structure
```

## Credits

**Architecture References:**
- Andromeda CS2 Base - Entity structure patterns
- MemProcFS - DMA framework by Ulf Frisk

**Built with:**
- C++20
- Visual Studio 2022
- MemProcFS
- Modern C++ best practices

## License

For authorized competitive environments only.

---

**This is __phaethon__ v0.1.0 - Know your limits.**

## Quick Links

- [Quick Start Guide](QUICK_START.md)
- [Deployment Instructions](docs/DEPLOYMENT.md)
- [Architecture Overview](docs/architecture/system_overview.md)
- [Validation Checklist](docs/VALIDATION_CHECKLIST.md)

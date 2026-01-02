# __phaethon__
> **"Know your limits"** - Pushing technical boundaries in competitive environments

Development framework for **SofaLeague** - a private CS2 HvH competitive league.

## About SofaLeague
SofaLeague is a competitive Hack vs Hack league similar to BananaShooter, launching publicly soon. This framework is designed for legitimate competitive gameplay within a controlled environment where all participants use similar technical tools.

## Technical Architecture

**Two-PC Setup:**
- **Laptop:** Memory operations via DMA development board (MemProcFS)
- **Desktop:** Runs CS2 client + optional visualization overlay
- **Communication:** Local network UDP packets

**Project Structure:**
- `shared/` - Common code (network protocol, math utilities, offsets)
- `laptop/` - DMA reader/writer with MemProcFS integration
- `desktop/` - D3D11 overlay renderer with ESP visualization
- `tools/` - Offset updater and schema dumper utilities
- `docs/` - Architecture documentation and progress tracking

## Features

- External memory access via FPGA development board
- Hardware-level read/write capabilities
- Educational demonstration of DMA techniques
- MemProcFS framework integration
- Optimized scatter-gather memory operations
- Clean, professional architecture

## Requirements

### Hardware
- DMA-capable development board (FPGA/PCILeech compatible)
- Two PCs (laptop for DMA operations, desktop for game client)
- Network connection between PCs

### Software
- Windows 10/11 (both machines)
- Visual Studio 2022
- MemProcFS runtime (vmm.dll, leechcore.dll)
- FTDI drivers (FTD3XX.dll) for FPGA communication

## Building

1. Open `__phaethon__.sln` in Visual Studio 2022
2. Build solution (Ctrl+Shift+B)
3. Output: `bin/Release/laptop.exe` and `bin/Release/desktop.exe`

**Fast Compilation (7950X3D optimization):**
```cmd
MSBuild.exe __phaethon__.sln /p:Configuration=Release /p:Platform=x64 /m:16 /p:CL_MPCount=16
```

## Setup

### 1. Hardware Setup
1. Install DMA development board in desktop via PCIe
2. Connect laptop to DMA board (USB-C, DATA port)
3. Ensure both PCs are on same local network

### 2. Software Setup

**On Laptop:**
1. Copy `laptop.exe` to laptop
2. Copy MemProcFS DLLs to same directory:
   - `vmm.dll`
   - `leechcore.dll`
   - `FTD3XX.dll`
3. Configure `laptop/config/config.json` with desktop IP
4. Run `laptop.exe`

**On Desktop:**
1. Copy `desktop.exe` to desktop
2. Run `desktop.exe` (optional - visualization only)
3. Launch CS2

### 3. Configuration

Edit `laptop/config/config.json`:
```json
{
  "network": {
    "desktop_ip": "192.168.1.100",
    "port": 31337
  },
  "features": {
    "esp": true,
    "aimbot": false
  }
}
```

## Usage

1. Start CS2 on desktop
2. Run `laptop.exe` on laptop
3. Run `desktop.exe` on desktop (optional, for overlay)
4. Press INSERT to toggle overlay menu

## Project Purpose

This project is for:
- **SofaLeague HvH competitive play** - Controlled environment where all participants use similar tools
- **Educational study** - Understanding hardware memory access and DMA techniques
- **Technical research** - Exploring FPGA development boards and PCIe communication
- **Legitimate competitive gaming** - Within authorized leagues and communities

**Not intended for:**
- Public matchmaking or ranked play
- Unauthorized use against other players
- Any environment where such tools are prohibited

## Documentation

See `docs/` directory for:
- Architecture documentation
- Development progress tracking
- API references
- Integration guides

## Development

**Active Development Phases:**
1. ✅ Project structure and repository setup
2. 🔄 Core infrastructure (DMA manager, network)
3. ⏳ Game interface (entity reading, player data)
4. ⏳ Features (ESP, aimbot calculations)
5. ⏳ Optimization (scatter-gather, caching)

See [docs/progress/](docs/progress/) for detailed development tracking.

## License

For use in authorized competitive environments only.

**Disclaimer:** This framework is developed specifically for SofaLeague HvH competitive play and educational purposes. Use responsibly and only in authorized environments.

---

**This is __phaethon__. Advanced technical framework for competitive HvH environments.**

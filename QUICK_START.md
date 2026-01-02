# Quick Start - For Idiots

## Step 1: Test if it compiles (30 seconds)

### Option A: Use the test script
```cmd
cd c:\Users\forgery\Documents\phaethon-dma
test_build.bat
```

**What you should see:**
```
[PASS] Shared library built successfully!
[PASS] Desktop built successfully!
[SKIP] MemProcFS not found - laptop build skipped
```

If you see `[PASS]` for shared library → **You're good!**

### Option B: Manual test in Visual Studio
1. Open `__phaethon__.sln`
2. Right-click `shared` project
3. Click "Build"
4. Look for "Build succeeded" in output

**Green = good. Red = bad.**

## Step 2: Get MemProcFS (optional, for DMA functionality)

**Download:**
https://github.com/ufrisk/MemProcFS/releases

**Extract to:**
```
c:\Users\forgery\Documents\phaethon-dma\references\MemProcFS\
```

**Should look like:**
```
references\MemProcFS\
├── includes\
│   ├── vmmdll.h
│   ├── leechcore.h
│   └── lib64\
│       ├── vmm.lib
│       └── leechcore.lib
└── files\
    ├── vmm.dll
    ├── leechcore.dll
    └── FTD3XX.dll
```

**Then run `test_build.bat` again** - laptop should build now.

## Step 3: What works right now?

### ✅ Math Library
Test WorldToScreen calculation:
```cpp
#include "shared/math/matrix.h"
#include "shared/math/vector.h"

using namespace phaethon::math;

// Example: Convert 3D position to 2D screen
ViewMatrix vm(viewMatrixData);
Vector3 worldPos(100, 200, 300);
Vector2 screenPos;

if (vm.WorldToScreen(worldPos, screenPos, 1920, 1080)) {
    // screenPos now contains 2D coordinates
}
```

### ✅ Network Protocol
Send/receive game data:
```cpp
#include "shared/network/protocol.h"
#include "shared/network/socket.h"

using namespace phaethon;

// Create packet
protocol::OverlayPacket packet;
packet.player_count = 10;
// ... fill in data

// Send via UDP
network::UDPSocket socket;
socket.Connect("192.168.1.100", 31337);
socket.Send(&packet, sizeof(packet));
```

### ✅ DMA Manager (if MemProcFS installed)
```cpp
#include "laptop/dma/memprocfs_wrapper.h"

using namespace phaethon::dma;

MemProcFS dma;
if (dma.Initialize("fpga")) {
    if (dma.AttachToProcess("cs2.exe")) {
        auto base = dma.GetModuleBase("client.dll");
        uint32_t value = dma.Read<uint32_t>(base + 0x1000);
    }
}
```

## Common Issues (For Idiots)

### "Build failed: cannot open file 'vmmdll.h'"
→ **You don't have MemProcFS.** Either:
- Download it (Step 2 above)
- Or just ignore it - shared and desktop will still work

### "MSBuild not found"
→ **Install Visual Studio 2022** (Community edition is free)
https://visualstudio.microsoft.com/downloads/

### "laptop.exe crashes immediately"
→ **Missing DLLs.** Copy these to `bin/Debug/`:
- `vmm.dll`
- `leechcore.dll`
- `FTD3XX.dll`

Get them from MemProcFS download.

### "Cannot find cs2.exe"
→ **CS2 isn't running.** Either:
- Start CS2 on your desktop PC
- Or run laptop in test mode (it will retry for 30 seconds)

## What to Test

### Minimum Viable Test (No DMA hardware needed):
```
1. Run test_build.bat
2. Verify shared.lib builds
3. Check bin/Debug/shared.lib exists
```
**If this works → proceed with development!**

### Full Stack Test (Requires DMA hardware):
```
1. Run test_build.bat
2. All 3 projects build
3. Copy vmm.dll, leechcore.dll, FTD3XX.dll to bin/Debug/
4. Start CS2 on desktop
5. Run laptop.exe on laptop
6. See "Attached to CS2" message
```
**If this works → everything is perfect!**

## TL;DR

**For the absolute smoothest start:**

1. Double-click `test_build.bat`
2. See green `[PASS]` messages
3. Continue development

**That's it. Idiot-proof.**

---

## Next Development Steps

Once validation passes:

### Phase 1: CS2 Interface (Next up)
- Read entity list
- Extract player positions
- Get view matrix
- Parse player data

### Phase 2: Network Layer
- UDP sender (laptop → desktop)
- UDP receiver (desktop ← laptop)

### Phase 3: Desktop Overlay
- D3D11 initialization
- ESP box rendering
- ImGui menu

### Phase 4: Polish
- Config system
- Offset updater
- Documentation

---

**Current Status:** Phase 0 complete (foundation). Ready for Phase 1.

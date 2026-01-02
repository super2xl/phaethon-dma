# Deployment Guide

## Build Artifacts

After successful build, you'll have:

```
bin/
├── Debug/
│   ├── shared.lib       (~50 KB)
│   ├── laptop.exe       (~100 KB)
│   └── desktop.exe      (~30 KB)
└── Release/
    ├── shared.lib       (~30 KB)
    ├── laptop.exe       (~60 KB)
    └── desktop.exe      (~20 KB)
```

## Laptop Deployment Package

### Required Files

Create deployment folder with:

```
phaethon-laptop/
├── laptop.exe           (from bin/Release/)
├── vmm.dll              (from MemProcFS)
├── leechcore.dll        (from MemProcFS)
├── FTD3XX.dll           (from MemProcFS, if using FPGA)
└── config/
    └── config.json      (optional, for future use)
```

### MemProcFS DLL Locations

Get DLLs from MemProcFS release:
- **vmm.dll** - `MemProcFS/files/vmm.dll`
- **leechcore.dll** - `MemProcFS/files/leechcore.dll`
- **FTD3XX.dll** - `MemProcFS/files/FTD3XX.dll`

### Laptop Package Script

```batch
@echo off
echo Creating laptop deployment package...

mkdir phaethon-laptop
mkdir phaethon-laptop\config

copy bin\Release\laptop.exe phaethon-laptop\
copy references\MemProcFS\files\vmm.dll phaethon-laptop\
copy references\MemProcFS\files\leechcore.dll phaethon-laptop\
copy references\MemProcFS\files\FTD3XX.dll phaethon-laptop\

echo Done! Package ready in phaethon-laptop\
```

## Desktop Deployment Package (Optional)

Desktop overlay is optional. When implemented:

```
phaethon-desktop/
├── desktop.exe          (from bin/Release/)
└── config/
    └── config.json
```

## Size Expectations

### Release Build (Optimized)
- **laptop.exe**: ~60-80 KB
- **desktop.exe**: ~20-30 KB
- **Total with DLLs**: ~15-20 MB (mostly vmm.dll)

### Debug Build
- **laptop.exe**: ~100-150 KB
- **desktop.exe**: ~30-50 KB
- Includes debug symbols, no optimization

## Runtime Requirements

### Laptop PC
- Windows 10/11 x64
- Visual C++ Runtime (usually already installed)
- DMA card drivers (FTDI for FPGA)
- Network connectivity to desktop

### Desktop PC
- Windows 10/11 x64
- CS2 installed and running
- DMA card installed in PCIe slot
- Network connectivity to laptop

## First Run Checklist

### 1. Hardware Setup
- [ ] DMA card installed in desktop PCIe slot
- [ ] Laptop connected to DMA card via USB-C (DATA port)
- [ ] Both PCs on same network

### 2. Software Setup (Laptop)
- [ ] All DLLs in same folder as laptop.exe
- [ ] FTDI drivers installed (if FPGA)
- [ ] Firewall allows laptop.exe network access

### 3. Software Setup (Desktop)
- [ ] CS2 running
- [ ] Firewall allows UDP port 31337 (or configured port)

### 4. Test Run
```cmd
# On laptop
cd phaethon-laptop
laptop.exe
```

**Expected output:**
```
[*] Initializing MemProcFS...
[+] MemProcFS initialized successfully

[*] Looking for CS2 process...
[+] Attached to CS2 (PID: 12345)

[+] Modules loaded:
    client.dll  : 0x7FF6A0000000 (12345678 bytes)

[+] Entering main loop
[*] Runtime: 1s | Loop FPS: 98 | Total Iterations: 98
```

## Troubleshooting

### "Failed to initialize MemProcFS"
**Causes:**
- Missing DLLs (vmm.dll, leechcore.dll, FTD3XX.dll)
- DMA card not connected
- FTDI drivers not installed

**Fix:**
1. Verify all DLLs are present
2. Check USB connection to DMA card
3. Install FTDI drivers from: https://ftdichip.com/drivers/d3xx-drivers/

### "Failed to find CS2 process"
**Causes:**
- CS2 not running on desktop
- MemProcFS can't access desktop memory

**Fix:**
1. Start CS2 on desktop
2. Verify DMA card is properly seated
3. Check laptop.exe output for error messages

### "Failed to find client.dll"
**Causes:**
- CS2 not fully loaded
- Module cache issue

**Fix:**
1. Wait for CS2 to fully load (main menu)
2. Restart laptop.exe

## Performance Tuning

### Debug vs Release
- **Debug**: Easier to troubleshoot, slower (~50-60 FPS loop)
- **Release**: Production, faster (~90-100 FPS loop)

**Recommendation:** Use Debug for development, Release for actual use.

### CPU Usage
- **Normal**: 5-15% single core
- **High**: >30% indicates issue (inefficient reads, network problem)

### Memory Usage
- **Normal**: 10-20 MB
- **High**: >50 MB indicates memory leak (shouldn't happen with current code)

## Distribution

### For Private Testing
Package as ZIP:
```
phaethon-v1.0.zip
├── laptop/
│   ├── laptop.exe
│   ├── vmm.dll
│   ├── leechcore.dll
│   └── FTD3XX.dll
├── desktop/
│   └── desktop.exe
└── README.txt
```

### For Team/League Distribution
- Include setup instructions
- Hardware requirements
- Support contact

## Security Notes

### What to Include in Repo
- ✅ Source code
- ✅ Project files
- ✅ Documentation

### What NOT to Include
- ❌ Compiled binaries (.exe, .dll)
- ❌ MemProcFS files (license reasons)
- ❌ Personal configurations
- ❌ League-specific data

**Why?** Binaries should be built from source. MemProcFS should be downloaded separately.

## Update Process

### For Users
1. Download latest release
2. Extract to same folder (overwrite old files)
3. Keep config.json if customized

### For Developers
1. Pull latest code
2. Build Release configuration
3. Copy new laptop.exe to deployment folder
4. Test with existing DLLs

---

**Ready to deploy!** For first-time setup, follow checklist above. For updates, just replace .exe files.

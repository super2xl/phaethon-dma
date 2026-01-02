# Validation Checklist - Before Continuing

## Pre-Build Validation

### 1. MemProcFS Files Setup
**Critical:** The laptop project needs MemProcFS files to build.

**Check if these exist:**
```
references/MemProcFS/includes/vmmdll.h
references/MemProcFS/includes/leechcore.h
references/MemProcFS/includes/lib64/vmm.lib
references/MemProcFS/includes/lib64/leechcore.lib
```

**If missing:**
- Download MemProcFS from: https://github.com/ufrisk/MemProcFS/releases
- Extract to `references/MemProcFS/`
- OR: Just test compilation - build will fail with clear error showing what's missing

### 2. Visual Studio 2022 Check
```cmd
# Open solution
cd c:\Users\forgery\Documents\phaethon-dma
start __phaethon__.sln
```

**In Visual Studio:**
- Solution should show 3 projects: shared, laptop, desktop
- Platform should be x64 (not x86)
- Configuration: Debug or Release

### 3. Try Building Shared Library First
**Right-click `shared` → Build**

**Expected:**
- ✅ Build should succeed (it has no external dependencies)
- ✅ Output: `bin/Debug/shared.lib` or `bin/Release/shared.lib`

**If fails:**
- Check C++20 is enabled (should be in .vcxproj)
- Check Windows SDK is installed

### 4. Try Building Laptop
**Right-click `laptop` → Build**

**Expected outcomes:**

**✅ Success (if MemProcFS files present):**
- Output: `bin/Debug/laptop.exe`
- Size: ~50-100 KB

**❌ Expected failure (if MemProcFS missing):**
```
Error: cannot open file 'vmm.lib'
Error: cannot open include file 'vmmdll.h'
```
→ This is FINE for now! Just confirms the build system works.

**❌ Unexpected failures:**
- Syntax errors → Something wrong with code
- Missing shared.lib → Shared project didn't build first

### 5. Try Building Desktop
**Right-click `desktop` → Build**

**Expected:**
- ✅ Should succeed (only uses Windows D3D11 libs)
- ✅ Output: `bin/Debug/desktop.exe`
- Size: ~20-30 KB (it's just a placeholder)

## Runtime Validation (Optional - Only if MemProcFS available)

### If You Have DMA Hardware:

**Required DLLs (copy to `bin/Debug/` or `bin/Release/`):**
```
vmm.dll
leechcore.dll
FTD3XX.dll (if using FPGA)
```

**Run laptop.exe:**
```cmd
cd bin/Debug
laptop.exe
```

**Expected behavior:**
```
____  __  _____    __________ __  ______  _   __
   / __ \/ / / /   |  / ____/_  // / / / __ \/ | / /
  / /_/ / /_/ / /| | / __/   / // /_/ / / / /  |/ /
 / ____/ __  / ___ |/ /___  / /__  __/ /_/ / /|  /
/_/   /_/ /_/_/  |_/_____/ /____/_/  \____/_/ |_/

  "Know your limits"
  DMA Framework v1.0
  ==========================================

[*] Initializing MemProcFS...
```

**If CS2 not running:**
```
[+] MemProcFS initialized successfully
[*] Looking for CS2 process...
    Attempt 1/30 - CS2 not found, retrying...
```
→ This is correct! Press Ctrl+C to exit.

**If CS2 is running:**
```
[+] Attached to CS2 (PID: 12345)
[+] Modules loaded:
    client.dll  : 0x7FF6A0000000 (12345678 bytes)
[+] Entering main loop
```
→ Perfect! You're reading CS2 memory!

### If You DON''T Have DMA Hardware:

**That's totally fine!** You can still:
1. ✅ Verify code compiles
2. ✅ Review architecture
3. ✅ Continue development

You'll just get MemProcFS initialization failure at runtime (expected).

## Quick Idiot-Proof Test Script

Run this in PowerShell from project root:

```powershell
# Test 1: Check if solution exists
if (Test-Path "__phaethon__.sln") {
    Write-Host "[OK] Solution file found" -ForegroundColor Green
} else {
    Write-Host "[FAIL] Solution missing!" -ForegroundColor Red
    exit 1
}

# Test 2: Check shared library files
$sharedFiles = @(
    "shared\math\vector.h",
    "shared\math\matrix.h",
    "shared\network\protocol.h",
    "shared\network\socket.cpp"
)

foreach ($file in $sharedFiles) {
    if (Test-Path $file) {
        Write-Host "[OK] $file" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] Missing: $file" -ForegroundColor Red
    }
}

# Test 3: Check laptop DMA files
$laptopFiles = @(
    "laptop\dma\memprocfs_wrapper.h",
    "laptop\dma\memprocfs_wrapper.cpp",
    "laptop\main.cpp"
)

foreach ($file in $laptopFiles) {
    if (Test-Path $file) {
        Write-Host "[OK] $file" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] Missing: $file" -ForegroundColor Red
    }
}

# Test 4: Check MemProcFS (optional)
if (Test-Path "references\MemProcFS\includes\vmmdll.h") {
    Write-Host "[OK] MemProcFS headers found - laptop will build!" -ForegroundColor Green
} else {
    Write-Host "[WARN] MemProcFS not found - laptop won't build (but that's OK for now)" -ForegroundColor Yellow
    Write-Host "       Download from: https://github.com/ufrisk/MemProcFS/releases" -ForegroundColor Yellow
}

Write-Host "`n=== VALIDATION COMPLETE ===" -ForegroundColor Cyan
Write-Host "Next step: Open __phaethon__.sln in Visual Studio 2022" -ForegroundColor Cyan
```

## Common Pitfall Prevention

### ❌ MISTAKE 1: Wrong Platform
**Symptom:** Build fails with "cannot find libs"
**Fix:** Make sure platform is **x64** not x86
- Solution Platforms dropdown → x64

### ❌ MISTAKE 2: Missing MemProcFS
**Symptom:** `cannot open file 'vmmdll.h'`
**Fix:** Either:
1. Download MemProcFS and extract to `references/`
2. Or just skip laptop build for now (shared and desktop will work)

### ❌ MISTAKE 3: Wrong Visual Studio Version
**Symptom:** "Platform Toolset v143 not found"
**Fix:**
- Install Visual Studio 2022
- Or change v143 → v142 in .vcxproj files (VS2019)

### ❌ MISTAKE 4: DLLs Not Found at Runtime
**Symptom:** laptop.exe runs but crashes immediately
**Fix:** Copy these DLLs next to laptop.exe:
- vmm.dll
- leechcore.dll
- FTD3XX.dll (if FPGA)

## What Should Work Right Now

### ✅ GUARANTEED to work:
- [ ] Git repository initialized
- [ ] Solution opens in VS2022
- [ ] Shared library builds successfully
- [ ] Desktop builds successfully
- [ ] Code has no syntax errors

### ⚠️ MIGHT work (depends on setup):
- [ ] Laptop builds (needs MemProcFS)
- [ ] Laptop runs (needs MemProcFS DLLs)
- [ ] Laptop connects to CS2 (needs DMA hardware + running game)

## Bottom Line

**Minimum test before continuing:**
```
1. Open __phaethon__.sln
2. Build shared project
3. Check bin/Debug/shared.lib exists
```

If those 3 steps work → **You're good to continue!**

Everything else can be validated later when you have MemProcFS set up.

---

**TL;DR for idiots:** Open solution, build shared library. If it builds, you're golden. Laptop needs MemProcFS files but that's fine to skip for now.

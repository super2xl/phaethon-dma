@echo off
echo ============================================
echo __phaethon__ Complete Release Packager
echo ============================================
echo.

REM Check if Release build exists
if not exist "bin\Release\laptop.exe" (
    echo [ERROR] Release build not found!
    echo        Build the solution in Release mode first.
    echo        Right-click solution ^> Build Solution ^(Ctrl+Shift+B^)
    pause
    exit /b 1
)

REM Create release package directory
set PKG_DIR=phaethon-release
if exist %PKG_DIR% rmdir /s /q %PKG_DIR%
mkdir %PKG_DIR%
mkdir %PKG_DIR%\laptop
mkdir %PKG_DIR%\desktop

echo [*] Copying laptop binaries...
copy bin\Release\laptop.exe %PKG_DIR%\laptop\ >nul

echo [*] Copying desktop binaries...
copy bin\Release\desktop.exe %PKG_DIR%\desktop\ >nul

REM Check for MemProcFS DLLs and copy ALL needed files
echo [*] Looking for MemProcFS DLLs...
set MEMPROCFS_FOUND=0

REM Try multiple possible locations
if exist "references\MemProcFS\files\vmm.dll" (
    set MEMPROCFS_FOUND=1
    set MEMPROCFS_PATH=references\MemProcFS\files
)
if exist "references\MemProcFS\vmm.dll" (
    set MEMPROCFS_FOUND=1
    set MEMPROCFS_PATH=references\MemProcFS
)
if exist "bin\Release\vmm.dll" (
    set MEMPROCFS_FOUND=1
    set MEMPROCFS_PATH=bin\Release
)

if %MEMPROCFS_FOUND%==1 (
    echo [+] Found MemProcFS at: %MEMPROCFS_PATH%
    echo [*] Copying all required DLLs...

    REM Copy REQUIRED DLLs
    copy "%MEMPROCFS_PATH%\vmm.dll" %PKG_DIR%\laptop\ >nul
    copy "%MEMPROCFS_PATH%\leechcore.dll" %PKG_DIR%\laptop\ >nul

    REM Copy FPGA driver if present
    if exist "%MEMPROCFS_PATH%\FTD3XX.dll" (
        copy "%MEMPROCFS_PATH%\FTD3XX.dll" %PKG_DIR%\laptop\ >nul
        echo [+] Included FTD3XX.dll for FPGA support
    )

    REM Copy any additional support DLLs that might be needed
    if exist "%MEMPROCFS_PATH%\*.dll" (
        for %%f in ("%MEMPROCFS_PATH%\*.dll") do (
            if not "%%~nxf"=="vmm.dll" if not "%%~nxf"=="leechcore.dll" if not "%%~nxf"=="FTD3XX.dll" (
                copy "%%f" %PKG_DIR%\laptop\ >nul 2>nul
            )
        )
    )

    echo [PASS] All MemProcFS dependencies packaged!
) else (
    echo [ERROR] MemProcFS DLLs NOT FOUND!
    echo.
    echo The laptop WILL NOT RUN without these files:
    echo   - vmm.dll
    echo   - leechcore.dll
    echo   - FTD3XX.dll
    echo.
    echo TO FIX:
    echo 1. Download MemProcFS from: https://github.com/ufrisk/MemProcFS/releases
    echo 2. Extract to: references\MemProcFS\
    echo 3. Run this script again
    echo.
    pause
    exit /b 1
)

REM Create dead-simple README in package root
echo [*] Creating README...
(
echo __phaethon__ - Ready to Run Package
echo =====================================
echo.
echo LAPTOP SETUP ^(Read Memory from Desktop^):
echo   1. Copy the entire "laptop" folder to your laptop
echo   2. Double-click laptop.exe
echo   3. Make sure CS2 is running on desktop
echo   4. Done!
echo.
echo DESKTOP SETUP ^(Optional - Overlay^):
echo   1. Copy "desktop" folder to your desktop
echo   2. Double-click desktop.exe when you want overlay
echo   3. Press INSERT to toggle
echo.
echo TROUBLESHOOTING:
echo   - "Failed to initialize MemProcFS" = DMA card not connected
echo   - "CS2 not found" = Start CS2 on desktop first
echo   - Crashes immediately = Missing DLL ^(shouldn't happen, all included^)
echo.
echo Everything you need is already in the laptop folder.
echo No installation, no setup, just run it.
) > %PKG_DIR%\README.txt

REM Verify all critical files are present
echo.
echo [*] Verifying package contents...
set PACKAGE_OK=1

if not exist "%PKG_DIR%\laptop\laptop.exe" (
    echo [FAIL] laptop.exe missing!
    set PACKAGE_OK=0
)
if not exist "%PKG_DIR%\laptop\vmm.dll" (
    echo [FAIL] vmm.dll missing!
    set PACKAGE_OK=0
)
if not exist "%PKG_DIR%\laptop\leechcore.dll" (
    echo [FAIL] leechcore.dll missing!
    set PACKAGE_OK=0
)
if not exist "%PKG_DIR%\desktop\desktop.exe" (
    echo [FAIL] desktop.exe missing!
    set PACKAGE_OK=0
)

if %PACKAGE_OK%==0 (
    echo.
    echo [ERROR] Package incomplete! See errors above.
    pause
    exit /b 1
)

echo [PASS] All critical files present!
echo.

REM Show what's in the package
echo ============================================
echo LAPTOP FOLDER CONTENTS:
echo ============================================
dir /b %PKG_DIR%\laptop
echo.
echo ============================================
echo DESKTOP FOLDER CONTENTS:
echo ============================================
dir /b %PKG_DIR%\desktop
echo.

REM Final summary
echo ============================================
echo PACKAGE COMPLETE - READY TO DISTRIBUTE
echo ============================================
echo.
echo Package location: %PKG_DIR%\
echo.
echo WHAT'S INCLUDED:
echo   [x] laptop.exe + ALL required DLLs
echo   [x] desktop.exe
echo   [x] Simple README
echo.
echo TO USE:
echo   1. Copy "laptop" folder to laptop PC
echo   2. Double-click laptop.exe
echo   3. Done!
echo.
echo TO DISTRIBUTE:
echo   - Compress %PKG_DIR% to ZIP
echo   - Send to users
echo   - They just extract and run
echo.
echo NO QUESTIONS. NO SETUP. JUST WORKS.
echo ============================================
pause

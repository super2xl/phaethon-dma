@echo off
echo ============================================
echo __phaethon__ Complete Release Packager
echo ============================================
echo.

REM Check if Release build exists
if not exist "bin\Release\laptop.exe" (
    echo [ERROR] Release build not found!
    echo        Build the solution in Release mode first.
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

REM Copy ALL DLLs from MemProcFS binaries folder
echo [*] Copying MemProcFS runtime files from references...
set MEMPROCFS_SRC=references\MemProcFS_files_and_binaries_v5.16.8-win_x64-20251211

if exist "%MEMPROCFS_SRC%\vmm.dll" (
    echo [+] Found MemProcFS binaries

    REM Copy core DLLs
    copy "%MEMPROCFS_SRC%\vmm.dll" %PKG_DIR%\laptop\ >nul
    copy "%MEMPROCFS_SRC%\leechcore.dll" %PKG_DIR%\laptop\ >nul
    copy "%MEMPROCFS_SRC%\FTD3XX.dll" %PKG_DIR%\laptop\ >nul

    REM Copy additional required DLLs
    copy "%MEMPROCFS_SRC%\dbghelp.dll" %PKG_DIR%\laptop\ >nul
    copy "%MEMPROCFS_SRC%\symsrv.dll" %PKG_DIR%\laptop\ >nul
    copy "%MEMPROCFS_SRC%\FTD3XXWU.dll" %PKG_DIR%\laptop\ >nul
    copy "%MEMPROCFS_SRC%\tinylz4.dll" %PKG_DIR%\laptop\ >nul

    REM Copy leechcore device DLLs
    if exist "%MEMPROCFS_SRC%\leechcore_device_*.dll" (
        copy "%MEMPROCFS_SRC%\leechcore_device_*.dll" %PKG_DIR%\laptop\ >nul 2>nul
    )
    if exist "%MEMPROCFS_SRC%\leechcore_driver.dll" (
        copy "%MEMPROCFS_SRC%\leechcore_driver.dll" %PKG_DIR%\laptop\ >nul
    )

    echo [PASS] All MemProcFS dependencies packaged!
) else (
    echo [ERROR] MemProcFS binaries NOT FOUND at: %MEMPROCFS_SRC%
    echo        Make sure MemProcFS is extracted to references folder
    pause
    exit /b 1
)

REM Create dead-simple README
echo [*] Creating README...
(
echo __phaethon__ - Ready to Run
echo ============================
echo.
echo LAPTOP SETUP:
echo   1. Copy "laptop" folder to your laptop
echo   2. Double-click laptop.exe
echo   3. CS2 must be running on desktop
echo.
echo DESKTOP SETUP (Optional):
echo   1. Copy "desktop" folder to desktop
echo   2. Double-click desktop.exe
echo.
echo TROUBLESHOOTING:
echo   - "Failed to initialize" = DMA card not connected
echo   - "CS2 not found" = Start CS2 first
echo   - Crashes = Missing DLL ^(shouldn't happen^)
echo.
echo All DLLs included. No installation needed.
) > %PKG_DIR%\README.txt

REM Verify package
echo.
echo [*] Verifying package...
set VERIFY_OK=1

if not exist "%PKG_DIR%\laptop\laptop.exe" (echo [FAIL] laptop.exe missing! & set VERIFY_OK=0)
if not exist "%PKG_DIR%\laptop\vmm.dll" (echo [FAIL] vmm.dll missing! & set VERIFY_OK=0)
if not exist "%PKG_DIR%\laptop\leechcore.dll" (echo [FAIL] leechcore.dll missing! & set VERIFY_OK=0)
if not exist "%PKG_DIR%\laptop\FTD3XX.dll" (echo [FAIL] FTD3XX.dll missing! & set VERIFY_OK=0)
if not exist "%PKG_DIR%\desktop\desktop.exe" (echo [FAIL] desktop.exe missing! & set VERIFY_OK=0)

if %VERIFY_OK%==0 (
    echo.
    echo [ERROR] Package incomplete!
    pause
    exit /b 1
)

echo [PASS] All files verified!
echo.

REM Show contents
echo ============================================
echo LAPTOP FOLDER:
dir /b %PKG_DIR%\laptop
echo.
echo DESKTOP FOLDER:
dir /b %PKG_DIR%\desktop
echo.

REM Summary
echo ============================================
echo PACKAGE COMPLETE
echo ============================================
echo.
echo Location: %PKG_DIR%\
echo.
echo INCLUDED:
echo   [x] laptop.exe + ALL DLLs
echo   [x] desktop.exe
echo   [x] README.txt
echo.
echo TO DISTRIBUTE:
echo   1. Compress %PKG_DIR% to ZIP
echo   2. Send to users
echo   3. They extract and run
echo.
echo ZERO SETUP. JUST WORKS.
echo ============================================
pause

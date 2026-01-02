@echo off
echo ============================================
echo __phaethon__ Release Packaging Script
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
mkdir %PKG_DIR%\docs

echo [*] Copying laptop binaries...
copy bin\Release\laptop.exe %PKG_DIR%\laptop\ >nul
if exist "bin\Release\laptop.pdb" copy bin\Release\laptop.pdb %PKG_DIR%\laptop\ >nul

echo [*] Copying desktop binaries...
copy bin\Release\desktop.exe %PKG_DIR%\desktop\ >nul
if exist "bin\Release\desktop.pdb" copy bin\Release\desktop.pdb %PKG_DIR%\desktop\ >nul

echo [*] Copying documentation...
copy README.md %PKG_DIR%\ >nul
copy QUICK_START.md %PKG_DIR%\ >nul
copy docs\DEPLOYMENT.md %PKG_DIR%\docs\ >nul
copy docs\architecture\system_overview.md %PKG_DIR%\docs\ >nul

echo [*] Checking for MemProcFS DLLs...
if exist "references\MemProcFS\files\vmm.dll" (
    echo [*] Copying MemProcFS runtime files...
    copy references\MemProcFS\files\vmm.dll %PKG_DIR%\laptop\ >nul
    copy references\MemProcFS\files\leechcore.dll %PKG_DIR%\laptop\ >nul
    if exist "references\MemProcFS\files\FTD3XX.dll" (
        copy references\MemProcFS\files\FTD3XX.dll %PKG_DIR%\laptop\ >nul
    )
    echo [PASS] MemProcFS DLLs included - laptop package is ready to run!
) else (
    echo [WARN] MemProcFS DLLs not found
    echo        Users will need to download MemProcFS separately
    echo        and copy vmm.dll, leechcore.dll, FTD3XX.dll to laptop folder
)

REM Create setup instructions
echo [*] Creating setup instructions...
(
echo __phaethon__ Release Package
echo ============================
echo.
echo LAPTOP SETUP:
echo 1. Navigate to laptop/ folder
echo 2. Ensure vmm.dll, leechcore.dll, FTD3XX.dll are present
echo 3. Run laptop.exe
echo.
echo DESKTOP SETUP (Optional):
echo 1. Navigate to desktop/ folder
echo 2. Run desktop.exe
echo.
echo DOCUMENTATION:
echo - README.md - Project overview
echo - QUICK_START.md - Getting started guide
echo - docs/DEPLOYMENT.md - Deployment instructions
echo - docs/system_overview.md - Architecture details
echo.
echo For full setup instructions, see QUICK_START.md
) > %PKG_DIR%\SETUP.txt

echo.
echo ============================================
echo Package created: %PKG_DIR%\
echo ============================================
echo.
echo Contents:
dir /b /s %PKG_DIR%
echo.
echo To distribute, compress the %PKG_DIR% folder to ZIP.
echo.
pause

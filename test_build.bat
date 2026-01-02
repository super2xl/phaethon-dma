@echo off
echo ============================================
echo __phaethon__ Build Validation Test
echo ============================================
echo.

REM Find MSBuild
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
if not exist %MSBUILD% (
    set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
)
if not exist %MSBUILD% (
    set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)
if not exist %MSBUILD% (
    echo [ERROR] MSBuild not found! Install Visual Studio 2022
    pause
    exit /b 1
)

echo [*] Found MSBuild: %MSBUILD%
echo.

REM Test 1: Build shared library
echo ============================================
echo TEST 1: Building shared library (no dependencies)
echo ============================================
%MSBUILD% shared\shared.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Shared library built successfully!
    if exist "bin\Debug\shared.lib" (
        echo [PASS] Output file exists: bin\Debug\shared.lib
    ) else (
        echo [WARN] shared.lib not found at expected location
    )
) else (
    echo [FAIL] Shared library build failed!
    echo        Check C++ compiler and Windows SDK installation
    pause
    exit /b 1
)
echo.

REM Test 2: Build desktop (optional)
echo ============================================
echo TEST 2: Building desktop (D3D11 dependencies)
echo ============================================
%MSBUILD% desktop\desktop.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Desktop built successfully!
    if exist "bin\Debug\desktop.exe" (
        echo [PASS] Output file exists: bin\Debug\desktop.exe
    )
) else (
    echo [WARN] Desktop build failed (non-critical)
)
echo.

REM Test 3: Build laptop (expected to fail without MemProcFS)
echo ============================================
echo TEST 3: Building laptop (requires MemProcFS)
echo ============================================
if exist "references\MemProcFS\includes\vmmdll.h" (
    echo [*] MemProcFS headers detected, attempting build...
    %MSBUILD% laptop\laptop.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Laptop built successfully!
        if exist "bin\Debug\laptop.exe" (
            echo [PASS] Output file exists: bin\Debug\laptop.exe
            echo.
            echo [INFO] To run laptop.exe, you need these DLLs in bin\Debug\:
            echo        - vmm.dll
            echo        - leechcore.dll
            echo        - FTD3XX.dll (if using FPGA)
        )
    ) else (
        echo [WARN] Laptop build failed
        echo        Check MemProcFS lib files exist in references\MemProcFS\includes\lib64\
    )
) else (
    echo [SKIP] MemProcFS not found - laptop build skipped
    echo        This is OK! Download from:
    echo        https://github.com/ufrisk/MemProcFS/releases
    echo        Extract to references\MemProcFS\
)
echo.

REM Summary
echo ============================================
echo VALIDATION SUMMARY
echo ============================================
if exist "bin\Debug\shared.lib" (
    echo [PASS] Core library works - ready to continue!
) else (
    echo [FAIL] Core build failed - fix errors above
    pause
    exit /b 1
)

if exist "bin\Debug\desktop.exe" (
    echo [PASS] Desktop builds
)

if exist "bin\Debug\laptop.exe" (
    echo [PASS] Laptop builds - full stack ready!
) else (
    echo [INFO] Laptop not built yet (needs MemProcFS)
)

echo.
echo ============================================
echo All critical components validated!
echo You can continue development.
echo ============================================
pause

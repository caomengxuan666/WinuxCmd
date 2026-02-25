@echo off
REM WinuxCmd Build Script (CMD version)
REM Usage: build.cmd [dev|release|test|clean] [rebuild] [test]

set MODE=%1
if "%MODE%"=="" set MODE=dev

set REBUILD=
set RUN_TESTS=

:parse_args
shift
if "%~1"=="" goto end_parse
if /i "%~1"=="rebuild" set REBUILD=1
if /i "%~1"=="test" set RUN_TESTS=1
goto parse_args
:end_parse

echo ========================================
echo WinuxCmd Build Script
echo ========================================
echo.

if "%MODE%"=="clean" (
    echo [INFO] Cleaning all build directories...
    if exist build-dev rd /s /q build-dev
    if exist build-release rd /s /q build-release
    if exist build-release-debug rd /s /q build-release-debug
    if exist build-test rd /s /q build-test
    if exist build rd /s /q build
    echo [OK] Clean complete!
    exit /b 0
)

REM Find Visual Studio
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist %VSWHERE% (
    echo [ERROR] Visual Studio not found!
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -property installationPath`) do set VSPATH=%%i
set VCVARS="%VSPATH%\VC\Auxiliary\Build\vcvars64.bat"

if not exist %VCVARS% (
    echo [ERROR] vcvars64.bat not found!
    exit /b 1
)

echo [INFO] Found Visual Studio: %VSPATH%
echo.

REM Configure based on mode
if "%MODE%"=="dev" (
    set BUILD_DIR=build-dev
    set BUILD_CONFIG=Debug
    set BUILD_MODE=DEV
    echo [INFO] Mode: Development build
) else if "%MODE%"=="release" (
    set BUILD_DIR=build-release
    set BUILD_CONFIG=Release
    set BUILD_MODE=RELEASE
    echo [INFO] Mode: Release build
) else if "%MODE%"=="test" (
    set BUILD_DIR=build-test
    set BUILD_CONFIG=Debug
    set BUILD_MODE=DEV
    echo [INFO] Mode: Test build
) else (
    echo [ERROR] Unknown mode: %MODE%
    echo Usage: build.cmd [dev^|release^|test^|clean] [rebuild] [test]
    exit /b 1
)

echo  Build Dir: %BUILD_DIR%
echo  Build Config: %BUILD_CONFIG%
echo  Build Mode: %BUILD_MODE%
echo.

REM Configure CMake
if "%REBUILD%"=="1" (
    if exist %BUILD_DIR% rd /s /q %BUILD_DIR%
)

if not exist %BUILD_DIR%\CMakeCache.txt (
    echo [INFO] Configuring CMake...
    call %VCVARS%
    cmake -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=%BUILD_CONFIG% -DBUILD_MODE=%BUILD_MODE% -DUSE_STATIC_CRT=ON -G Ninja
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed!
        exit /b 1
    )
    echo [OK] CMake configured!
) else (
    echo [INFO] CMake already configured, skipping...
)

REM Build
echo.
echo [INFO] Building WinuxCmd...
call %VCVARS%
cmake --build %BUILD_DIR% --config %BUILD_CONFIG%
if errorlevel 1 (
    echo [ERROR] Build failed!
    exit /b 1
)
echo [OK] Build completed!

REM Run tests if requested
if "%RUN_TESTS%"=="1" (
    echo.
    echo [INFO] Running tests...
    call %VCVARS%
    ctest --test-dir %BUILD_DIR% --output-on-failure
)

echo.
echo ========================================
echo Build Complete!
echo ========================================
# WinuxCmd Build Script
# Ensures MSVC compiler is used and handles all build configurations

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("dev", "release", "release-debug", "test", "clean")]
    [string]$Mode = "dev",

    [Parameter(Mandatory=$false)]
    [switch]$Rebuild,

    [Parameter(Mandatory=$false)]
    [switch]$RunTests,

    [Parameter(Mandatory=$false)]
    [switch]$Verbose
)

# Colors for output
function Write-ColorOutput {
    param([string]$Message, [string]$Color = "White")
    Write-Host $Message -ForegroundColor $Color
}

Write-ColorOutput "========================================" "Cyan"
Write-ColorOutput "WinuxCmd Build Script" "Cyan"
Write-ColorOutput "========================================" "Cyan"
Write-ColorOutput ""

# Check for MSVC environment
$msvcEnv = $null
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -property installationPath
    if ($vsPath) {
        $vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
        if (Test-Path $vcvars) {
            Write-ColorOutput "[OK] Found Visual Studio: $vsPath" "Green"
            $msvcEnv = $vcvars
        }
    }
}

if (-not $msvcEnv) {
    Write-ColorOutput "[ERROR] MSVC not found! Please install Visual Studio with C++ workload." "Red"
    exit 1
}

Write-ColorOutput ""

# Build configurations
$buildConfigs = @{
    "dev" = @{
        "Dir" = "build-dev"
        "Config" = "Debug"
        "Mode" = "DEV"
        "Description" = "Development build"
    }
    "release" = @{
        "Dir" = "build-release"
        "Config" = "Release"
        "Mode" = "RELEASE"
        "Description" = "Release build"
    }
    "release-debug" = @{
        "Dir" = "build-release-debug"
        "Config" = "Release"
        "Mode" = "DEBUG_RELEASE"
        "Description" = "Release with debug features"
    }
    "test" = @{
        "Dir" = "build-test"
        "Config" = "Debug"
        "Mode" = "DEV"
        "Description" = "Test-focused build"
    }
}

# Handle clean
if ($Mode -eq "clean") {
    Write-ColorOutput "[INFO] Cleaning all build directories..." "Yellow"
    $dirs = @("build-dev", "build-release", "build-release-debug", "build-test", "build")
    foreach ($dir in $dirs) {
        if (Test-Path $dir) {
            Write-ColorOutput "  Removing: $dir" "Yellow"
            Remove-Item -Path $dir -Recurse -Force
        }
    }
    Write-ColorOutput "[OK] Clean complete!" "Green"
    exit 0
}

$config = $buildConfigs[$Mode]
Write-ColorOutput "[INFO] Configuration: $($config.Description)" "Cyan"
Write-ColorOutput "  Build Dir: $($config.Dir)"
Write-ColorOutput "  Build Type: $($config.Config)"
Write-ColorOutput "  Build Mode: $($config.Mode)"
Write-ColorOutput ""

# Create build directory
if (-not (Test-Path $config.Dir)) {
    New-Item -ItemType Directory -Path $config.Dir | Out-Null
}

# Setup MSVC environment
$envCmd = "`"$msvcEnv`" && cd /d `"$PWD`" &&"

# Configure step
if ($Rebuild -or -not (Test-Path "$($config.Dir)\CMakeCache.txt")) {
    Write-ColorOutput "[INFO] Configuring CMake..." "Yellow"

    $cmakeArgs = @(
        "-B", $config.Dir,
        "-DCMAKE_BUILD_TYPE=$($config.Config)",
        "-DBUILD_MODE=$($config.Mode)",
        "-DUSE_STATIC_CRT=ON",
        "-G", "Ninja"
    )

    if ($Mode -eq "test") {
        $cmakeArgs += @("-DENABLE_TESTS=ON", "-DGENERATE_MAP_INFO=ON")
    }

    if ($Verbose) {
        $cmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
    }

    $cmakeCmd = $cmakeArgs -join " "

    $result = cmd /c "$envCmd cmake $cmakeCmd 2>&1"
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput "[ERROR] CMake configuration failed!" "Red"
        Write-ColorOutput $result
        exit 1
    }
    Write-ColorOutput "[OK] CMake configured successfully!" "Green"
} else {
    Write-ColorOutput "[INFO] CMake already configured, skipping..." "Yellow"
}

# Build step
Write-ColorOutput ""
Write-ColorOutput "[INFO] Building WinuxCmd..." "Yellow"

$buildTarget = "winuxcmd"
if ($Mode -eq "test") {
    $buildTarget = "winuxcmd-tests"
}

$buildCmd = "cmake --build $($config.Dir) --target $buildTarget --config $($config.Config)"
if ($Verbose) {
    $buildCmd += " --verbose"
}

$result = cmd /c "$envCmd $buildCmd 2>&1"
if ($LASTEXITCODE -ne 0) {
    Write-ColorOutput "[ERROR] Build failed!" "Red"
    Write-ColorOutput $result
    exit 1
}
Write-ColorOutput "[OK] Build completed successfully!" "Green"

# Run tests if requested
if ($RunTests -and $Mode -eq "test") {
    Write-ColorOutput ""
    Write-ColorOutput "[INFO] Running tests..." "Yellow"

    $testCmd = "ctest --test-dir $($config.Dir) --output-on-failure"
    $result = cmd /c "$envCmd $testCmd 2>&1"

    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput "[WARNING] Some tests failed!" "Yellow"
        Write-ColorOutput $result
    } else {
        Write-ColorOutput "[OK] All tests passed!" "Green"
    }
}

Write-ColorOutput ""
Write-ColorOutput "========================================" "Cyan"
Write-ColorOutput "Build Complete!" "Green"
Write-ColorOutput "========================================" "Cyan"
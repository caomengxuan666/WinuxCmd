<#
.SYNOPSIS
WinuxCmd Profile Initializer - One-time setup
.DESCRIPTION
Adds winux wrapper to PowerShell profile without PATH dependency
Run this ONCE after installing WinuxCmd.
#>

$ErrorActionPreference = "Stop"

function Write-Color {
    param($Color, $Text)
    $Colors = @{
        Green  = "`e[32m"
        Yellow = "`e[33m"
        Red    = "`e[31m"
        Blue   = "`e[34m"
        Cyan   = "`e[36m"
        Reset  = "`e[0m"
    }
    Write-Host "$($Colors[$Color])$Text$($Colors.Reset)"
}

# ========== Find WinuxCmd installation ==========
function Get-WinuxBinDir {
    $baseDir = "$env:LOCALAPPDATA\WinuxCmd"

    if (-not (Test-Path $baseDir)) {
        Write-Color "Red" "WinuxCmd not found in: $baseDir"
        return $null
    }

    # Find any WinuxCmd-* directory
    $versionDir = Get-ChildItem -Path $baseDir -Directory -Filter "WinuxCmd-*" |
                  Select-Object -First 1

    if (-not $versionDir) {
        Write-Color "Red" "No WinuxCmd version directory found"
        return $null
    }

    # Find bin directory
    $binDir = Join-Path $versionDir.FullName "bin"

    if (-not (Test-Path $binDir)) {
        # Try to find winuxcmd.exe to locate bin directory
        $exeFile = Get-ChildItem -Path $versionDir.FullName -Filter "winuxcmd.exe" -Recurse -File |
                   Select-Object -First 1
        if ($exeFile) {
            $binDir = $exeFile.DirectoryName
        }
    }

    if (-not (Test-Path $binDir)) {
        Write-Color "Red" "bin directory not found"
        return $null
    }

    return $binDir
}

# ========== Install to Profile ==========
function Install-WinuxToProfile {
    param([string]$BinDir)

    $winuxPs1Path = Join-Path $BinDir "winux.ps1"
    $winuxCmdPath = Join-Path $BinDir "winuxcmd.exe"

    # Verify winuxcmd.exe exists
    if (-not (Test-Path $winuxCmdPath)) {
        Write-Color "Red" "Error: winuxcmd.exe not found at: $winuxCmdPath"
        return $false
    }

    # 创建正确的winux函数
    $winuxFunction = @"
# WinuxCmd wrapper (added by winux_init.ps1)
function global:winux {
    param(
        [string]`$Command = "help",
        [Parameter(ValueFromRemainingArguments = `$true)]
        [string[]]`$Arguments
    )

    `$winuxPs1Path = '$winuxPs1Path'
    `$winuxCmdPath = '$winuxCmdPath'

    # Management commands that go to winux.ps1
    `$managementCommands = @("activate", "deactivate", "status", "list", "version")

    # Check if this is a management command
    if (`$Command -in `$managementCommands) {
        switch (`$Command) {
            "activate" {
                if (Test-Path `$winuxPs1Path) {
                    & `$winuxPs1Path -Action activate @`$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Cannot activate." -ForegroundColor Red
                    Write-Host "Copy winux.ps1 to: `$winuxPs1Path" -ForegroundColor Yellow
                }
            }
            "deactivate" {
                if (Test-Path `$winuxPs1Path) {
                    & `$winuxPs1Path -Action deactivate @`$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Cannot deactivate." -ForegroundColor Red
                }
            }
            "status" {
                if (Test-Path `$winuxPs1Path) {
                    & `$winuxPs1Path -Action status @`$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Status unknown." -ForegroundColor Red
                }
            }
            "list" {
                if (Test-Path `$winuxPs1Path) {
                    & `$winuxPs1Path -Action list @`$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Cannot list commands." -ForegroundColor Red
                    Write-Host "Direct GNU commands available via winuxcmd.exe" -ForegroundColor Yellow
                }
            }

            "version" {
                if (Test-Path `$winuxCmdPath) {
                    & `$winuxCmdPath --version
                } else {
                    Write-Host "winuxcmd.exe not found" -ForegroundColor Red
                }
            }
        }
        return
    }

    # Special case: --help and --version flags
    if (`$Command -eq "--help" -or `$Command -eq "-h") {
        if (Test-Path `$winuxCmdPath) {
            & `$winuxCmdPath --help
        }
        return
    }

    if (`$Command -eq "--version" -or `$Command -eq "-v") {
        if (Test-Path `$winuxCmdPath) {
            & `$winuxCmdPath --version
        }
        return
    }

    # All other commands: pass through to winuxcmd.exe
    if (Test-Path `$winuxCmdPath) {
        # Build argument list properly
        `$allArgs = @(`$Command)
        if (`$Arguments.Count -gt 0) {
            `$allArgs += `$Arguments
        }

        & `$winuxCmdPath @allArgs
    } else {
        Write-Host "winuxcmd.exe not found at: `$winuxCmdPath" -ForegroundColor Red
    }
}

Set-Alias -Name winuxcmd -Value '$winuxCmdPath' -Scope Global
"@

    # 添加到profile
    $profilePath = $PROFILE.CurrentUserAllHosts

    # Ensure profile directory exists
    $profileDir = Split-Path $profilePath -Parent
    if (-not (Test-Path $profileDir)) {
        New-Item -ItemType Directory -Path $profileDir -Force | Out-Null
    }

    # 读取当前profile
    $currentContent = ""
    if (Test-Path $profilePath) {
        $currentContent = Get-Content $profilePath -Raw
        $currentContent = $currentContent.Trim()
    }

    # 添加新函数
    $newContent = $currentContent + "`n`n" + $winuxFunction
    Set-Content -Path $profilePath -Value $newContent -Encoding UTF8 -Force

    return $true
}

# ========== Main Script ==========
Write-Color "Cyan" "WinuxCmd Profile Initializer"
Write-Color "Cyan" "==========================="
Write-Host ""

Write-Color "Yellow" "Setting up WinuxCmd in PowerShell profile..."

# Find WinuxCmd
$binDir = Get-WinuxBinDir
if (-not $binDir) {
    Write-Color "Red" "Failed to find WinuxCmd installation"
    Write-Host "Please install WinuxCmd first." -ForegroundColor Yellow
    exit 1
}

Write-Color "Cyan" "Found WinuxCmd at: $binDir"

# Ask for confirmation
Write-Host ""
Write-Host "This will add the 'winux' command to your PowerShell profile." -ForegroundColor Cyan
Write-Host "Profile location: $PROFILE.CurrentUserAllHosts" -ForegroundColor Gray
Write-Host ""

$confirm = Read-Host "Continue? (Y/N)"
if ($confirm -notmatch '^[Yy]') {
    Write-Color "Yellow" "Cancelled."
    exit 0
}

# 安装到profile
if (Install-WinuxToProfile -BinDir $binDir) {
    Write-Color "Green" "✓ WinuxCmd added to PowerShell profile"
    Write-Host ""
    Write-Color "Cyan" "Next steps:"
    Write-Host "1. RESTART PowerShell or run: . `$PROFILE" -ForegroundColor Yellow
    Write-Host "2. Test with: winux" -ForegroundColor Green
    Write-Host "3. If you have winux.ps1, copy it to: $binDir" -ForegroundColor Cyan
    Write-Host "4. Optional: winux activate (if winux.ps1 exists)" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "Usage after restart:" -ForegroundColor Blue
    Write-Host "  > winux                     # Show help"
    Write-Host "  > winux ls -la              # Use GNU ls directly"
    Write-Host "  > winux activate            # Activate (if winux.ps1 exists)"
    Write-Host "  > winuxcmd --help           # Direct alias to winuxcmd.exe"
} else {
    Write-Color "Red" "Failed to add WinuxCmd to profile"
    exit 1
}

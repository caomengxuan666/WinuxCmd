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

    # Dynamically find the latest winux version
    $winuxFunction = @'
# WinuxCmd wrapper (dynamically finds latest version)
function global:winux {
    param(
        [string]$Command = "help",
        [Parameter(ValueFromRemainingArguments = $true)]
        [string[]]$Arguments
    )

    function Find-LatestWinuxCmd {
        $baseDir = "$env:LOCALAPPDATA\WinuxCmd"
        if (-not (Test-Path $baseDir)) {
            return $null
        }

        # Fetch all version then sort.
        $allVersions = @()
        $dirs = Get-ChildItem -Path $baseDir -Directory -Filter "WinuxCmd-*" -ErrorAction SilentlyContinue

        foreach ($dir in $dirs) {
            if ($dir.Name -match 'WinuxCmd-(\d+\.\d+\.\d+)-win-(x64|arm64)') {
                try {
                    $version = [Version]$Matches[1]
                    $allVersions += [PSCustomObject]@{
                        Directory = $dir
                        Version = $version
                        FullName = $dir.FullName
                        VersionString = $Matches[1]
                    }
                }
                catch {
                    # skip parser error
                    continue
                }
            }
        }

        if ($allVersions.Count -eq 0) {
            return $null
        }

        # sort from by version
        $sorted = $allVersions | Sort-Object Version -Descending
        $latestDir = $sorted[0].FullName

        # find new and exe.
        $binDir = Join-Path $latestDir "bin"
        $winuxCmdPath = Join-Path $binDir "winuxcmd.exe"
        $winuxPs1Path = Join-Path $binDir "winux.ps1"

        if (-not (Test-Path $winuxCmdPath)) {
            $exeFile = Get-ChildItem -Path $latestDir -Filter "winuxcmd.exe" -Recurse -File -ErrorAction SilentlyContinue |
                       Select-Object -First 1
            if ($exeFile) {
                $binDir = $exeFile.DirectoryName
                $winuxCmdPath = $exeFile.FullName
                $winuxPs1Path = Join-Path $binDir "winux.ps1"
            } else {
                return $null
            }
        }

        return @{
            BinDir = $binDir
            WinuxCmdExe = $winuxCmdPath
            WinuxPs1 = $winuxPs1Path
            Version = $sorted[0].VersionString
        }
    }

    # Get WinuxCmd Path
    $winuxPaths = Find-LatestWinuxCmd
    if (-not $winuxPaths) {
        Write-Host "WinuxCmd not found. Please install WinuxCmd first." -ForegroundColor Red
        Write-Host "Expected location: $env:LOCALAPPDATA\WinuxCmd" -ForegroundColor Yellow
        return
    }

    $winuxPs1Path = $winuxPaths.WinuxPs1
    $winuxCmdPath = $winuxPaths.WinuxCmdExe
    $winuxVersion = $winuxPaths.Version

    # Management commands that go to winux.ps1
    $managementCommands = @("activate", "deactivate", "status", "list", "help", "version")

    # Check if this is a management command
    if ($Command -in $managementCommands) {
        switch ($Command) {
            "activate" {
                if (Test-Path $winuxPs1Path) {
                    & $winuxPs1Path -Action activate @$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Cannot activate." -ForegroundColor Red
                    Write-Host "Copy winux.ps1 to: $winuxPs1Path" -ForegroundColor Yellow
                }
            }
            "deactivate" {
                if (Test-Path $winuxPs1Path) {
                    & $winuxPs1Path -Action deactivate @$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Cannot deactivate." -ForegroundColor Red
                }
            }
            "status" {
                if (Test-Path $winuxPs1Path) {
                    & $winuxPs1Path -Action status @$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Status unknown." -ForegroundColor Red
                }
            }
            "list" {
                if (Test-Path $winuxPs1Path) {
                    & $winuxPs1Path -Action list @$Arguments
                } else {
                    Write-Host "winux.ps1 not found. Cannot list commands." -ForegroundColor Red
                    Write-Host "Direct GNU commands available via winuxcmd.exe" -ForegroundColor Yellow
                }
            }
            "help" {
                Write-Host "WinuxCmd v$winuxVersion - GNU Coreutils for Windows" -ForegroundColor Cyan
                Write-Host "===================================================" -ForegroundColor Cyan
                Write-Host ""
                Write-Host "Installation:" -ForegroundColor Green
                Write-Host "  Location: $($winuxPaths.BinDir)" -ForegroundColor Gray
                Write-Host ""
                Write-Host "Management Commands:" -ForegroundColor Yellow
                Write-Host "  winux activate          - Enable GNU commands"
                Write-Host "  winux deactivate        - Restore original commands"
                Write-Host "  winux status            - Check activation status"
                Write-Host "  winux list              - List available commands"
                Write-Host "  winux version           - Show version"
                Write-Host "  winux help              - Show this help"
                Write-Host ""
                Write-Host "GNU Commands (direct):" -ForegroundColor Magenta
                Write-Host "  winux ls -la            - List files"
                Write-Host "  winux cp source dest    - Copy files"
                Write-Host "  winux mv source dest    - Move files"
                Write-Host "  winux rm file           - Remove file"
                Write-Host "  winux cat file          - Show file content"
                Write-Host "  winux mkdir dir         - Create directory"
                Write-Host ""
                Write-Host "Direct Access:" -ForegroundColor Blue
                Write-Host "  winuxcmd --help         - Show winuxcmd help"
            }
            "version" {
                if (Test-Path $winuxCmdPath) {
                    & $winuxCmdPath --version
                } else {
                    Write-Host "winuxcmd.exe not found" -ForegroundColor Red
                }
            }
        }
        return
    }

    # Special case: --help and --version flags
    if ($Command -eq "--help" -or $Command -eq "-h") {
        if (Test-Path $winuxCmdPath) {
            & $winuxCmdPath --help
        }
        return
    }

    if ($Command -eq "--version" -or $Command -eq "-v") {
        if (Test-Path $winuxCmdPath) {
            & $winuxCmdPath --version
        }
        return
    }

    # All other commands: pass through to winuxcmd.exe
    if (Test-Path $winuxCmdPath) {
        # Build argument list properly
        $allArgs = @($Command)
        if ($Arguments.Count -gt 0) {
            $allArgs += $Arguments
        }

        & $winuxCmdPath @allArgs
    } else {
        Write-Host "winuxcmd.exe not found at: $winuxCmdPath" -ForegroundColor Red
    }
}

# Find winuxcmd.exe and set alias for it.
function Update-WinuxCmdAlias {
    $baseDir = "$env:LOCALAPPDATA\WinuxCmd"
    if (-not (Test-Path $baseDir)) {
        return
    }

    $dirs = Get-ChildItem -Path $baseDir -Directory -Filter "WinuxCmd-*" -ErrorAction SilentlyContinue
    $latestExe = $null

    foreach ($dir in $dirs) {
        if ($dir.Name -match 'WinuxCmd-(\d+\.\d+\.\d+)-win-(x64|arm64)') {
            $exePath = Join-Path $dir.FullName "bin\winuxcmd.exe"
            if (Test-Path $exePath) {
                $latestExe = $exePath
                # continue search for potential latest version.
            }
        }
    }

    if ($latestExe) {
        Set-Alias -Name winuxcmd -Value $latestExe -Scope Global -Force -ErrorAction SilentlyContinue
    }
}

# set alias
Update-WinuxCmdAlias

# update the alias when launch the powershell.
if ($null -ne (Get-EventSubscriber -SourceIdentifier "PowerShell.Exiting" -ErrorAction SilentlyContinue)) {
    Unregister-Event -SourceIdentifier "PowerShell.Exiting" -Force -ErrorAction SilentlyContinue
}

Register-EngineEvent -SourceIdentifier PowerShell.Exiting -Action {
    $baseDir = "$env:LOCALAPPDATA\WinuxCmd"
    if (Test-Path $baseDir) {
        $configFile = Join-Path $baseDir "last_alias.json"
        $currentAlias = (Get-Alias winuxcmd -ErrorAction SilentlyContinue).Definition

        if ($currentAlias -and (Test-Path $currentAlias)) {
            @{
                LastPath = $currentAlias
                LastUpdate = (Get-Date).ToString("o")
            } | ConvertTo-Json | Set-Content $configFile -Encoding UTF8
        }
    }
} | Out-Null
'@

    $profilePath = $PROFILE.CurrentUserAllHosts

    # Ensure profile directory exists
    $profileDir = Split-Path $profilePath -Parent
    if (-not (Test-Path $profileDir)) {
        New-Item -ItemType Directory -Path $profileDir -Force | Out-Null
    }

    # read current profile
    $currentContent = ""
    if (Test-Path $profilePath) {
        $currentContent = Get-Content $profilePath -Raw
        $currentContent = $currentContent.Trim()
    }

    # remove old configuration.
    $patterns = @(
        '(?s)# WinuxCmd wrapper.*?Set-Alias -Name winuxcmd -Value.*\n',
        '(?s)function global:winux.*?\n\}.*?\n',
        'Set-Alias -Name winuxcmd -Value.*',
        '(?s)# =+[\r\n]+# WinuxCmd Integration.*?# =+[\r\n]+# End WinuxCmd Integration[\r\n]+',
        'Register-EngineEvent -SourceIdentifier PowerShell.Exiting -Action.*Out-Null'
    )

    foreach ($pattern in $patterns) {
        $currentContent = $currentContent -replace $pattern, ''
    }

    # remove extra space
    $currentContent = $currentContent -replace '\n\n\n+', "`n`n"
    $currentContent = $currentContent.Trim()

    # add new dynamical function
    $newContent = $currentContent + "`n`n" + $winuxFunction
    Set-Content -Path $profilePath -Value $newContent -Encoding UTF8 -Force

    return $true
}

# ========== Main Script ==========
Write-Color "Cyan" "WinuxCmd Profile Initializer"
Write-Color "Cyan" "==========================="
Write-Host ""

Write-Color "Yellow" "Setting up WinuxCmd in PowerShell profile..."

# Find WinuxCmd (For Verify Installation)
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

# Install to profile.
if (Install-WinuxToProfile -BinDir $binDir) {
    Write-Color "Green" "✓ WinuxCmd added to PowerShell profile"
    Write-Host ""
    Write-Color "Cyan" "Features:"
    Write-Host "  • Automatically finds latest WinuxCmd version" -ForegroundColor Green
    Write-Host "  • No need to re-run after updates" -ForegroundColor Green
    Write-Host "  • Works with multiple installed versions" -ForegroundColor Green
    Write-Host ""
    Write-Color "Cyan" "Next steps:"
    Write-Host "1. RESTART PowerShell or run: . `$PROFILE" -ForegroundColor Yellow
    Write-Host "2. Test with: winux" -ForegroundColor Green
    Write-Host "3. If you have winux.ps1, copy it to WinuxCmd bin directory" -ForegroundColor Cyan
    Write-Host "4. Optional: winux activate (if winux.ps1 exists)" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "Usage after restart:" -ForegroundColor Blue
    Write-Host "  > winux                     # Show help and version info"
    Write-Host "  > winux ls -la              # Use GNU ls directly"
    Write-Host "  > winux activate            # Activate (if winux.ps1 exists)"
    Write-Host "  > winuxcmd --help           # Direct alias to winuxcmd.exe"
    Write-Host ""
    Write-Host "Note: Future WinuxCmd updates will be automatically detected!" -ForegroundColor Gray
} else {
    Write-Color "Red" "Failed to add WinuxCmd to profile"
    exit 1
}
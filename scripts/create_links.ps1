<#
.SYNOPSIS
    WinuxCmd Command Link Generator
.DESCRIPTION
    Creates hardlinks or symbolic links for all WinuxCmd commands
    from winuxcmd.exe in the current directory.
.PARAMETER Force
    Overwrite existing command executables without prompting.
.PARAMETER UseSymbolicLinks
    Use symbolic links instead of hardlinks (useful for non-NTFS filesystems).
.PARAMETER Remove
    Remove all created links (winuxcmd.exe is kept).
.EXAMPLE
    .\create_links.ps1
    Create hardlinks for all commands (requires NTFS).
.EXAMPLE
    .\create_links.ps1 -UseSymbolicLinks
    Create symbolic links (works on all Windows filesystems).
.EXAMPLE
    .\create_links.ps1 -Force
    Create hardlinks, overwriting existing files without prompting.
.EXAMPLE
    .\create_links.ps1 -Remove
    Remove all command links.
#>

[CmdletBinding()]
param(
    [switch]$Force,
    [switch]$UseSymbolicLinks,
    [switch]$Remove
)

$ErrorActionPreference = "Stop"
$Script:Version = "0.4.5"

# Available commands list
$Script:Commands = @(
    "cat", "chmod", "cp", "cut", "date", "df", "diff", "du", "echo", "env",
    "file", "find", "grep", "head", "kill", "ln", "ls", "mkdir", "mv", "ps",
    "pwd", "realpath", "rm", "rmdir", "sed", "sort", "tail", "tee", "touch",
    "tree", "uniq", "wc", "which", "xargs"
)

function Write-ColorOutput {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateSet("Green", "Yellow", "Red", "Cyan", "Blue", "Gray")]
        [string]$Color,
        
        [Parameter(Mandatory = $true)]
        [string]$Text
    )
    
    # Simple text-based markers for older terminals
    $Markers = @{
        Green  = "[OK]"
        Yellow = "[INFO]"
        Red    = "[ERROR]"
        Cyan   = "[INFO]"
        Blue   = "[NOTE]"
        Gray   = "      "
    }
    
    Write-Host "$($Markers[$Color]) $Text"
}

function Get-WinuxCmdPath {
    $currentDir = Get-Location
    $winuxcmdPath = Join-Path $currentDir "winuxcmd.exe"
    
    if (-not (Test-Path $winuxcmdPath)) {
        Write-ColorOutput "Red" "Error: winuxcmd.exe not found in current directory."
        Write-ColorOutput "Yellow" "Current directory: $currentDir"
        Write-ColorOutput "Gray" "Please run this script from the WinuxCmd bin directory."
        exit 1
    }
    
    return $winuxcmdPath
}

function Get-FileSystemType {
    try {
        $drive = (Get-Location).Drive.Root
        $volume = Get-WmiObject -Class Win32_Volume -Filter "DriveLetter='$drive'" -ErrorAction SilentlyContinue
        
        if ($volume) {
            return $volume.FileSystem
        }
    }
    catch {
        # Fall back to assuming NTFS if WMI fails
        return "Unknown"
    }
    
    return "Unknown"
}

function Remove-CommandLinks {
    param(
        [string]$WinuxCmdPath
    )
    
    $removedCount = 0
    $skippedCount = 0
    $errorsCount = 0
    
    Write-ColorOutput "Cyan" "Removing WinuxCmd command links..."
    Write-Host ""
    
    foreach ($cmd in $Script:Commands) {
        $cmdPath = Join-Path (Get-Location) "$cmd.exe"
        
        if (-not (Test-Path $cmdPath)) {
            continue
        }
        
        try {
            # Verify it's actually a link (either hardlink or symlink)
            $file = Get-Item $cmdPath -Force
            
            # Check if it's a reparse point (symlink) or has same attributes as winuxcmd (hardlink)
            if ($file.LinkType -eq "HardLink" -or $file.LinkType -eq "SymbolicLink" -or 
                ($file.Attributes -band [System.IO.FileAttributes]::ReparsePoint)) {
                
                Remove-Item $cmdPath -Force -ErrorAction Stop
                Write-ColorOutput "Green" "  [Removed] $cmd.exe"
                $removedCount++
            }
            else {
                Write-ColorOutput "Yellow" "  [Skipped] $cmd.exe (not a WinuxCmd link)"
                $skippedCount++
            }
        }
        catch {
            Write-ColorOutput "Red" "  [Error] Failed to remove $cmd.exe: $($_.Exception.Message)"
            $errorsCount++
        }
    }
    
    Write-Host ""
    Write-ColorOutput "Cyan" "Summary:"
    Write-ColorOutput "Green" "  Removed: $removedCount"
    if ($skippedCount -gt 0) {
        Write-ColorOutput "Yellow" "  Skipped: $skippedCount"
    }
    if ($errorsCount -gt 0) {
        Write-ColorOutput "Red" "  Errors: $errorsCount"
    }
    Write-ColorOutput "Gray" "  winuxcmd.exe is preserved"
}

function New-CommandLinks {
    param(
        [string]$WinuxCmdPath,
        [bool]$UseSymbolic
    )
    
    $createdCount = 0
    $skippedCount = 0
    $errorsCount = 0
    $fsType = Get-FileSystemType
    
    $linkType = if ($UseSymbolic) { "symbolic link" } else { "hardlink" }
    
    Write-ColorOutput "Cyan" "WinuxCmd Command Link Generator v$Script:Version"
    Write-ColorOutput "Cyan" "========================================="
    Write-Host ""
    Write-ColorOutput "Blue" "Configuration:"
    Write-ColorOutput "Gray" "  Target: $WinuxCmdPath"
    Write-ColorOutput "Gray" "  Link Type: $linkType"
    Write-ColorOutput "Gray" "  Filesystem: $fsType"
    Write-Host ""
    
    if (-not $UseSymbolic -and $fsType -ne "NTFS" -and $fsType -ne "Unknown") {
        Write-ColorOutput "Yellow" "Warning: Hardlinks require NTFS filesystem."
        Write-ColorOutput "Yellow" "         Current filesystem: $fsType"
        Write-ColorOutput "Yellow" "         Consider using -UseSymbolicLinks flag."
        Write-Host ""
        
        if (-not $Force) {
            $continue = Read-Host "Continue anyway? (Y/N)"
            if ($continue -notmatch '^[Yy]') {
                Write-ColorOutput "Yellow" "Cancelled."
                exit 0
            }
        }
    }
    
    # Check for existing files
    $existingFiles = @()
    foreach ($cmd in $Script:Commands) {
        $cmdPath = Join-Path (Get-Location) "$cmd.exe"
        if (Test-Path $cmdPath) {
            $existingFiles += $cmd
        }
    }
    
    if ($existingFiles.Count -gt 0 -and -not $Force) {
        Write-ColorOutput "Yellow" "Warning: The following files already exist:"
        foreach ($file in $existingFiles) {
            Write-ColorOutput "Yellow" "  - $file.exe"
        }
        Write-Host ""
        
        $overwrite = Read-Host "Overwrite existing files? (Y/N)"
        if ($overwrite -notmatch '^[Yy]') {
            Write-ColorOutput "Yellow" "Cancelled."
            exit 0
        }
    }
    
    Write-ColorOutput "Cyan" "Creating links..."
    Write-Host ""
    
    foreach ($cmd in $Script:Commands) {
        $cmdPath = Join-Path (Get-Location) "$cmd.exe"
        
        try {
            # Remove existing file if it exists
            if (Test-Path $cmdPath) {
                Remove-Item $cmdPath -Force -ErrorAction Stop
            }
            
            # Create the link
            if ($UseSymbolic) {
                New-Item -ItemType SymbolicLink -Path $cmdPath -Target $WinuxCmdPath -ErrorAction Stop | Out-Null
            }
            else {
                New-Item -ItemType HardLink -Path $cmdPath -Target $WinuxCmdPath -ErrorAction Stop | Out-Null
            }
            
            Write-ColorOutput "Green" "  [Created] $cmd.exe"
            $createdCount++
        }
        catch {
            $errorMsg = $_.Exception.Message
            
            # If hardlink failed and not already using symbolic links, try fallback
            if (-not $UseSymbolic -and $errorMsg -match "hardlink|reparse") {
                Write-ColorOutput "Yellow" "  [Fallback] $cmd.exe (hardlink failed, trying symbolic link)"
                try {
                    Remove-Item $cmdPath -Force -ErrorAction SilentlyContinue
                    New-Item -ItemType SymbolicLink -Path $cmdPath -Target $WinuxCmdPath -ErrorAction Stop | Out-Null
                    Write-ColorOutput "Green" "  [Created] $cmd.exe (symbolic link)"
                    $createdCount++
                    continue
                }
                catch {
                    $errorMsg = $_.Exception.Message
                }
            }
            
            Write-ColorOutput "Red" "  [Failed] $cmd.exe: $errorMsg"
            $errorsCount++
        }
    }
    
    Write-Host ""
    Write-ColorOutput "Cyan" "Summary:"
    Write-ColorOutput "Green" "  Created: $createdCount"
    if ($errorsCount -gt 0) {
        Write-ColorOutput "Red" "  Errors: $errorsCount"
    }
    Write-Host ""
    
    if ($createdCount -gt 0) {
        Write-ColorOutput "Green" "Success! WinuxCmd commands are now available."
        Write-Host ""
        Write-ColorOutput "Blue" "Usage:"
        Write-ColorOutput "Gray" "  $ ls -la           # Use ls command"
        Write-ColorOutput "Gray" "  $ cat file.txt     # Use cat command"
        Write-ColorOutput "Gray" "  $ grep pattern file  # Use grep command"
        Write-Host ""
        
        if ($UseSymbolic) {
            Write-ColorOutput "Yellow" "Note: Symbolic links were used. These may require elevated permissions"
            Write-ColorOutput "Yellow" "      on some systems. If you encounter permission issues, try running"
            Write-ColorOutput "Yellow" "      PowerShell as Administrator."
        }
    }
    else {
        Write-ColorOutput "Red" "Failed to create any command links."
        exit 1
    }
}

# Main script logic
$WinuxCmdPath = Get-WinuxCmdPath

if ($Remove) {
    Remove-CommandLinks -WinuxCmdPath $WinuxCmdPath
}
else {
    New-CommandLinks -WinuxCmdPath $WinuxCmdPath -UseSymbolic $UseSymbolicLinks
}

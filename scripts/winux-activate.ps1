<#
.SYNOPSIS
Automatically activates WinuxCmd commands, overriding PowerShell aliases

.DESCRIPTION
When this script is loaded, it will automatically:
1. Backup current aliases, functions, and PATH
2. Remove/override ls, cat, cp, mv, rm, mkdir, echo commands
3. Add WinuxCmd to the front of PATH
4. Provide winux-deactivate command to restore original state

.NOTES
Only affects the current PowerShell session, automatically restored when window is closed
#>

# Configuration - Auto-detect installation path
$ScriptPath = $MyInvocation.MyCommand.Path
$WinuxCmdPath = Split-Path $ScriptPath -Parent
Write-Host "WinuxCmd path detected: $WinuxCmdPath" -ForegroundColor Yellow

$CommandsToOverride = @('ls', 'cat', 'cp', 'mv', 'rm', 'mkdir', 'echo')

# Check if WinuxCmd path exists
if (-not (Test-Path "$WinuxCmdPath\winuxcmd.exe")) {
    Write-Host "Error: winuxcmd.exe not found in: $WinuxCmdPath" -ForegroundColor Red
    Write-Host "Please make sure WinuxCmd is installed correctly" -ForegroundColor Yellow
    return
}

# Backup storage variable (global, for deactivate use)
if (-not $global:WinuxBackup) {
    $global:WinuxBackup = @{
        Aliases = @{}
        Functions = @{}
        OriginalPath = $env:PATH
        Activated = $false
        ActivationTime = Get-Date
    }
}

# Backup current state
function Backup-CommandState {
    foreach ($cmd in $CommandsToOverride) {
        $command = Get-Command $cmd -ErrorAction SilentlyContinue

        if ($command) {
            switch ($command.CommandType) {
                'Alias' {
                    $global:WinuxBackup.Aliases[$cmd] = @{
                        Type = 'Alias'
                        Definition = $command.Definition
                        Options = $command.Options
                    }
                }
                'Function' {
                    $global:WinuxBackup.Functions[$cmd] = @{
                        Type = 'Function'
                        Definition = $command.Definition
                    }
                }
            }
        }
    }
}

# Remove/override commands
function Remove-Commands {
    foreach ($cmd in $CommandsToOverride) {
        $command = Get-Command $cmd -ErrorAction SilentlyContinue

        if ($command) {
            switch ($command.CommandType) {
                'Alias' {
                    try {
                        Remove-Item "Alias:\$cmd" -Force
                    } catch {
                        # If deletion fails, try in child scope
                        & { Remove-Item "Alias:\$cmd" -Force -ErrorAction SilentlyContinue }
                    }
                }
                'Function' {
                    try {
                        # Redefine to call WinuxCmd
                        $functionBody = @"
                        param([Parameter(ValueFromRemainingArguments)]`$args)
                        & "$WinuxCmdPath\$cmd.exe" @args
"@
                        Set-Item -Path "Function:\$cmd" -Value ([ScriptBlock]::Create($functionBody)) -Force
                    } catch {
                        # Silently ignore function override errors
                    }
                }
            }
        }

        # Ensure command points to WinuxCmd
        if (Test-Path "$WinuxCmdPath\$cmd.exe") {
            Set-Alias $cmd "$WinuxCmdPath\$cmd.exe" -Force -Scope Global -ErrorAction SilentlyContinue
        }
    }
}

# Update PATH
function Update-Path {
    # Ensure WinuxCmdPath is at the front of PATH
    $pathParts = $env:PATH -split ';'

    # Remove existing WinuxCmd path (avoid duplicates)
    $pathParts = $pathParts | Where-Object { $_ -ne $WinuxCmdPath }

    # Add to the front
    $newPath = $WinuxCmdPath + ';' + ($pathParts -join ';')
    $env:PATH = $newPath
}

# Auto activation function
function Enable-WinuxCmdAuto {
    if ($global:WinuxBackup.Activated) {
        return  # Already activated
    }

    # Backup
    Backup-CommandState

    # Process commands
    Remove-Commands

    # Update PATH
    Update-Path

    # Set activation state
    $global:WinuxBackup.Activated = $true
    $global:WinuxBackup.ActivationTime = Get-Date
}

# Restore function
function winux-deactivate {
    if (-not $global:WinuxBackup.Activated) {
        return
    }

    # Restore PATH
    $env:PATH = $global:WinuxBackup.OriginalPath

    # Restore aliases
    foreach ($cmd in $global:WinuxBackup.Aliases.Keys) {
        $aliasInfo = $global:WinuxBackup.Aliases[$cmd]
        try {
            if ($aliasInfo.Options) {
                Set-Alias $cmd $aliasInfo.Definition -Option $aliasInfo.Options -Force
            } else {
                Set-Alias $cmd $aliasInfo.Definition -Force
            }
        } catch {
            # Ignore restore errors
        }
    }

    # Restore functions
    foreach ($cmd in $global:WinuxBackup.Functions.Keys) {
        $funcInfo = $global:WinuxBackup.Functions[$cmd]
        try {
            Set-Item -Path "Function:\$cmd" -Value ([ScriptBlock]::Create($funcInfo.Definition)) -Force
        } catch {
            # Ignore restore errors
        }
    }

    # Clear activation state
    $global:WinuxBackup.Activated = $false
}

# Status check function
function winux-status {
    Write-Host "WinuxCmd Status" -ForegroundColor Cyan
    Write-Host "=" * 50 -ForegroundColor DarkGray

    Write-Host "Activation status: $(if ($global:WinuxBackup.Activated) { '✅ Activated' } else { '❌ Not activated' })" -ForegroundColor $(if ($global:WinuxBackup.Activated) { 'Green' } else { 'Red' })

    if ($global:WinuxBackup.Activated) {
        Write-Host "Activation time: $($global:WinuxBackup.ActivationTime.ToString('HH:mm:ss'))" -ForegroundColor Gray
        Write-Host "Duration: $((Get-Date) - $global:WinuxBackup.ActivationTime)" -ForegroundColor Gray
    }

    Write-Host "" -ForegroundColor Gray
    Write-Host "Command status:" -ForegroundColor Cyan
    foreach ($cmd in $CommandsToOverride) {
        $command = Get-Command $cmd -ErrorAction SilentlyContinue
        if ($command -and $command.Source -like "*$WinuxCmdPath*") {
            Write-Host "  $cmd : ✅ WinuxCmd" -ForegroundColor Green
        } elseif ($command) {
            Write-Host "  $cmd : ⚠️  $($command.CommandType)" -ForegroundColor Yellow
        } else {
            Write-Host "  $cmd : ❌ Not found" -ForegroundColor Red
        }
    }

    Write-Host "=" * 50 -ForegroundColor DarkGray
}

# Test command function
function winux-test {
    foreach ($cmd in $CommandsToOverride) {
        $exePath = "$WinuxCmdPath\$cmd.exe"
        if (Test-Path $exePath) {
            try {
                & $exePath --help > $null 2>&1
                Write-Host "  ✅ $cmd : Working" -ForegroundColor Green
            } catch {
                Write-Host "  ❌ $cmd : Test failed" -ForegroundColor Red
            }
        } else {
            Write-Host "  ❌ $cmd : Executable not found" -ForegroundColor Red
        }
    }
}

# Helper function
function winux-help {
    Write-Host "WinuxCmd Help" -ForegroundColor Cyan
    Write-Host "=" * 50 -ForegroundColor DarkGray

    Write-Host "Available commands:" -ForegroundColor Cyan
    Write-Host "  winux-deactivate     Restore original state" -ForegroundColor Gray
    Write-Host "  winux-status         Check activation status" -ForegroundColor Gray
    Write-Host "  winux-test           Test WinuxCmd commands" -ForegroundColor Gray
    Write-Host "  winux-help           Show this help message" -ForegroundColor Gray
    Write-Host "" -ForegroundColor Gray
    Write-Host "Overridden commands:" -ForegroundColor Cyan
    Write-Host "  ls, cat, cp, mv, rm, mkdir, echo" -ForegroundColor Gray

    Write-Host "=" * 50 -ForegroundColor DarkGray
}

# === Main execution: Auto activate on load ===
try {
    Enable-WinuxCmdAuto
} catch {
    # Silently ignore activation errors
}

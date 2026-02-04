<#
.SYNOPSIS
WinuxCmd - Complete system backup and restore solution for PowerShell commands
#>

param(
    [ValidateSet("activate", "deactivate", "status", "list")]
    [string]$Action = "activate"
)

$ScriptDir = $PSScriptRoot

$CommandMap = @{
    "ls"    = "ls.exe"
    "cat"   = "cat.exe"
    "cp"    = "cp.exe"
    "mkdir" = "mkdir.exe"
    "mv"    = "mv.exe"
    "rm"    = "rm.exe"
}

# ========== Backup System ==========

function Get-CommandBackup {
    param([string]$CommandName)

    $cmd = Get-Command $CommandName -ErrorAction SilentlyContinue
    if (-not $cmd) { return $null }

    $backup = @{
        Name         = $cmd.Name
        CommandType  = $cmd.CommandType.ToString()
        ModuleName   = $cmd.ModuleName
        Source       = $cmd.Source
        Definition   = $cmd.Definition
        BackupTime   = [DateTime]::Now
    }

    switch ($cmd.CommandType) {
        "Alias" {
            $backup.ResolvedCommand = (Get-Command $cmd.Definition -ErrorAction SilentlyContinue)
            $backup.Options = $cmd.Options -join ","
        }
        "Function" {
            $backup.ScriptBlock = $cmd.ScriptBlock.ToString()
            $backup.Parameters = $cmd.Parameters.Keys -join ","
        }
    }

    return $backup
}

function Save-CommandBackups {
    $global:Winux_Backups = @{}

    foreach ($cmdName in $CommandMap.Keys) {
        $backup = Get-CommandBackup -CommandName $cmdName
        if ($backup) {
            $global:Winux_Backups[$cmdName] = $backup
        }
    }
}

function Restore-FromBackups {
    if (-not $global:Winux_Backups) { return }

    foreach ($cmdName in $global:Winux_Backups.Keys) {
        $backup = $global:Winux_Backups[$cmdName]

        try {
            Remove-Item "Function:\global:$cmdName" -Force -ErrorAction SilentlyContinue

            switch ($backup.CommandType) {
                "Alias" {
                    $alias = Get-Alias $cmdName -ErrorAction SilentlyContinue
                    if (-not $alias) {
                        Set-Alias -Name $cmdName -Value $backup.Definition -Scope Global -Force
                    }
                }
                "Function" {
                    if ($backup.ScriptBlock) {
                        $scriptBlock = [scriptblock]::Create($backup.ScriptBlock)
                        Set-Item -Path "Function:\global:$cmdName" -Value $scriptBlock -Force
                    }
                }
            }
        }
        catch {
	Write-Warning "Failed to restore ${cmdName}: $_"
        }
    }
}

# ========== Main Functions ==========

function Show-CommandList {
    Write-Host "Available WinuxCmd Commands:" -ForegroundColor Cyan
    Write-Host "=============================" -ForegroundColor Cyan

    foreach ($cmd in $CommandMap.Keys | Sort-Object) {
        $exePath = Join-Path $ScriptDir $CommandMap[$cmd]
        $exeExists = Test-Path $exePath
        $exeStatus = if ($exeExists) { "✓" } else { "✗" }

        Write-Host "  $cmd" -ForegroundColor Yellow -NoNewline
        Write-Host " -> $($CommandMap[$cmd]) [$exeStatus]"
    }

    Write-Host ""
    Write-Host "To use these commands, run: .\winux.ps1 activate" -ForegroundColor Green
}

function Show-Status {
    Write-Host "WinuxCmd Status Report" -ForegroundColor Cyan
    Write-Host "======================" -ForegroundColor Cyan

    $activeCommands = @()
    foreach ($cmd in $CommandMap.Keys) {
        $current = Get-Command $cmd -ErrorAction SilentlyContinue
        if ($current -and $current.Definition -like "*.exe*") {
            $activeCommands += $cmd
        }
    }

    if ($activeCommands.Count -gt 0) {
        Write-Host "Status: ACTIVE" -ForegroundColor Green
        Write-Host "Active commands: $($activeCommands -join ', ')" -ForegroundColor Yellow
    } else {
        Write-Host "Status: INACTIVE" -ForegroundColor Gray
    }

    Write-Host ""
    Write-Host "Run '.\winux.ps1 list' to see available commands" -ForegroundColor Gray
}

function Invoke-Activate {
    Write-Host "Activating WinuxCmd..." -ForegroundColor Green

    Save-CommandBackups

    foreach ($cmd in $CommandMap.Keys) {
        $exePath = Join-Path $ScriptDir $CommandMap[$cmd]

        if (-not (Test-Path $exePath)) {
            Write-Warning "Skipping ${cmd}: $($CommandMap[$cmd]) not found"
            continue
        }

        $functionString = @"
function global:$cmd {
    param([string[]]`$argsArray)
    & '$exePath' @argsArray
}
"@

        try {
            Remove-Item "Alias:\$cmd" -Force -ErrorAction SilentlyContinue 2>$null
            Remove-Item "Function:\$cmd" -Force -ErrorAction SilentlyContinue 2>$null

            Invoke-Expression $functionString -ErrorAction Stop
            Write-Host "  ✓ $cmd" -ForegroundColor Cyan
        }
        catch {
            Write-Warning "Failed to create ${cmd}: ${_}"
        }
    }

    Write-Host "Activation complete!" -ForegroundColor Green
    Show-CommandList
}

function Invoke-Deactivate {
    Write-Host "Deactivating WinuxCmd..." -ForegroundColor Green

    Restore-FromBackups
    Remove-Variable Winux_Backups -Scope Global -ErrorAction SilentlyContinue 2>$null

    Write-Host "Deactivation complete! All original commands restored." -ForegroundColor Green
}

# ========== Main Logic ==========

switch ($Action) {
    "activate"   { Invoke-Activate }
    "deactivate" { Invoke-Deactivate }
    "status"     { Show-Status }
    "list"       { Show-CommandList }
    default      { Invoke-Activate }
}

Register-EngineEvent -SourceIdentifier PowerShell.Exiting -Action {
    if ($global:Winux_Backups) {
        foreach ($cmd in $CommandMap.Keys) {
            Remove-Item "Function:\global:$cmd" -Force -ErrorAction SilentlyContinue 2>$null
        }
        Remove-Variable Winux_Backups -Scope Global -ErrorAction SilentlyContinue 2>$null
    }
} | Out-Null
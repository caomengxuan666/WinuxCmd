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
    "ls"     = "ls.exe"
    "cat"    = "cat.exe"
    "cp"     = "cp.exe"
    "mv"     = "mv.exe"
    "rm"     = "rm.exe"
    "mkdir"  = "mkdir.exe"
    "rmdir"  = "rmdir.exe"
    "touch"  = "touch.exe"
    "echo"   = "echo.exe"
    "head"   = "head.exe"
    "tail"   = "tail.exe"
    "find"   = "find.exe"
    "grep"   = "grep.exe"
    "sort"   = "sort.exe"
    "uniq"   = "uniq.exe"
    "cut"    = "cut.exe"
    "wc"     = "wc.exe"
    "which"  = "which.exe"
    "env"    = "env.exe"
    "sed"    = "sed.exe"
    "chmod"  = "chmod.exe"
    "date"   = "date.exe"
    "diff"   = "diff.exe"
    "ln"     = "ln.exe"
    "ps"     = "ps.exe"
    "pwd"    = "pwd.exe"
    "tee"    = "tee.exe"
    "xargs"  = "xargs.exe"
    "file"   = "file.exe"
    "realpath" = "realpath.exe"
    "df"     = "df.exe"
    "du"     = "du.exe"
    "kill"   = "kill.exe"
    "tree"   = "tree.exe"
    "lsof"   = "lsof.exe"
    # New commands added in v0.5.3
    "base64" = "base64.exe"
    "tr"     = "tr.exe"
    "less"   = "less.exe"
    "watch"  = "watch.exe"
    "jq"     = "jq.exe"
    "md5sum" = "md5sum.exe"
    "sha256sum" = "sha256sum.exe"
    "basename" = "basename.exe"
    "dirname" = "dirname.exe"
    "free"   = "free.exe"
    "column" = "column.exe"
    "seq"    = "seq.exe"
    "stat"   = "stat.exe"
    # New commands added in v0.7.0 - Hash tools
    "sha1sum" = "sha1sum.exe"
    "sha224sum" = "sha224sum.exe"
    "sha384sum" = "sha384sum.exe"
    "sha512sum" = "sha512sum.exe"
    "b2sum" = "b2sum.exe"
    # New commands added in v0.7.0 - Text processing
    "paste" = "paste.exe"
    "join" = "join.exe"
    "comm" = "comm.exe"
    "split" = "split.exe"
    "csplit" = "csplit.exe"
    "cmp" = "cmp.exe"
    "nl" = "nl.exe"
    "fold" = "fold.exe"
    "fmt" = "fmt.exe"
    # New commands added in v0.7.0 - Text conversion
    "expand" = "expand.exe"
    "unexpand" = "unexpand.exe"
    "tac" = "tac.exe"
    # New commands added in v0.7.0 - System information
    "hostname" = "hostname.exe"
    "whoami" = "whoami.exe"
    "arch" = "arch.exe"
    "uname" = "uname.exe"
    "id" = "id.exe"
    "who" = "who.exe"
    "users" = "users.exe"
    "groups" = "groups.exe"
    # New commands added in v0.7.0 - File operations
    "truncate" = "truncate.exe"
    "mktemp" = "mktemp.exe"
    "install" = "install.exe"
    "readlink" = "readlink.exe"
    "cksum" = "cksum.exe"
    "sum" = "sum.exe"
    "mkfifo" = "mkfifo.exe"
    # New commands added in v0.7.0 - Other tools
    "sleep" = "sleep.exe"
    "timeout" = "timeout.exe"
    "uptime" = "uptime.exe"
    "shuf" = "shuf.exe"
    "pr" = "pr.exe"
    "yes" = "yes.exe"
    "ptx" = "ptx.exe"
    # New commands added in v0.7.0 - Basic utilities
    "clear" = "clear.exe"
    "true" = "true.exe"
    "false" = "false.exe"
    "tty" = "tty.exe"
    "sync" = "sync.exe"
    "reset" = "reset.exe"
    "logname" = "logname.exe"
    "printenv" = "printenv.exe"
    # New commands added in v0.7.0 - Text processing
    "rev" = "rev.exe"
    "d2u" = "d2u.exe"
    "u2d" = "u2d.exe"
    "dos2unix" = "dos2unix.exe"
    "unix2dos" = "unix2dos.exe"
    "base32" = "base32.exe"
    "basenc" = "basenc.exe"
    "cygpath" = "cygpath.exe"
    "pathchk" = "pathchk.exe"
    # New commands added in v0.7.0 - Programming tools
    "printf" = "printf.exe"
    "expr" = "expr.exe"
    "test" = "test.exe"
    "[" = "[.exe"
    # New commands added in v0.7.0 - Binary tools
    "od" = "od.exe"
    "xxd" = "xxd.exe"
    "dd" = "dd.exe"
    "shred" = "shred.exe"
    # New commands added in v0.7.0 - System utilities
    "numfmt" = "numfmt.exe"
    "hmac256" = "hmac256.exe"
    "nice" = "nice.exe"
    "nohup" = "nohup.exe"
    "stdbuf" = "stdbuf.exe"
    # New commands added in v0.7.0 - Development tools
    "patch" = "patch.exe"
    "diff3" = "diff3.exe"
    "sdiff" = "sdiff.exe"
    # New commands added in v0.7.0 - Calendar and sorting
    "cal" = "cal.exe"
    "tsort" = "tsort.exe"
    # New commands added in v0.7.0 - Terminal tools
    "tput" = "tput.exe"
    "infocmp" = "infocmp.exe"
    "tic" = "tic.exe"
    "toe" = "toe.exe"
    # New commands added in v0.7.0 - System information
    "hostid" = "hostid.exe"
    "locale" = "locale.exe"
    "tzset" = "tzset.exe"
    "pinky" = "pinky.exe"
    "mpicalc" = "mpicalc.exe"
    # New commands added in v0.7.0 - Archive tools
    "cpio" = "cpio.exe"
    # New commands added in v0.7.0 - System utilities
    "nproc" = "nproc.exe"
    "getconf" = "getconf.exe"
    "link" = "link.exe"
    "unlink" = "unlink.exe"
    "factor" = "factor.exe"
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

        # Remove regular aliases/functions
        if (Test-Path "Alias:\$cmd") {
            Remove-Item "Alias:\$cmd" -Force -ErrorAction SilentlyContinue
        }

        if (Test-Path "Alias:\global:$cmd") {
            Remove-Item "Alias:\global:$cmd" -Force -ErrorAction SilentlyContinue
        }

        if (Test-Path "Function:\$cmd") {
            Remove-Item "Function:\$cmd" -Force -ErrorAction SilentlyContinue
        }

        if (Test-Path "Function:\global:$cmd") {
            Remove-Item "Function:\global:$cmd" -Force -ErrorAction SilentlyContinue
        }
    }

    # Add WinuxCmd bin directory to PATH
    if ($env:PATH -notlike "$ScriptDir*") {
        $env:PATH = "$ScriptDir;$env:PATH"
    }

    Write-Host "Activation complete! (native pipeline enabled)" -ForegroundColor Green

    # ----- Special notice for echo and cp -----
    $specialCmds = @("echo", "cp")
    foreach ($special in $specialCmds) {
        $exePath = Join-Path $ScriptDir $CommandMap[$special]
        if (Test-Path $exePath) {
            Write-Host "Warning: '$special' is a built-in PowerShell alias. Use '$special.exe' to run the WinuxCmd version." -ForegroundColor Magenta
        }
    }

    # ----- Print all command mappings -----
    Write-Host ""
    Write-Host "WinuxCmd Command Mapping:" -ForegroundColor Cyan
    Write-Host "========================" -ForegroundColor Cyan
    foreach ($cmd in $CommandMap.Keys | Sort-Object) {
        $exePath = Join-Path $ScriptDir $CommandMap[$cmd]
        $exists = if (Test-Path $exePath) { "✓" } else { "✗" }
        Write-Host ("{0,-8} -> {1,-15} [{2}]" -f $cmd, $CommandMap[$cmd], $exists)
    }

    Write-Host ""
    Write-Host "Use commands directly (except 'echo' and 'cp' need '.exe')." -ForegroundColor Cyan
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
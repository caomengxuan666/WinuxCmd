# ==============================================================================
# WinuxCmd PowerShell Tab Completion
# ==============================================================================
# Dot-source this file from your $PROFILE once (handled automatically by
# winuxcmd --install / winuxcmd_setup.ps1).
#
# What it provides:
#   - winuxcmd <Tab>           -> list all sub-commands + description
#   - winuxcmd ls <Tab>        -> list ls options with descriptions
#   - ls <Tab>  /  ls.exe <Tab>  -> same, for individual command executables
# ==============================================================================

$ErrorActionPreference = 'SilentlyContinue'

# ─── Locate winuxcmd.exe (used only at load time to enumerate commands) ────────
function _WinuxFind-Exe {
    $here = Split-Path -Parent $MyInvocation.ScriptName
    if ($here -and (Test-Path "$here\winuxcmd.exe")) { return "$here\winuxcmd.exe" }
    $found = Get-Command winuxcmd.exe -ErrorAction SilentlyContinue
    if ($found) { return $found.Source }
    $base = "$env:LOCALAPPDATA\WinuxCmd"
    if (Test-Path $base) {
        $exe = Get-ChildItem "$base\WinuxCmd-*\*\winuxcmd.exe","$base\WinuxCmd-*\winuxcmd.exe" `
               -ErrorAction SilentlyContinue | Sort-Object FullName -Descending | Select-Object -First 1
        if ($exe) { return $exe.FullName }
    }
    return $null
}

$_initExe = _WinuxFind-Exe
if (-not $_initExe) { return }   # winuxcmd not available, skip silently

# ─── Shared inline helper (embedded in each scriptblock for scope safety) ─────
# Returns an array of CompletionResult from "text\tdesc\n" lines.
$_lineParser = {
    param([string[]]$Lines, [string]$Prefix)
    foreach ($line in $Lines) {
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        $parts = $line -split "`t", 2
        $text  = $parts[0].Trim()
        $desc  = if ($parts.Count -gt 1) { $parts[1].Trim() } else { $text }
        if ($text.StartsWith($Prefix)) {
            [System.Management.Automation.CompletionResult]::new(
                $text, $text, 'ParameterValue', $desc)
        }
    }
}

# ─── winuxcmd <subcommand> [options] completer ────────────────────────────────
# Self-contained scriptblock: calls 'winuxcmd' (assumed on PATH).
$_winuxcmdCompleter = {
    param($wordToComplete, $commandAst, $cursorPosition)

    $els   = $commandAst.CommandElements
    $count = $els.Count

    # Determine context:
    #   completing_cmd = true  → user is still typing the subcommand name
    #   completing_cmd = false → user is typing an option for a known subcommand
    $completing_cmd = ($count -le 1) -or
                      ($count -eq 2 -and ($els[1].ToString() -eq $wordToComplete))

    if ($completing_cmd) {
        $lines = winuxcmd --complete-cmd $wordToComplete 2>$null
    } else {
        $cmd   = $els[1].ToString() -replace '\.exe$', ''
        $lines = winuxcmd --complete-opt $cmd $wordToComplete 2>$null
    }

    foreach ($line in $lines) {
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        $p    = $line -split "`t", 2
        $text = $p[0].Trim()
        $desc = if ($p.Count -gt 1) { $p[1].Trim() } else { $text }
        if ($text.StartsWith($wordToComplete)) {
            [System.Management.Automation.CompletionResult]::new(
                $text, $text, 'ParameterValue', $desc)
        }
    }
}
Register-ArgumentCompleter -Native -CommandName winuxcmd -ScriptBlock $_winuxcmdCompleter

# ─── Per-command completer (self-contained, no $Script: references) ───────────
# Derives the bare command name from the AST first element, then queries options.
$_cmdOptCompleter = {
    param($wordToComplete, $commandAst, $cursorPosition)

    $bare  = $commandAst.CommandElements[0].ToString() `
             -replace '\.exe$', '' -replace '^.*[/\\]', ''
    $lines = winuxcmd --complete-opt $bare $wordToComplete 2>$null

    foreach ($line in $lines) {
        if ([string]::IsNullOrWhiteSpace($line)) { continue }
        $p    = $line -split "`t", 2
        $text = $p[0].Trim()
        $desc = if ($p.Count -gt 1) { $p[1].Trim() } else { $text }
        if ($text.StartsWith($wordToComplete)) {
            [System.Management.Automation.CompletionResult]::new(
                $text, $text, 'ParameterValue', $desc)
        }
    }
}

# Enumerate all commands at load time and register completers.
# Register both bare name AND .exe form (the .exe form bypasses PS built-in aliases
# like "kill" → Stop-Process, "sort" → Sort-Object, etc.)
$_allCmds = (& $_initExe --complete-cmd "" 2>$null) |
            ForEach-Object { ($_ -split "`t")[0].Trim() } |
            Where-Object { $_ }

foreach ($c in $_allCmds) {
    Register-ArgumentCompleter -Native -CommandName $c       -ScriptBlock $_cmdOptCompleter
    Register-ArgumentCompleter -Native -CommandName "$c.exe" -ScriptBlock $_cmdOptCompleter
}

Remove-Variable _allCmds, _initExe -ErrorAction SilentlyContinue


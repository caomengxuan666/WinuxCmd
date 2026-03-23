# WinuxCmd Shell Integration (PowerShell / CMD / Windows Terminal)

## Recommended Startup

### PowerShell profile auto-enter

Add this snippet to your `$PROFILE`:

```powershell
# Auto-enter WinuxCmd interactive environment
$cliArgs = [Environment]::GetCommandLineArgs() | ForEach-Object { $_.ToLowerInvariant() }
$isNonInteractiveLaunch = ($cliArgs -contains '-command') -or ($cliArgs -contains '-c') -or ($cliArgs -contains '-file') -or ($cliArgs -contains '-f')
$isRealTerminal = $env:WT_SESSION -or $env:TERM_PROGRAM
if ($Host.Name -eq 'ConsoleHost' -and -not $isNonInteractiveLaunch `
    -and $env:WINUXCMD_BOOTSTRAPPED -ne '1' -and $isRealTerminal) {
    $env:WINUXCMD_BOOTSTRAPPED = '1'
    $winuxExe = (Get-Command winuxcmd -ErrorAction SilentlyContinue).Source
    if (-not $winuxExe) {
        $devExe = 'your\winuxcmd.exe\path'  # replace with your local path
        if (Test-Path $devExe) {
            $winuxExe = $devExe
        }
    }
    if ($winuxExe -and (Test-Path $winuxExe)) {
        & $winuxExe
    }
}
```

Replace `$devExe` with the actual local path to your `winuxcmd.exe`.

### CMD / Windows Terminal startup command

```bat
%SystemRoot%\System32\cmd.exe /k winuxcmd
```

## Fallback Behavior

- Built-in WinuxCmd commands: handled internally
- Unknown commands: delegated to native shell
- Native shell selection is automatic:
  - parent shell is PowerShell/pwsh -> fallback uses PowerShell
  - parent shell is cmd -> fallback uses cmd

This keeps PowerShell cmdlets usable when entering WinuxCmd from PowerShell.

## Completion

- default completion file: `%USERPROFILE%\.winuxcmd\completions\user-completions.txt`
- env override: `WINUXCMD_COMPLETION_FILE`

## Troubleshooting

1. `Get-Process` not recognized:
   - You likely entered from cmd; use PowerShell startup path if you need cmdlets.
2. `lsof` not recognized in piped fallback:
   - Update to latest build; built-in command pipeline rewriting should handle this.
3. Profile script not loaded:
   - verify execution policy and profile path with `echo $PROFILE`.

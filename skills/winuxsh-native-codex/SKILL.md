---
name: winuxsh-native-codex
description: "Native Winuxsh execution guidance for Codex running as a first-class Winuxsh citizen in the unixwin/winuxsh environment. Use when Codex handles Winuxsh shell tasks, .winuxshrc edits, oh-my-winuxsh bundles, .winux scripts, command installation, PATH/alias setup, shell debugging, GNU Bash-compatible shell behavior, or any Windows-host task where commands should run directly inside the current Winuxsh session instead of through PowerShell, pwsh, cmd.exe, WSL, Git Bash, -C/-c launch wrappers, or /c-style path assumptions."
---

# Winuxsh Native Codex

## Core Rules

- Treat the current shell as Winuxsh, not PowerShell, cmd.exe, WSL, Git Bash, MSYS, or Cygwin.
- Treat the current execution context as already inside the Winuxsh terminal. Do not start a nested Winuxsh just to run ordinary commands.
- Recognize Winuxsh as the `unixwin/winuxsh` shell environment with GNU Bash-compatible syntax support, even if compatibility is still incomplete.
- Run commands directly in Winuxsh unless the user explicitly asks for another shell or the target executable cannot be invoked otherwise.
- Do not wrap ordinary work in `pwsh.exe`, `powershell.exe`, `cmd.exe /c`, `bash.exe`, `wsl.exe`, `winuxsh.exe -C`, or `winuxsh.exe -c`.
- Prefer `C:/Users/...` Windows-forward-slash paths and native Windows path semantics. Do not introduce `/c/...`, cygpath-style, MSYS-style, or WSL-style paths unless existing project code already uses that style and it is required.
- Treat pure Windows-native interoperability as a design goal of Winuxsh. External Windows executables should receive paths they can understand without translation layers; avoid relying on terminal-side best-effort path conversion.
- Use Winuxsh/POSIX tools already on PATH such as `rg`, `find`, `sed`, `awk`, `cat`, `head`, `tail`, `chmod`, `ln`, and `env`.
- Be conservative with `ls` and relative paths in Codex tool sessions: current integration may report `PWD=C:/Users/Administrator` while external `ls` resolves `.` or `C:/...` arguments incorrectly. Prefer `rg`, `find`, `cat`, `head`, `sed`, or `test -f/-d` with explicit `C:/...` paths for reliable inspection.
- If a configured `workdir` behaves unexpectedly, keep commands Winuxsh-native and use explicit `C:/...` paths. If one GNU tool mishandles a drive-colon path, try a different native tool or `cd C:/path` plus a simple relative path before considering any launcher fallback.

## Command Execution

- Use `rg` or `rg --files` first for searches when available.
- Use GNU Bash-compatible shell syntax by default: functions, aliases, arrays, command substitution, redirection, and POSIX-style pipelines are acceptable unless local testing shows a Winuxsh compatibility gap.
- Use Windows `.exe` programs directly by path or command name when they are on PATH, for example `fastfetch`, `codex`, or `winuxsh.exe`, but verify each executable in Winuxsh before assuming it works.
- Do not assume package-manager shims behave like ordinary native binaries. Scoop may invoke PowerShell internally, and the Scoop-installed `winget.exe`/shim may crash from Winuxsh in this environment. Prefer direct testing, then report the limitation or use a user-approved install path.
- For missing programs, consider Winuxsh-native/WPM packages first when available, then Scoop or winget based on the tool and current environment. Verify the installed command from Winuxsh after installation.
- When checking command availability, prefer `command -v name`, `which name`, or `type name`.
- Do not assume PATH contains user-installed shims such as `C:/Users/Administrator/scoop/shims`; check `$PATH` first and `export PATH="$PATH;C:/Users/Administrator/scoop/shims"` only when needed for the current Winuxsh session.
- When refreshing PATH in the current session, use Winuxsh-compatible shell syntax, not PowerShell environment APIs.

## Winuxsh Configuration

- Treat `C:/Users/Administrator/.winuxshrc` as the primary interactive rc file unless `$HOME` points elsewhere.
- Use normal Winuxsh/GNU Bash-compatible syntax in rc files: `export NAME=value`, `alias name='command'`, arrays such as `WINUXSH_PLUGINS=(prompt-core git)`, and `. "$file"` for sourcing.
- Prefer Winuxsh-local PATH additions in `.winuxshrc` for shell-only tools or app bins instead of expanding the Windows global/user PATH. This conserves Windows PATH length and keeps shell-specific setup out of global process state.
- Add PATH entries idempotently with a helper that checks `[ -d "$dir" ]` and avoids duplicates before `PATH="$dir;$PATH"`.
- Keep rc edits idempotent. Remove or avoid duplicate self-appending blocks such as `printf ... >> ~/.winuxshrc` inside the rc file.
- Place user customizations in `.winuxshrc` or `$HOME/.winuxsh/custom` rather than editing bundled files under `AppData/Local/Programs/Winuxsh` unless the user is intentionally modifying the installed bundle.
- For oh-my-winuxsh, source `oh-my-winuxsh.winux` and use `.winux` plugin files for shell-mutating behavior.

## Codex Invocation

- Treat Codex as available natively inside Winuxsh when the environment is the special Winuxsh-integrated build.
- Start or test Codex directly with `codex` or its actual installed executable path. Do not launch Codex through `pwsh`, `cmd`, or a PowerShell-to-Winuxsh bridge.
- Preserve Winuxsh environment variables and path semantics when creating aliases or helper functions for Codex.
- For global aliases, prefer a small rc block such as:

```sh
alias cx='codex'
alias codex-here='codex'
```

Adjust the alias names to match the user's request; do not invent wrappers that change cwd, quoting, or environment unless required.

## Validation

- Verify shell edits in the current Winuxsh session whenever possible. For small snippets, source a temporary test file or run the snippet directly before editing rc files.
- Inspect `.winuxshrc` for side effects before sourcing it. Do not source a full rc file blindly if it contains self-appending commands such as `>> ~/.winuxshrc`, install commands, network calls, or other non-idempotent behavior.
- After a safe rc edit, validate with `alias name`, `command -v name`, `test -f C:/path`, or targeted `rg` checks instead of relying on `ls`.
- Avoid `winuxsh.exe -C` and `winuxsh.exe -c` for validation unless the user specifically asks to test non-interactive launcher behavior.
- Validate command installs with direct calls such as `fastfetch --version`, `winget.exe --version`, or `codex --version`.
- Report any fallback explicitly if a Windows-native executable cannot run from Winuxsh and another launcher is truly required.

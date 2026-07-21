# WinuxCmd

[English](README.md) | [中文](README-zh.md)

> GNU-style command layer for Windows-native shells.

![GitHub release (latest by date)](https://img.shields.io/github/v/release/unixwin/WinuxCmd)
![GitHub all releases](https://img.shields.io/github/downloads/unixwin/WinuxCmd/total)
![GitHub stars](https://img.shields.io/github/stars/unixwin/WinuxCmd)
![GitHub license](https://img.shields.io/github/license/unixwin/WinuxCmd)
![Windows Support](https://img.shields.io/badge/platform-Windows-blue)

WinuxCmd provides native Windows implementations of familiar Unix/GNU commands such as `ls`, `cat`, `grep`, `find`, `xargs`, `rm`, `cp`, `mv`, `sed`, `sort`, and `uniq`.

It is the command layer used by [winuxsh](https://github.com/unixwin/winuxsh), and it can also be installed directly for PowerShell, Command Prompt, Windows Terminal, build scripts, and CI jobs.

![Windows Terminal](DOCS/images/WindowsTerminal.png)

## Recommended: Winuxsh

If you want a bash-like terminal on Windows, start with [winuxsh](https://github.com/unixwin/winuxsh):

```text
winuxsh = rubash shell engine + reedline frontend + winuxcmd coreutils
```

Winuxsh gives you bash-compatible shell syntax, prompt/git integration, history, completions, and a bundled WinuxCmd command set without requiring WSL, MSYS2, Git Bash, or PowerShell semantics.

Download the matching package from the [winuxsh releases](https://github.com/unixwin/winuxsh/releases):

```powershell
# choose x64 or arm64, unzip, then run:
.\winuxsh.exe
```

The winuxsh release package carries its own architecture-matched `winuxcmd/winuxcmd.exe` and activation script. That copy is intentionally isolated from any WinuxCmd you install with winget or an installer for PowerShell.

## PowerShell Install

If you want GNU-style commands inside your existing PowerShell workflow, install WinuxCmd directly:

```powershell
winget install caomengxuan666.WinuxCmd
```

Then use commands through `winuxcmd`:

```powershell
winuxcmd ls -la
winuxcmd grep --color=auto TODO README.md
winuxcmd find . -name "*.cpp"
```

For interactive PowerShell sessions, the package also includes `winux.ps1` / `winux.cmd` helpers. After profile setup, `winux activate` can override common PowerShell aliases such as `ls`, `cat`, `rm`, `grep`, and `man` in the current session; `winux deactivate` restores the original aliases.

## Keep The Channels Separate

Use the right channel for the job:

- **winuxsh release**: bundles its own `winuxcmd/` directory for the shell. Upgrade it by upgrading winuxsh.
- **winget / installer release**: installs WinuxCmd for PowerShell, Command Prompt, and direct `winuxcmd <command>` usage.
- **Development builds**: use local hardlinks or `winuxcmd <command>` from the build tree.

Do not make winuxsh depend on a winget-installed WinuxCmd in normal use. The shell package is designed to be self-contained so x64 and arm64 builds stay reproducible.

## External Tools With WPM

WinuxCmd intentionally stays focused on the GNU-style command layer it can maintain well. Complete third-party tools such as `jq`, `ncat`, `7z`, `zstd`, and `yq` should come from WPM-managed external packages or another package manager instead of partial built-ins.

WPM installs external executables directly into the selected WinuxCmd root by default. That means `wpm install jq --root <dir>` places `jq.exe` beside `winuxcmd.exe`, so adding that one directory to `PATH` is enough. Package indexes can still map support files explicitly when a tool needs side-by-side DLLs.

Typical flow:

```powershell
winuxcmd wpm source list
winuxcmd wpm index update
winuxcmd wpm install jq --root "C:\path\to\winuxcmd"
winuxcmd wpm links rebuild --root "C:\path\to\winuxcmd"
```

For winuxsh bundles, run WPM against the bundled WinuxCmd directory, not a separate winget install. Keep external helper executables in the same PATH-visible directory that winuxsh uses for bundled commands.

## Why WinuxCmd Exists

Windows developers constantly receive Linux-flavored commands from docs, CI logs, issue comments, shell snippets, and muscle memory. WinuxCmd makes those text workflows useful on Windows without asking you to leave Windows.

It focuses on:

- native Windows process behavior and paths
- familiar GNU-style command-line flags where they matter
- predictable pipelines with Windows tools such as `tasklist`, `netstat`, `sc`, and `ipconfig`
- compatibility with older Windows PowerShell 5.1 script environments
- small release artifacts that can be bundled by other tools

## What It Feels Like

```bash
netstat -ano | grep 8080
tasklist | grep -i chrome
ipconfig | grep -i ipv4
dir /b | grep -E "\.cpp$" | sort | uniq
find . -name "*.cpp" -print0 | xargs -0 grep -n TODO
```

The point is not to turn Windows into Linux. The point is to make familiar text workflows work where your files, terminals, scripts, and CI already are.

## Command Coverage

WinuxCmd currently implements 153 commands, including practical coverage across:

- file tools: `ls`, `cp`, `mv`, `rm`, `mkdir`, `ln`, `stat`, `readlink`, `realpath`
- text tools: `cat`, `grep`, `sed`, `sort`, `uniq`, `cut`, `head`, `tail`, `wc`
- search and composition: `find`, `xargs`
- Windows-friendly utilities: `ps`, `lsof`, `which`, `tree`, `hexdump`, `strings`

Detailed compatibility references live here:

- [Command Compatibility Matrix (EN)](DOCS/en/commands_implementation_en.md)
- [GNU Parity Ledger (EN)](DOCS/en/gnu_coreutils_parity.md)
- [命令兼容性矩阵 (ZH)](DOCS/zh/commands_implementation.md)
- [GNU 兼容清单 (ZH)](DOCS/zh/gnu_coreutils_parity.md)

## Windows Notes

- Unknown commands fall back through the parent shell, so WinuxCmd complements PowerShell and `cmd` instead of replacing them.
- `--version` is handled uniformly by WinuxCmd; do not treat individual command version output as separate compatibility work.
- Color-aware commands such as `grep` follow terminal and `--color` behavior. Use `--color=always` when you intentionally want ANSI color preserved through a pipe.
- For interactive `cmd` use, launching Windows Terminal with `%SystemRoot%\System32\cmd.exe /k winuxcmd` is safer than a global `AutoRun` hook.

## Building

```powershell
cmake --preset vs2022
cmake --build build-vs --target winuxcmd --parallel
```

Useful local checks:

```powershell
ctest --test-dir build-vs -R "^(grep|find|rm)\." --output-on-failure
```

## More

- [winuxsh](https://github.com/unixwin/winuxsh)
- [Contributing Guide (EN)](CONTRIBUTING.md)
- [Contributing Guide (ZH)](CONTRIBUTING_ZH.MD)
- [Build Modes](DOCS/en/build_modes_en.md)
- [Custom Containers and Benchmarks](DOCS/en/custom_containers.md)

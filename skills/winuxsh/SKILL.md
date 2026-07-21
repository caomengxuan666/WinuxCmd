---
name: winuxsh
description: >-
  Use winuxsh as the Windows-native agent shell: run commands through winuxsh
  -c, prefer bundled WinuxCmd coreutils over PowerShell aliases, locate or
  install the repo-local winuxsh build on PATH, export MSVC build variables when
  needed, and keep downloaded helper tools in one PATH-visible directory. Use
  when working on Windows repositories where winuxsh or WinuxCmd are available,
  especially WinuxCmd/winuxsh development, builds, tests, release work, or agent
  automation.
---

# Winuxsh

Use `winuxsh` as the default command runner for Windows agent work once winuxsh
is installed on PATH. It provides a bash-like shell and bundled WinuxCmd
coreutils, so the agent can stay inside the project's own Windows-native
toolchain.

## Command Runner

- Prefer `winuxsh -c '...'` for shell work. Do not write a hardcoded local
  `winuxsh.exe` path into routine commands once PATH is configured.
- Keep the actual command inside the `-c` string: `rg`, `sed`, `grep`, `find`,
  `which`, `ls`, `cp`, `mv`, `rm`, and similar tools should come from WinuxCmd
  or the winuxsh environment.
- If `winuxsh -c 'printf ok'` does not run a shell command, fix the local PATH
  first instead of falling back to PowerShell.
- Use `multi_tool_use.parallel` for independent reads and searches, but do not
  join unrelated commands with noisy separators.
- If a Windows app execution alias is not visible from winuxsh, call a real
  executable path or use a narrowly scoped bridge such as `cmd.exe /c`.

## One-Time Setup

- Ensure the winuxsh release directory is permanently on the user PATH before
  expecting agents to use it. The PATH entry should be the directory containing
  the shell launcher, not a parent directory.
- Use `winuxsh` as the stable command name in skills and automation. Shorter
  aliases such as `winux` are a user preference and should not be assumed.
- Do not create `winux.exe`, `winux.cmd`, or other aliases on the user's behalf
  unless the user explicitly asks for that alias.
- Verify setup with:

```bash
winuxsh -c 'printf "winuxsh ok\n"; which sed; which grep; which jq'
```

- If only a local build exists, add its release directory to PATH once, then use
  `winuxsh -c` normally.

## Path Rules

- Treat `../winuxsh/target/release` as the local release/dev directory when
  working from the WinuxCmd repo, but add it to PATH instead of spelling it out
  in every command.
- Put extra downloaded helper executables directly in one PATH-visible folder.
  Do not bury command executables in per-package subdirectories unless that
  folder itself is added to PATH.
- For winuxsh-bundled tooling, prefer colocating external helpers beside
  `winuxsh.exe` or beside the bundled `winuxcmd.exe` directory that winuxsh
  prepends to PATH.
- Remember that a PATH entry names a directory, not a recursive tree.

## MSVC Builds

When MSVC environment variables are missing, export them inside the winuxsh
command before running CMake/Ninja:

```bash
export INCLUDE="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\include;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\winrt;C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\cppwinrt"
export LIB="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\lib\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64"
export PATH="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64;C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64;$PATH"
ninja -C build-vs winuxcmd-tests
```

## WinuxCmd Development

- Use `wpm links rebuild` from the relevant WinuxCmd root to regenerate command
  hardlinks.
- `wpm` is an internal WinuxCmd tool, not a coreutils command.
- Do not reintroduce built-in `jq`; use WPM or external package managers for
  complete third-party tools such as `jq`, `ncat`, `7z`, `zstd`, and `yq`.
- Keep `json.hpp/json.cppm` when WPM needs JSON parsing for local index and
  source metadata.

## Validation

- Prefer targeted tests: `./build-vs/tests/winuxcmd-tests.exe --gtest_filter=wpm.*`
  and `--gtest_filter=winuxcmd.*` for WPM/link changes.
- Verify command visibility with `which <tool>` and `tool --version`.
- Use `git diff --check` before committing.

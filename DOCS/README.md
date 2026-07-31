# WinuxCmd Docs

This folder is the working README for WinuxCmd documentation.

## What to read first

1. [Command compatibility matrix](generated/command_compatibility_matrix.md)
2. [Command performance baseline](generated/command_performance_baseline.md)
3. [Command compatibility audit](command_compatibility_audit.md)
4. [GNU Coreutils parity ledger](en/gnu_coreutils_parity.md)
5. [Implementation status](en/commands_implementation_en.md)
6. [Workspace integration](en/workspace_integration.md)

## Quick Start

1. Download the Windows binary archive from the project release page.
2. Unpack it into the repository, then run
   `.\scripts\setup-workspace-bin.ps1 -Source <unpacked-binary-dir>`.
3. Enable this workspace without touching global PATH:
   `.\scripts\activate-workspace.ps1`.
4. On Windows, call WinuxCmd manuals as `man.exe <command>`.

Release packages should include both the Windows binaries and
`WinuxCmd-skill-v<version>.zip`, so agents can install the repo-local skill
next to the workspace integration scripts.

## Rules

- Keep GNU references pointed at the official manuals.
- Track compatibility as a matrix, not as copied manual text.
- Keep file-operand wildcard expansion limited to commands that explicitly opt in.
- For fixed-arity file commands such as `diff`, `diff3`, `split`, and
  `csplit`, wildcard expansion must resolve to the exact number of operands
  the command expects.
- Do not touch global PATH for workspace activation.
- Use `man.exe` on Windows when asking WinuxCmd for help.

## Current Focus

- Keep this folder focused on durable evidence: generated matrices, performance baselines, and source-led compatibility audit notes.
- Track forward-looking implementation plans in GitHub issues instead of long-lived plan documents.
- Follow-up coverage for `cpio`, `free`, `lsof`, `man`, `top`, `tree`, `uptime`, and `watch` is tracked in [issue #114](https://github.com/unixwin/WinuxCmd/issues/114).
- Refresh generated compatibility docs with `scripts/audit-command-compatibility.py` and `scripts/benchmark-command-parity.py` before each release PR.

## References

- [English overview](en/overview.md)
- [Chinese overview](zh/overview_zh.md)
- [WinuxCmd vs Microsoft Coreutils matrix](generated/microsoft_coreutils_matrix.md)
- [WinuxCmd vs Microsoft Coreutils behavior matrix](generated/microsoft_coreutils_behavior_matrix.md)
- [English TODO](en/TODO.md)
- [Chinese TODO](zh/TODO_zh.md)
- GNU `find` action references: [`-exec ... ;`](https://www.gnu.org/software/findutils/manual/html_node/find_html/Single-File.html),
  [`-exec ... {} +`](https://www.gnu.org/software/findutils/manual/html_node/find_html/Multiple-Files.html)
- GNU `xargs` reference: [Invoking xargs](https://www.gnu.org/software/findutils/manual/html_node/find_html/Invoking-xargs.html)

# WinuxCmd 文档入口

这个目录是 WinuxCmd 的文档入口页。

## 先读这些

1. [命令兼容性矩阵](generated/command_compatibility_matrix.md)
2. [命令性能基线](generated/command_performance_baseline.md)
3. [命令兼容性审计](command_compatibility_audit.md)
4. [GNU Coreutils 兼容性审计](zh/gnu_coreutils_parity.md)
5. [命令实现状态](zh/commands_implementation.md)
6. [工作区集成](zh/workspace_integration.md)

## 快速开始

1. 从项目 release 页面下载 Windows binary 压缩包。
2. 解压到仓库内，然后运行
   `.\scripts\setup-workspace-bin.ps1 -Source <解压后的二进制目录>`。
3. 在不污染全局 PATH 的情况下启用当前工作区：
   `.\scripts\activate-workspace.ps1`。
4. Windows 上查 WinuxCmd 帮助时使用 `man.exe <command>`。

release 包应同时附带 Windows binaries 和 `WinuxCmd-skill-v<version>.zip`，
这样 agent 可以把仓库内 skill 和工作区集成脚本一起安装使用。

## 规则

- GNU 参考只链接官方手册。
- 兼容性用矩阵维护，不直接复制手册正文。
- 只有显式接入 `contains_wildcard` / `glob_expand` 的命令才展开通配符。
- 对于 `diff`、`diff3`、`split`、`csplit` 这类固定参数个数的命令，
  通配符展开必须精确匹配命令期望的参数数量。
- 工作区激活不要污染全局 PATH。
- 在 Windows 上询问帮助时使用 `man.exe`。

## 当前重点

- 这个目录只保留长期证据：生成矩阵、性能基线和基于源码阅读的兼容性审计。
- 后续实现计划放到 GitHub issue，不再维护单独的长期计划文档。
- `cpio`、`free`、`lsof`、`man`、`top`、`tree`、`uptime`、`watch` 的下一小版本覆盖计划见 [issue #114](https://github.com/unixwin/WinuxCmd/issues/114)。
- 每次 release PR 前用 `scripts/audit-command-compatibility.py` 和 `scripts/benchmark-command-parity.py` 刷新生成文档。

## 入口索引

- [中文概览](zh/overview_zh.md)
- [英文概览](en/overview.md)
- [WinuxCmd 与 Microsoft Coreutils 对照矩阵](generated/microsoft_coreutils_matrix.md)
- [WinuxCmd 与 Microsoft Coreutils 行为矩阵](generated/microsoft_coreutils_behavior_matrix.md)
- [中文 TODO](zh/TODO_zh.md)
- [英文 TODO](en/TODO.md)
- GNU `find` action 参考：[`-exec ... ;`](https://www.gnu.org/software/findutils/manual/html_node/find_html/Single-File.html)、
  [`-exec ... {} +`](https://www.gnu.org/software/findutils/manual/html_node/find_html/Multiple-Files.html)
- GNU `xargs` 参考：[Invoking xargs](https://www.gnu.org/software/findutils/manual/html_node/find_html/Invoking-xargs.html)

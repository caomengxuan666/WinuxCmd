# WinuxCmd

[English](README.md) | 中文

> 给 Windows 原生终端用的 Unix 风格命令层。

![GitHub release (latest by date)](https://img.shields.io/github/v/release/unixwin/WinuxCmd)
![GitHub all releases](https://img.shields.io/github/downloads/unixwin/WinuxCmd/total)
![GitHub stars](https://img.shields.io/github/stars/unixwin/WinuxCmd)
![GitHub license](https://img.shields.io/github/license/unixwin/WinuxCmd)
![Windows Support](https://img.shields.io/badge/platform-Windows-blue)

WinuxCmd 给 Windows 终端补上一层紧凑的 GNU 风格命令：`ls`、`cat`、
`grep`、`find`、`xargs`、`sed`、`sort`、`uniq`、`cp`、`mv`、`rm`
等等。

0.13 这一版开始，定位更明确：WinuxCmd 负责核心命令，WPM 负责管理
`awk`、`jq`、`rg`、`fd`、`fzf`、`bat`、`yq`、`7z`、`ffmpeg` 这类可携带的
外部二进制工具。

```powershell
winuxcmd ls -la
winuxcmd grep --color=auto TODO README.md
winuxcmd find . -name "*.cpp" -print0

winuxcmd wpm source list -v
winuxcmd wpm search json
winuxcmd wpm info jq
```

## 演示

观看这段短的 Unix 风格工作流录屏：[winuxcmd-unix-demo.mp4](DOCS/media/winuxcmd-unix-demo.mp4)。

![WinuxCmd 终端演示](DOCS/images/auto.gif)

## 为什么需要

Windows 开发者每天都会从文档、CI 日志、issue 评论和肌肉记忆里遇到
Linux 风格命令。WinuxCmd 的目标不是把 Windows 伪装成 Linux，而是让这
些文本工作流在你已经使用的文件、终端、脚本和 CI 里直接可用。

重点是：

- Windows 原生 exe、进程行为和路径。
- 高频 GNU 风格参数，够用、可预期。
- 能和 `tasklist`、`netstat`、`sc`、`ipconfig` 等 Windows 工具组成管道。
- 产物小，方便被 shell、安装包、CI 镜像和 portable 工具目录内置。

## 安装

给 PowerShell、命令提示符、Windows Terminal、构建脚本和 CI 使用：

```powershell
winget install caomengxuan666.WinuxCmd
```

然后通过 `winuxcmd` 运行：

```powershell
winuxcmd --version
winuxcmd ls -la
winuxcmd grep -n TODO src
```

安装包也包含 `winux.ps1` 和 `winux.cmd`。在交互式 PowerShell 会话里，
`winux activate` 可以暴露 `ls`、`cat`、`rm`、`grep`、`man` 等常见命令，
但不会要求你换掉整个 shell。

## WPM

WPM 是 WinuxCmd 自带的小型包和 hardlink 管理器。它不是 winget 的替代品。

WPM 适合：

- 放在 `winuxcmd.exe` 同级目录的 portable 命令行 sidecar。
- 单文件 `.exe`，或者带明确文件映射的简单 `.zip`。
- Windows shell 工作流里常用的 Unix-like helper。
- 重建 hardlink，让命令可以直接按名字调用。

winget 更适合：

- GUI 应用、运行时、SDK、服务、驱动和系统级安装器。
- 需要厂商安装逻辑、PATH 策略、文件关联或后台组件的工具。

官方 WPM 源已经独立出去：

- Source repo: [unixwin/wpm-source](https://github.com/unixwin/wpm-source)
- Raw index: `https://raw.githubusercontent.com/unixwin/wpm-source/main/index.json`
- CDN index: `https://cdn.jsdelivr.net/gh/unixwin/wpm-source@main/index.json`

典型流程：

```powershell
winuxcmd wpm source list -v
winuxcmd wpm index update
winuxcmd wpm search editor
winuxcmd wpm info jq
winuxcmd wpm links rebuild --root "C:\path\to\winuxcmd"
```

包在没有架构 URL、SHA-256 和文件映射之前只会显示为 `index-only`。这样
WPM 可以先展示元数据，但只有经过校验的二进制才会真正安装。

## Winuxsh

如果你想要更接近 bash 的 Windows 终端，可以直接用
[winuxsh](https://github.com/unixwin/winuxsh)：

```text
winuxsh = rubash shell engine + reedline frontend + winuxcmd command layer
```

winuxsh release 包会自带架构匹配的 `winuxcmd/` 目录。它应该和 winget
安装的 WinuxCmd 分开，这样 x64 和 arm64 shell bundle 才能稳定复现。

## 自带什么

WinuxCmd 目前实现 153 个命令，覆盖这些常见场景：

- 文件：`ls`、`cp`、`mv`、`rm`、`mkdir`、`ln`、`stat`、`readlink`、`realpath`
- 文本：`cat`、`grep`、`sed`、`sort`、`uniq`、`cut`、`head`、`tail`、`wc`
- 搜索与组合：`find`、`xargs`
- Windows 侧补位：`ps`、`lsof`、`which`、`tree`、`hexdump`、`strings`
- WPM：`wpm source`、`wpm index`、`wpm list`、`wpm search`、`wpm info`、
  `wpm install`、`wpm links`

详细兼容清单：

- [命令兼容性矩阵 (ZH)](DOCS/zh/commands_implementation.md)
- [GNU 兼容清单 (ZH)](DOCS/zh/gnu_coreutils_parity.md)
- [Command Compatibility Matrix (EN)](DOCS/en/commands_implementation_en.md)
- [GNU Parity Ledger (EN)](DOCS/en/gnu_coreutils_parity.md)

## 构建

```powershell
cmake --preset vs2022
cmake --build build-vs --target winuxcmd --parallel
```

常用检查：

```powershell
build-vs\tests\winuxcmd-tests.exe --gtest_filter=wpm.*
ctest --test-dir build-vs -R "^(grep|find|rm)\." --output-on-failure
```

## 链接

- [winuxsh](https://github.com/unixwin/winuxsh)
- [WPM source](https://github.com/unixwin/wpm-source)
- [贡献指南](CONTRIBUTING_ZH.MD)
- [构建模式文档](DOCS/zh/build_modes.md)


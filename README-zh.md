# WinuxCmd：为 Windows 带来 Linux 命令 × 跨平台管道

[English](README.md) | 中文

> 原生 Windows 实现的 Linux 命令集 | 仅 ~900KB | 无需 WSL/WSL2 | Windows 命令 × Linux 过滤器无缝互通

![GitHub release (latest by date)](https://img.shields.io/github/v/release/caomengxuan666/WinuxCmd)
![GitHub all releases](https://img.shields.io/github/downloads/caomengxuan666/WinuxCmd/total)
![GitHub stars](https://img.shields.io/github/stars/caomengxuan666/WinuxCmd)
![GitHub license](https://img.shields.io/github/license/caomengxuan666/WinuxCmd)
![Windows Support](https://img.shields.io/badge/platform-Windows-blue)

## ⭐ Star 趋势

[![Star History Chart](https://api.star-history.com/svg?repos=caomengxuan666/WinuxCmd&type=date&legend=top-left)](https://www.star-history.com/#caomengxuan666/WinuxCmd&type=date&legend=top-left)

## ✨ 核心亮点

| | 特性 | 说明 |
|---|---|---|
| 🔗 | **Windows × Linux 管道直连** | `netstat -ano \| grep 8080` 开箱即用，Windows 工具输出直接喂给 Linux 过滤器 |
| ⚡ | **极速启动** | < 10ms，比 GNU coreutils/MSYS2 快 2–8 倍 |
| 🪶 | **超轻量** | 单个可执行文件 ~900KB，35 条命令零冗余 |
| 🤖 | **AI 友好** | AI 生成的 Linux 命令在 Windows 上直接运行，无需改写 |
| 🔍 | **智能补全** | 内置命令 + Windows 系统命令 + 用户自定义，Tab 键即达 |
| 🚫 | **无需管理员** | 安装与运行均不需要提权 |
| 📦 | **无外部依赖** | 纯 Win32 API，无 VC++ 运行时依赖 |

---

## 🚀 快速开始

### 一键安装（推荐）

```powershell
# 在 PowerShell 中运行（无需管理员权限）
irm https://dl.caomengxuan666.com/install.ps1 | iex
```

### 手动安装

1. 从 [Releases](https://github.com/caomengxuan666/WinuxCmd/releases) 下载
2. 解压到任意目录，进入 `bin` 目录
3. 运行 `create_links.ps1` 生成命令链接
4. 将 `bin` 目录加入 PATH

```powershell
.\create_links.ps1                    # NTFS 文件系统（推荐）
.\create_links.ps1 -UseSymbolicLinks  # 非 NTFS 文件系统
.\create_links.ps1 -Remove            # 移除全部链接
```

### Shell 集成

**PowerShell** — 在 `$PROFILE` 中加入以下内容，打开终端自动进入 WinuxCmd：

```powershell
$cliArgs = [Environment]::GetCommandLineArgs() | ForEach-Object { $_.ToLowerInvariant() }
$isNonInteractiveLaunch = ($cliArgs -contains '-command') -or ($cliArgs -contains '-c') -or ($cliArgs -contains '-file') -or ($cliArgs -contains '-f')
if ($Host.Name -eq 'ConsoleHost' -and -not $isNonInteractiveLaunch -and $env:WINUXCMD_BOOTSTRAPPED -ne '1') {
    $env:WINUXCMD_BOOTSTRAPPED = '1'
    $winuxExe = (Get-Command winuxcmd -ErrorAction SilentlyContinue).Source
    if (-not $winuxExe) {
        $devExe = 'your\winuxcmd.exe\path'  # 替换为实际路径
        if (Test-Path $devExe) { $winuxExe = $devExe }
    }
    if ($winuxExe -and (Test-Path $winuxExe)) { & $winuxExe }
}
```

**CMD / Windows Terminal** — 将命令行配置改为：

```bat
%SystemRoot%\System32\cmd.exe /k winuxcmd
```

### 自动补全

![自动补全演示](DOCS/images/auto.gif)

内置三层补全，Tab 键直达：

- WinuxCmd 命令及其参数
- Windows 系统命令白名单（含说明）
- 用户自定义扩展：`%USERPROFILE%\.winuxcmd\completions\user-completions.txt`

```text
# 用户补全文件格式
cmd|git|Distributed version control
opt|git|pull|Fetch from and integrate with another repository
```

> 📖 详见 [补全配置文档](DOCS/zh/overview_zh.md)

---

## 📦 已实现的命令（35 个）

> 完整参数列表见 [命令实现文档](DOCS/zh/commands_implementation.md)

| 命令 | 描述 | 命令 | 描述 |
|------|------|------|------|
| `ls` | 列出目录内容 | `ps` | 进程列表 |
| `cat` | 显示文件内容 | `kill` | 发送信号 |
| `grep` | 文本搜索 | `lsof` | 打开文件与网络连接 |
| `find` | 查找文件 | `sed` | 流式编辑器 |
| `cp` / `mv` / `rm` | 文件操作 | `sort` / `uniq` / `cut` | 文本处理 |
| `mkdir` / `rmdir` | 目录操作 | `head` / `tail` / `wc` | 文本工具 |
| `chmod` / `ln` / `touch` | 文件系统 | `df` / `du` / `diff` | 磁盘/比较 |
| `echo` / `env` / `which` | 工具类 | `pwd` / `realpath` / `tree` | 路径工具 |
| `date` / `tee` / `xargs` | 实用命令 | `file` | 文件类型识别 |

---

## 🎯 为什么选择 WinuxCmd？

### 问题所在

- **AI 工具**（GitHub Copilot、Cursor）生成的 Linux 命令在 Windows 直接报错
- **Windows 原生管道**无法连接 Windows 命令与 Linux 过滤器
- **PowerShell** 没有交互式智能补全

```bash
# 这些在纯 Windows 上全部失败：
netstat -ano | grep 8080        # grep 不存在
tasklist | awk '{print $2}'     # awk 不存在
ls -la | grep ".cpp"            # ls、grep 不存在
```

### 解决方案

```bash
# ✅ 安装 WinuxCmd 后，全部开箱即用：
netstat -ano | grep 8080                      # 查占用端口的进程
tasklist | grep -i chrome | awk '{print $2}'  # 找 Chrome PID
ipconfig | grep -i "ipv4"                     # 只看 IP 地址
lsof -i :8080                                 # 查端口归属（支持 :PORT / tcp:PORT）
ls -la | grep ".cpp" | xargs wc -l            # 统计 C++ 代码行数
set | grep -i proxy                           # 检查代理环境变量
```

---

## 💡 技术亮点

### 1. 跨平台管道 — 最大差异化特性

WinuxCmd 的管道符（`|`）是 **Windows 工具与 Linux 工具的直连桥梁**。任何 Windows 命令的 stdout，无需任何转换，直接作为 Linux 过滤器的 stdin。

```bash
# 网络诊断
netstat -an | grep LISTENING | awk '{print $2}' | sort -u   # 所有监听端口
lsof -i :8080                     # 查端口归属
lsof -i tcp:443                   # 过滤协议 + 端口

# 进程管理
tasklist | grep -i chrome | awk '{print $2}'   # 提取 Chrome PID

# 系统分析
dir /b /a-d | grep -oP '\.[^.]+$' | sort | uniq -c | sort -rn  # 按扩展名统计
```

### 2. 启动性能

完整命令执行耗时（启动 + 执行 + 退出），1000 文件目录，20 次迭代：

| 命令 | WinuxCmd (ms) | uutils Rust (ms) | 胜负 |
|:----:|:---:|:---:|:---:|
| ls   | 6.30 | 7.27 | ✅ WinuxCmd |
| cat  | 6.19 | 7.01 | ✅ WinuxCmd |
| sort | 6.31 | 7.27 | ✅ WinuxCmd |
| grep | 6.42 | 5.99 | uutils |

**7/8 命令胜出，整体比 uutils (Rust) 快 1.09x；比 MSYS2 快 2–8 倍**

### 3. 体积对比

```
WinuxCmd（静态）：        ~900 KB
BusyBox Windows：        ~1.24 MB
GNU coreutils（MSYS2）：  ~5 MB
单个 ls.exe（C/CMake）：  ~1.5 MB
```

所有命令共用同一个可执行文件的硬链接，35 个命令只占 1 份体积。

---

## 🔧 构建

> 需要 MSVC 与 CMake 3.30+，详见 [构建模式文档](DOCS/zh/build_modes.md)

```powershell
cmake --preset dev        # 配置开发模式
cmake --build build-dev   # 构建
```

---

## 🤝 参与贡献

欢迎提交 PR！详见 [CONTRIBUTING_ZH.MD](CONTRIBUTING_ZH.MD)。

**新手友好任务：**
- 为命令添加 `-h/--help` 支持
- 改进错误信息

---

## ❓ 常见问题

**Q：和 WSL 有什么区别？**  
A：WSL 是完整的 Linux 子系统；WinuxCmd 是理解 Linux 语法的原生 Windows 可执行文件，无虚拟化开销。

**Q：会影响原生 Windows 命令吗？**  
A：不会。WinuxCmd 无法识别的命令自动回退到 Windows 原生命令行执行。

**Q：安全吗？**  
A：开源，无网络连接，无遥测。

**Q：为什么用 C++ 而不是 Rust/Go？**  
A：最大性能、最小依赖、直接访问 Win32 API。

---

## 📚 文档

| 文档 | 说明 |
|------|------|
| [概览与架构](DOCS/zh/overview_zh.md) | 模块设计、核心 API |
| [命令实现细节](DOCS/zh/commands_implementation.md) | 完整参数兼容性矩阵 |
| [构建模式](DOCS/zh/build_modes.md) | Dev / Release / ASan 预设 |
| [选项处理指南](DOCS/zh/option-handling_zh.md) | 如何添加新命令 |
| [测试框架](DOCS/zh/testing_framework.md) | wctest 使用说明 |
| [Shell 集成](DOCS/zh/winux_shell_integration_zh.md) | PowerShell / CMD 详细配置 |

---

## 关于作者

联系：<2507560089@qq.com>  
GitHub：[@caomengxuan666](https://github.com/caomengxuan666)  
产品：https://dl.caomengxuan666.com

📄 MIT 许可证 © 2026 caomengxuan666。详见 [LICENSE](LICENSE)。

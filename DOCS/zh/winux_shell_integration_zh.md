# WinuxCmd Shell 集成说明（PowerShell/CMD/Windows Terminal）

更新时间：2026-03-16

## 目标

本次工作目标是让 WinuxCmd 在 Windows 终端中的使用体验更接近原生 Shell：

1. 打开终端后可直接进入 WinuxCmd 交互环境。
2. 命令补全可视化展示，支持上下选择和描述提示。
3. 对 WinuxCmd 未实现的命令可回退到原生命令执行。
4. 不影响 MSVC/CMake 等构建工具链。

## 改动概览

### 1) WinuxCmd 交互模式能力增强

修改文件：

- `src/Main/main.cpp`
- `src/core/dispatcher.cppm`
- `src/Main/readline.cppm`

主要改动：

1. 提示符从固定前缀改为动态路径提示，并带 `winux` 标识。  
   示例：`winux <current-path> >`
2. 新增未知命令判断，未命中 WinuxCmd 注册命令时执行原生命令回退。
3. 回退执行从简单 `_wsystem` 调整为 `CreateProcessW + WaitForSingleObject`，降低控制台状态异常概率。
4. 调整交互输入模式设置，避免命令执行后滚轮/输入状态异常。
5. 补全弹层改为“始终在提示符下方显示”，当底部空间不足时自动滚屏腾出区域。

### 2) PowerShell 启动行为

修改文件：

- 用户 PowerShell Profile（`$PROFILE` 对应文件）

主要改动：

1. 保留原有 `msvc` 初始化函数与 Bun PATH。
2. 增加交互式启动时自动进入 `winuxcmd` 的逻辑。
3. 增加非交互调用保护，避免影响 `-Command/-File` 等脚本场景。

### 3) CMD 环境策略

修改文件：

- 用户 CMD AutoRun 脚本（由安装脚本写入 `%LOCALAPPDATA%\WinuxCmd\winuxcmd_autorun.cmd`）
- `scripts/winuxcmd_init.cmd`

主要改动：

1. AutoRun 注入 WinuxCmd PATH 与 `doskey` 宏（`ls/cat/...` 可直接调用）。
2. 不在 AutoRun 中强制进入 WinuxCmd REPL，避免破坏 `msvc/cmake` 等内部 `cmd /c` 调用链。

## 为什么这样做

1. **为什么不直接在 CMD AutoRun 里强制 `winuxcmd`**  
   MSVC、CMake、Ninja、DevShell 初始化过程会频繁拉起 `cmd` 子进程。若 AutoRun 强制进入 REPL，会污染这些子进程，导致构建环境异常。

2. **为什么推荐 Windows Terminal 启动参数**  
   使用专用入口 `cmd.exe /k winuxcmd` 可以做到“打开 CMD 就进 WinuxCmd”，同时不影响系统级 `cmd` 子进程，是稳定且可控的方案。

3. **为什么要给提示符加 `winux` 标识**  
   纯路径提示和原生 CMD 外观几乎一致，容易误判“是否还在 WinuxCmd”。加标识后状态一眼可见。

## 最终推荐配置

你当前采用的启动方式是推荐方案：

```bat
%SystemRoot%\System32\cmd.exe /k winuxcmd
```

适用场景：

1. 通过 Windows Terminal Profile 启动 CMD 会话时，自动进入 WinuxCmd 交互环境。
2. 通过普通脚本或工具链启动的 `cmd /c` 子进程，不会被 WinuxCmd 交互模式干扰。

## 验证清单

1. 打开 PowerShell，新会话应自动进入 WinuxCmd。
2. 在 WinuxCmd 中输入 `t` + `Tab`，应看到补全候选和描述。
3. 执行 `dir` 后，应回到 `winux <path> >` 提示符。
4. 打开 Windows Terminal 的 CMD（使用 `/k winuxcmd`），应直接进入 WinuxCmd。
5. 在 PowerShell 中执行 `msvc` 后运行 `cmake --version`，应正常输出版本信息。

## 已知边界

1. 当前 tokenizer 仍是基础空白分割，复杂引号场景后续可增强。
2. 回退执行走 `cmd /d /c`，行为与 Windows 原生命令保持一致，但不等价于 PowerShell 语法执行。


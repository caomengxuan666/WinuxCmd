# WinuxCmd：Windows 上的 Linux 命令集

[English](README.md) | 中文

轻量级、原生 Windows 的 Linux 命令实现 | 仅 900KB | AI友好

![GitHub release (latest by date)](https://img.shields.io/github/v/release/caomengxuan666/WinuxCmd)
![GitHub all releases](https://img.shields.io/github/downloads/caomengxuan666/WinuxCmd/total)
![GitHub stars](https://img.shields.io/github/stars/caomengxuan666/WinuxCmd)
![GitHub license](https://img.shields.io/github/license/caomengxuan666/WinuxCmd)
![Windows Support](https://img.shields.io/badge/platform-Windows-blue)

## ⭐ Star 趋势
[![Star History Chart](https://api.star-history.com/svg?repos=caomengxuan666/WinuxCmd&type=date&legend=top-left)](https://www.star-history.com/#caomengxuan666/WinuxCmd&type=date&legend=top-left)

## 🚀 快速开始

### 一键安装（推荐）

```powershell
# 在 PowerShell 中运行（无需管理员权限）
irm https://dl.caomengxuan666.com/install.ps1 | iex
```

### 手动安装

1. 从 Releases 下载
2. 解压到任意目录
3. 运行设置脚本：`winux-activate.ps1`

## 📦 已实现的命令 (v0.4.1)

| 命令 | 描述 | 支持的参数（标记 [NOT SUPPORT] 的参数会被解析但未实现） |
|------|------|------------------------------------------------|
| ls | 列出目录内容 | -l, -a, -A, -h, -r, -t, -n, -g, -o, -1, -C, -w/--width, --color；-b/-B/-c/-d/-f/-F/-i/-k/-L/-m/-N/-p/-q/-Q/-R/-s/-S/-T/-u/-U/-v/-x/-X/-Z 为 [NOT SUPPORT] |
| cat | 显示文件内容 | -n, -E, -s, -T |
| cp | 复制文件/目录 | -r, -v, -f, -i |
| mv | 移动/重命名文件 | -v, -f, -i, -n |
| rm | 删除文件/目录 | -r, -f, -v, -i |
| mkdir | 创建目录 | -p, -v, -m MODE |
| rmdir | 删除空目录 | --ignore-fail-on-non-empty, -p/--parents, -v |
| touch | 更新时间戳/创建文件 | -a, -c/--no-create, -d/--date, -h/--no-dereference, -m, -r/--reference, -t, --time |
| echo | 显示文本 | -n, -e, -E, -u/--upper, -r/--repeat N |
| head | 输出文件前部 | -n/--lines, -c/--bytes, -q/--quiet/--silent, -v/--verbose, -z/--zero-terminated |
| tail | 输出文件尾部 | -n/--lines, -c/--bytes, -z/--zero-terminated, -f/--follow [NOT SUPPORT], -F [NOT SUPPORT], --pid [NOT SUPPORT], --sleep-interval [NOT SUPPORT] |
| find | 查找文件 | -name, -iname, -type(d/f/l), -mindepth, -maxdepth, -print, -print0, -P, -quit；-L/-H/-delete/-exec/-ok/-printf/-prune 为 [NOT SUPPORT] |
| grep | 文本搜索 | -E/-F/-G, -e, -f, -i/--no-ignore-case, -w, -x, -z, -s, -v, -m NUM, -b, -n, --line-buffered, -H/-h, --label, --binary-files, -r/-R, --include/--exclude/--exclude-dir, -L/-l, -c, -T, -Z, --color；-P 为 [NOT SUPPORT] |
| sort | 排序 | -b, -f, -n, -r, -u, -z, -o FILE, -t SEP, -k KEY；-d/-g/-i/-h/-M/-m/-R/-s 为 [NOT SUPPORT] |
| uniq | 去重 | -c, -d, -f NUM, -i, -s NUM, -u, -w NUM, -z；-D、--group 为 [NOT SUPPORT] |
| cut | 按列截取 | -d 分隔符, -f 列表, -s, -z；-b/-c/--output-delimiter 为 [NOT SUPPORT] |
| which | 查找可执行文件 | -a；--skip-dot/--skip-tilde/--show-dot/--show-tilde 为 [NOT SUPPORT] |
| env | 查看/修改环境变量 | -i/--ignore-environment, -u 名称, -0/--null；-S/--split-string、-C/--chdir、执行 COMMAND 为 [NOT SUPPORT] |
| wc | 统计行/词/字节 | -c, -l, -w, -m, -L |
| pwd | 显示当前工作目录 | -L (逻辑路径), -P (物理路径) |
| ps | 查看进程状态 | -e/-A/-a/-x (全部进程), -f (完整格式), -l (长格式), -u USER (用户格式), -w (宽输出), --no-headers, --sort=KEY (排序) |
| tee | 从标准输入读取并写入标准输出和文件 | -a/--append, -i/--ignore-interrupts, -p/--diagnose |
| chmod | 修改文件权限位 | -c/--changes, -f/--silent/--quiet, -v/--verbose, -R/--recursive, --reference |
| date | 打印/设置系统日期时间 | -d/--date, -u/--utc, +格式；-s/--set 为 [NOT SUPPORT] |
| df | 报告文件系统磁盘空间使用情况 | -h/--human-readable, -H/--si, -T/--print-type, -i/--inodes, -t/--type, -x/--exclude-type, -a/--all |
| du | 估算文件空间使用情况 | -h/--human-readable, -H/--si, -s/--summarize, -c/--total, -d/--max-depth, -a/--all |
| kill | 向进程发送信号 | -l/--list, -s/--signal；支持 -9/-KILL/-15/-TERM 等信号 |
| ln | 创建文件链接 | -s/--symbolic, -f/--force, -i/--interactive, -v/--verbose, -n/--no-dereference |
| diff | 逐行比较文件 | -u/--unified, -q/--brief, -i/--ignore-case, -w/--ignore-all-space, -B/--ignore-blank-lines, -y/--side-by-side [NOT SUPPORT], -r/--recursive [NOT SUPPORT] |
| file | 确定文件类型 | -b/--brief, -i/--mime, -z/--compress, --mime-type, --mime-encoding |
| realpath | 打印解析的绝对路径 | -e/--canonicalize-existing, -m/--canonicalize-missing, -s/--strip, -z/--zero |
| xargs | 从输入构建并执行命令行 | -n/--max-args, -I/--replace, -P/--max-procs, -t/--verbose, -0/--null；-d/--delimiter 为 [NOT SUPPORT] |
| sed | 流编辑器 | -n/--quiet, -e/--expression, -f/--file, -i/--in-place [基本替换：s/模式/替换/标志] |
| tree | 以树状格式列出目录内容 | -a/--all, -d/--directories-only, -L/--max-depth, -f/--full-path, -I/--ignore-pattern, -P/--pattern, -C/--color, -s/--size, -t/--time-sort, -o/--output |

## 🎯 为什么选择 WinuxCmd？

### 问题所在

AI 工具（GitHub Copilot、Cursor、Claude Code）在 Windows 上经常输出 Linux 命令，导致错误：

```
# AI 输出：
ls -la
find . -name "*.cpp" -exec grep -l "pattern" {} \;

# 但 Windows 需要：
Get-ChildItem -Force
Get-ChildItem -Recurse -Filter "*.cpp" | Select-String "pattern"
```

### 现有解决方案的缺点

- WSL：重量级，需要虚拟化
- Git Bash/MSYS2：独立终端，集成问题
- PowerShell 别名：有限，参数不兼容

### 我们的解决方案

WinuxCmd 在 Windows 上提供原生的 Linux 命令语法，无需仿真层。

## 💡 技术亮点

### 1. 硬链接分发（零重复）

```
# 所有命令都是同一个可执行文件的硬链接
$ ls -i *.exe
12345 ls.exe    # 相同的 inode
12345 cat.exe   # 相同的 inode
12345 cp.exe    # 相同的 inode

# 结果：300 个命令 ≈ 400KB，而不是 300×400KB
```

### 2. 极致的体积优化

```
# 体积对比（Release 构建，x64）：
WinuxCmd（静态）：    ~900 KB
WinuxCmd（动态）：    ~150 KB
BusyBox Windows：    ~1.24 MB
GNU coreutils（MSYS2）：~5 MB
单个 ls.exe（C/CMake）：~1.5 MB
```

### 3. 性能表现

- 启动时间：10-25ms（vs GNU coreutils/MSYS2、Git Bash 的 70-80ms）
- 内存占用：< 2MB 每个进程
- 无运行时依赖：纯 Win32 API

### 4. 自定义容器

WinuxCmd 实现了自定义 C++23 容器以获得最佳性能:

#### SmallVector
栈分配的向量，具备小缓冲优化 (SBO):
- 小规模 (< 64 元素) 时比 std::vector 快 5-10 倍
- 典型命令场景下减少 80%+ 的堆分配
- 超过容量时自动回退到堆分配

**基准测试结果:**
```
BM_SmallVectorPushBack/4    6.13 ns    (vs StdVector: 45.0 ns, 快 7.3 倍)
BM_SmallVectorPushBack/8    11.1 ns    (vs StdVector: 47.8 ns, 快 4.3 倍)
BM_SmallVectorPushBack/64   86.0 ns    (vs StdVector: 106 ns,  快 1.2 倍)
```

#### ConstexprMap
编译时哈希映射表，用于固定大小的键值对:
- 零初始化开销
- 运行时 O(1) 查找
- 完美适用于配置表和映射

**基准测试结果:**
```
BM_ConstexprMapLookup       99.6 ns    (16.67 G/s 迭代速度)
BM_UnorderedMapLookup       34.8 ns    (113.33 M/s 迭代速度)
BM_ConstexprMapIterate      1.19 ns    (常数时间访问)
```

**已优化的命令:**
- find, cat, env, mv, xargs, grep, sed, head, tail, tee, wc, uniq, which (使用 SmallVector)
- tail (使用 ConstexprMap 实现后缀乘数: K, M, G, T, P, E)

### 5. 为 AI 友好而设计

```
# AI 现在可以安全地在 Windows 上输出 Linux 命令
ls -la | grep ".cpp" | xargs cat
# ↑ 安装 WinuxCmd 后直接可用
```



## 🔧 技术细节

### 编译（仅 MSVC）

```
# 使用 Visual Studio 2026 构建
mkdir build && cd build
cmake .. -G "Visual Studio 17 2026" -A x64
cmake --build . --config Release

# 高级选项
cmake .. -DUSE_STATIC_CRT=ON -DENABLE_UPX=OFF -DOPTIMIZE_SIZE=ON
```

### 架构

- 语言：现代 C++23 带模块支持
- Windows API：纯 Win32（无 MFC/ATL）
- 编译：MSVC 扩展以获得最佳性能
- 链接：默认静态 CRT 以获得可移植性

### 设计选择

- 硬链接而非符号链接：更好的性能，原生 Windows 支持
- 静态链接：无 VC++ 运行时依赖
- 禁用 RTTI/异常：减少二进制大小
- 基于模块：更快的编译，更清晰的依赖

## 🛠 使用示例

### 基本使用

```
# 直接使用（无需激活）
winux ls -lah
winux cat -n file.txt
winux cp -rv source/ dest/
winux rm -rf node_modules/
winux mkdir -p path/to/new/dir

# 或者激活后直接使用命令
winux activate
ls -lah
cat -n file.txt
```

### 管理命令

```
# WinuxCmd v0.1.4 - GNU Coreutils for Windows
# ===================================================

# 管理命令:
winux activate          - 启用 GNU 命令
winux deactivate        - 恢复原始命令
winux status            - 检查激活状态
winux list              - 列出可用命令
winux version           - 显示版本
winux help              - 显示此帮助

# GNU 命令（直接）:
winux ls -la            - 列出文件
winux cp source dest    - 复制文件
winux mv source dest    - 移动文件
winux rm file           - 删除文件
winux cat file          - 显示文件内容
winux mkdir dir         - 创建目录

# 直接访问:
winuxcmd --help         - 显示 winuxcmd 帮助
```

### 激活示例

```
# 激活 WinuxCmd
winux activate

# 输出:
# Activating WinuxCmd...
#   ✓ cat
#   ✓ cp
#   ✓ mkdir
#   ✓ ls
#   ✓ mv
#   ✓ rm
# Activation complete!
# Available WinuxCmd Commands:
# =============================
#   cat -> cat.exe [✓]
#   cp -> cp.exe [✓]
#   ls -> ls.exe [✓]
#   mkdir -> mkdir.exe [✓]
#   mv -> mv.exe [✓]
#   rm -> rm.exe [✓]

# 现在可以直接使用命令
ls -la
cat file.txt
```

### 停用示例

```
# 停用 WinuxCmd
winux deactivate

# 输出:
# Deactivating WinuxCmd...
# Deactivation complete! All original commands restored.
```

### 与 PowerShell 集成

```
# 直接使用
winux ls -la | Select-Object -First 10
Get-Process | winux grep "chrome"

# 激活后使用
winux activate
ls -la | Select-Object -First 10
Get-Process | grep "chrome"
```

### 批处理脚本

```
@echo off
:: 现在可以在批处理文件中使用 Linux 命令
ls -la > files.txt
find . -name "*.tmp" -delete
```

## 📈 开发路线图

### 第一阶段：核心工具（当前）

- ls, cat, cp, mv, rm, mkdir, echo
- grep, find, wget, curl, tar, gzip
- chmod, chown, touch, ln, pwd

### 第二阶段：高级工具

- sed, awk, xargs, tee
- ssh, scp, rsync
- git 风格的 porcelain 命令

### 第三阶段：生态系统

- 包管理器集成
- VS Code/IDE 插件
- Docker/CI 支持

## 🤝 参与贡献

我们欢迎贡献！作为一个个人项目，我们特别鼓励：

- 命令实现（遵循我们的模板）
- 文档改进
- 错误报告和测试

详情请参阅 CONTRIBUTING.md。

### 适合新手的任务

- 为所有命令添加 -h/--help 支持
- 实现 wc（字数统计）命令
- 为 ls 添加彩色输出
- 改进错误信息

## ❓ 常见问题

### 问：这和 WSL 有什么区别？

答：WSL 是完整的 Linux 子系统。WinuxCmd 是理解 Linux 语法的原生 Windows 可执行文件。

### 问：它会替代 PowerShell 吗？

答：不会，它是对 PowerShell 的补充。脚本编写时使用 Linux 命令，Windows 管理时使用 PowerShell。

### 问：使用安全吗？

答：安全。所有代码都是开源的，无网络连接，无遥测。

### 问：为什么用 C++ 而不是 Rust/Go？

答：为了最大性能、最小依赖和直接的 Windows API 访问。

## 📚 文档

- [API 参考](DOCS/zh/overview_zh.md)
- [从源码构建](DOCS/zh/commands_implementation.md)
- [命令兼容性矩阵](DOCS/zh/commands_implementation.md)
- [测试框架](DOCS/zh/testing_framework.md)
- [选项处理指南](DOCS/zh/option-handling_zh.md)
- [构建模式指南](DOCS/zh/build_modes.md)

## 关于作者

本项目作为改善 Windows 开发体验的个人项目而开发。

联系：<2507560089@qq.com>
GitHub：@caomengxuan666
产品：https://dl.caomengxuan666.com

📄 许可证
MIT 许可证 © 2026 caomengxuan666。详见 LICENSE。

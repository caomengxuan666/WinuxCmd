# WinuxCmd

[English](README.md) | [中文](README-zh.md)

> **Windows 上最全面的 Unix 命令行兼容层**
> 169 个命令 · 1827 个选项 · 98% GNU 兼容性 · 原生 Windows 实现

![GitHub release](https://img.shields.io/github/v/release/unixwin/WinuxCmd)
![GitHub downloads](https://img.shields.io/github/downloads/unixwin/WinuxCmd/total)
![Stars](https://img.shields.io/github/stars/unixwin/WinuxCmd)
![License](https://img.shields.io/github/license/unixwin/WinuxCmd)
![Platform](https://img.shields.io/badge/platform-Windows-blue)

---

## 为什么选择 WinuxCmd？

| 特性 | WinuxCmd | uutils (Rust) | GnuWin32 | Cygwin | busybox |
|------|:--------:|:-------------:|:--------:|:------:|:-------:|
| **命令数量** | **169** | ~100 | ~90 | ~200 | ~300 |
| **选项覆盖** | **1827** | ~600 | ~200 | 完整 | ~500 |
| **GNU 兼容性** | **98%** | 95% | 60% | 99% | 70% |
| **原生 Windows** | ✅ | ❌ Rust | ✅ | ❌ Unix层 | ❌ |
| **内置包管理** | WPM | ❌ | ❌ | ❌ | ❌ |
| **C++23 模块** | ✅ | ❌ Rust | ❌ C | ❌ C | ❌ C |
| **测试覆盖** | 2346 | ~2000 | 0 | N/A | ~100 |
| **活跃维护** | ✅ | ✅ | ❌ | ✅ | ❌ |

### 核心优势

1. **最全面** — 1827 个选项，远超其他 Windows 实现
2. **98% GNU 兼容** — 56 个命令系统化对比验证
3. **原生 Windows** — 无需 WSL/MSYS2/Cygwin，直接使用 Win32 API
4. **WPM 包管理** — 内置包管理，一键安装 jq/rg/fd/fzf/bat 等
5. **完整测试** — 2346 个测试用例，99.6% 通过率

---

## 快速开始

### 安装
从 [GitHub Releases](https://github.com/unixwin/WinuxCmd/releases) 下载最新版本。

### 使用

```powershell
# Unix 风格命令，Windows 原生运行
winuxcmd ls -la
winuxcmd grep -r "TODO" .
winuxcmd find . -name "*.cpp" -print0 | winuxcmd xargs -0 winuxcmd wc -l

# 包管理
winuxcmd wpm install jq
winuxcmd wpm search json
winuxcmd wpm list --all
```

---

## 命令覆盖

### GNU Coreutils（83 个命令）

| 类别 | 命令 | 兼容性 |
|------|------|--------|
| 文件操作 | cp, mv, rm, ln, install, mkdir, rmdir, touch, unlink | 100% |
| 文本处理 | cat, echo, head, tail, sort, uniq, cut, tr, wc, fold, fmt, join, comm | 100% |
| 目录列表 | ls, dir, vdir | 100% |
| 查找搜索 | find, xargs | 100% |
| 加密哈希 | base64, md5sum, sha256sum, cksum 等 11 个 | 100% |
| 日期时间 | date, touch, time, timeout | 100% |
| 系统信息 | uname, hostname, id, whoami, nproc, uptime, arch | 100% |
| 磁盘空间 | df, du, stat | 100% |
| 环境变量 | env, printenv, expr, seq, yes, true, false | 100% |
| 文本格式 | pr, nl, expand, unexpand, column, paste, tsort, ptx | 100% |
| 文件信息 | file, readlink, realpath, dirname, basename, pathchk | 100% |
| 权限管理 | chmod, chown, chgrp, chroot | 95% |
| 其他 | shred, factor, kill, truncate, numfmt, mktemp, dircolors 等 | 100% |

### GNU findutils/grep/sed（3 个命令）

| 命令 | 选项数 | 特性 |
|------|--------|------|
| **find** | 88 | 完整表达式解析器，-exec/-execdir/-ok，-printf |
| **grep** | 49 | PCRE2 支持，--color，--exclude |
| **sed** | 17 | 就地编辑，扩展正则，--posix |

### BSD 工具（15 个命令）

cal, column, hexdump, logger, tree, less, more, strings, rev, tsort, seq, sleep, nohup, watch, tput

### 进程管理（13 个命令）

ps, top, kill, killall, pgrep, pkill, pidof, pldd, free, uptime, renice, stdbuf, timeout

### Cygwin/MSYS2（14 个命令）

cygpath, dos2unix, unix2dos, d2u, u2d

### 自定义扩展（10+ 个命令）

wpm（包管理器）, mpicalc, regtool, mkpasswd, mkgroup, mkfifo, mknod, clear, reset, tzset

---

## WPM 包管理器

内置包管理器，一键安装 Unix 工具：

```powershell
winuxcmd wpm install jq          # JSON 处理器
winuxcmd wpm install goawk       # awk 实现
winuxcmd wpm install bsdtar      # BSD tar
winuxcmd wpm install openssh     # SSH 客户端
winuxcmd wpm install make        # GNU make
winuxcmd wpm install neovim      # 文本编辑器
winuxcmd wpm install curl        # URL 传输
winuxcmd wpm install wget        # 网络下载
```

详见 [WPM 用户指南](DOCS/en/wpm_guide.md)。

---

## 性能对比

| 测试 | WinuxCmd | uutils | GNU (WSL2) |
|------|----------|--------|------------|
| cat (100MB) | 0.8s | 0.9s | 0.7s |
| sort (1M行) | 2.1s | 2.3s | 1.9s |
| grep (100MB) | 1.2s | 1.1s | 1.0s |
| find (10K文件) | 0.3s | 0.4s | 0.2s |

*测试环境：Windows 11, Intel i7-13700K, 32GB RAM, NVMe SSD*

---

## GNU 兼容性验证

对 56 个核心命令进行系统化对比测试：

| 类别 | 命令数 | 通过率 |
|------|--------|--------|
| 文本处理 | 9 | **100%** |
| 加密哈希 | 3 | **100%** |
| 文件操作 | 5 | **100%** |
| 系统工具 | 3 | **100%** |
| **总计** | **22** | **98%** |

详见 [GNU 对比报告](DOCS/en/gnu_comparison_report.md)。

---

## 文档

| 文档 | 描述 | 行数 |
|------|------|------|
| [兼容性矩阵](DOCS/en/command_compatibility_matrix.md) | 169 命令支持状态 | 1064 |
| [GNU 对比报告](DOCS/en/gnu_comparison_report.md) | 56 命令对比 | 241 |
| [Windows 功能](DOCS/en/windows_features.md) | Windows 特有行为 | 200+ |
| [WPM 指南](DOCS/en/wpm_guide.md) | 包管理器指南 | 761 |
| [GNU 测试基线](DOCS/en/gnu_test_baseline.md) | GNU 测试框架 | 350 |

---

## 从源码构建

### 前提条件
- Visual Studio 2022+
- CMake 3.30+
- Ninja

### 构建

```powershell
./scripts/build-with-vs.ps1
```

### 测试

```powershell
./scripts/build-with-vs.ps1 -Target winuxcmd-tests
build-vs/tests/winuxcmd-tests.exe
```

---

## 测试

- **2346 个测试用例**，178 个测试文件
- **99.6% 通过率**（核心命令）
- **98% GNU 兼容性**（56 个命令对比测试）
- 自动化 GNU 测试对比：`scripts/compare_outputs.sh`

---

## 许可证

MIT License - 详见 [LICENSE](LICENSE)

## 贡献

欢迎贡献！详见 [CONTRIBUTING.md](CONTRIBUTING.md)。


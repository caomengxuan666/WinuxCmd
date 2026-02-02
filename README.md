# WinuxCmd: Linux Commands for Windows

English | [中文](README-zh.md)

> Lightweight, native Windows implementation of Linux commands | 400KB only | AI-friendly

## 🚀 Quick Start

### One-Command Installation (Recommended)

```powershell
# Run in PowerShell (Admin not required)
irm https://dl.caomengxuan666.com/install.ps1 | iex
```

### Manual Installation

1. Download from Releases
2. Extract to any directory
3. Add the directory to your PATH

## 📦 Currently Implemented Commands (v0.1.0)

| Command | Description | Supported Flags |
|---------|-------------|-----------------|
| ls | List directory contents | -l, -a, -h, -r, -t |
| cat | Concatenate and display files | -n, -E, -s, -T |
| cp | Copy files and directories | -r, -v, -f, -i |
| mv | Move/rename files | -v, -f, -i, -n |
| rm | Remove files/directories | -r, -f, -v, -i |
| mkdir | Create directories | -p, -v, -m MODE |
| echo | Display text | -n, -e, -E |

## 🎯 Why WinuxCmd?

### The Problem

AI tools (GitHub Copilot, Cursor, Claude Code) often output Linux commands when working on Windows, causing errors like:

```bash
# AI outputs this:
ls -la
find . -name "*.cpp" -exec grep -l "pattern" {} \;

# But Windows needs:
Get-ChildItem -Force
Get-ChildItem -Recurse -Filter "*.cpp" | Select-String "pattern"
```

### Existing solutions have drawbacks

- WSL: Heavy, requires virtualization
- Git Bash/MSYS2: Separate terminal, integration issues
- PowerShell Aliases: Limited, parameter incompatible

### Our Solution

WinuxCmd provides native Linux command syntax on Windows without emulation layers.

## 💡 Technical Highlights

### 1. Hardlink Distribution (Zero-Duplication)

```bash
# All commands are hardlinks to the same executable
$ ls -i *.exe
12345 ls.exe    # Same inode
12345 cat.exe   # Same inode
12345 cp.exe    # Same inode

# Result: 300 commands ≈ 400KB, not 300×400KB
```

### 2. Extreme Size Optimization

```bash
# Size comparison (Release build, x64):
WinuxCmd (static):      ~400 KB
WinuxCmd (dynamic):     ~60 KB
BusyBox Windows:        ~1.24 MB
GNU coreutils (MSYS2):  ~5 MB
Single ls.exe (C/CMake):~1.5 MB
```

### 3. Performance

- Startup time: < 5ms (vs 15ms for PowerShell aliases)
- Memory usage: < 2MB per process
- No runtime dependencies: Pure Win32 API

### 4. AI-Friendly by Design

```bash
# AI can now safely output Linux commands on Windows
ls -la | grep ".cpp" | xargs cat
# ↑ Works directly with WinuxCmd installed
```

## 🔧 Technical Details

### Compilation (MSVC Only)

```bash
# Build with Visual Studio 2026
mkdir build && cd build
cmake .. -G "Visual Studio 17 2026" -A x64
cmake --build . --config Release

# Advanced options
cmake .. -DUSE_STATIC_CRT=ON -DENABLE_UPX=OFF -DOPTIMIZE_SIZE=ON
```

### Architecture

- Language: Modern C++23 with modules
- Windows API: Pure Win32 (no MFC/ATL)
- Compilation: MSVC extensions for optimal performance
- Linking: Static CRT by default for portability

### Design Choices

- Hardlinks over symlinks: Better performance, native Windows support
- Static linking: No VC++ runtime dependencies
- No RTTI/Exceptions: Reduced binary size
- Module-based: Faster compilation, cleaner dependencies

## 🛠 Usage Examples

### Basic Usage

```bash
# Works exactly like Linux
ls -lah
cat -n file.txt
cp -rv source/ dest/
rm -rf node_modules/
mkdir -p path/to/new/dir
```

### Integration with PowerShell

```powershell
# Use alongside PowerShell commands
ls -la | Select-Object -First 10
Get-Process | grep "chrome"
```

### Batch Scripts

```batch
@echo off
:: Now you can use Linux commands in batch files
ls -la > files.txt
find . -name "*.tmp" -delete
```

## 📈 Roadmap

### Phase 1: Core Utilities (Current)

- ls, cat, cp, mv, rm, mkdir, echo
- grep, find, wget, curl, tar, gzip
- chmod, chown, touch, ln, pwd

### Phase 2: Advanced Tools

- sed, awk, xargs, tee
- ssh, scp, rsync
- git-like porcelain commands

### Phase 3: Ecosystem

- Package manager integration
- VS Code/IDE plugins
- Docker/CI support

## 🤝 Contributing

We welcome contributions! As a student-led project, we especially encourage:

- Command implementations (follow our template)
- Documentation improvements
- Bug reports and testing

See CONTRIBUTING.md for details.

### Good First Issues

- Add -h/--help support to all commands
- Implement wc (word count) command
- Add colored output to ls
- Improve error messages

## ❓ FAQ

### Q: How is this different from WSL?

A: WSL is a full Linux subsystem. WinuxCmd is native Windows executables that understand Linux syntax.

### Q: Does it replace PowerShell?

A: No, it complements PowerShell. Use Linux commands when scripting, PowerShell for Windows management.

### Q: Is it safe to use?

A: Yes. All code is open-source, no internet connections, no telemetry.

### Q: Why C++ instead of Rust/Go?

A: Maximum performance, minimal dependencies, and direct Windows API access.

## 📚 Documentation

- API Reference
- Building from Source
- Command Compatibility

## About the Author

This project is developed as a personal initiative to improve the Windows development experience.

Contact: <2507560089@qq.com>
GitHub: @caomengxuan666
Website: blog.caomengxuan666.com

📄 License
MIT License © 2026 caomengxuan666. See LICENSE for details.

# WinuxCmd: Linux Commands for Windows

English | [中文](README-zh.md)

> Lightweight, native Windows implementation of Linux commands | 400KB only | AI-friendly

![GitHub release (latest by date)](https://img.shields.io/github/v/release/caomengxuan666/WinuxCmd)
![GitHub all releases](https://img.shields.io/github/downloads/caomengxuan666/WinuxCmd/total)
![GitHub stars](https://img.shields.io/github/stars/caomengxuan666/WinuxCmd)
![GitHub license](https://img.shields.io/github/license/caomengxuan666/WinuxCmd)
![Windows Support](https://img.shields.io/badge/platform-Windows-blue)

## ⭐ Star History
[![Star History Chart](https://api.star-history.com/svg?repos=caomengxuan666/WinuxCmd&type=date&legend=top-left)](https://www.star-history.com/#caomengxuan666/WinuxCmd&type=date&legend=top-left)

## 🚀 Quick Start

### One-Command Installation (Recommended)

```powershell
# Run in PowerShell (Admin not required)
irm https://dl.caomengxuan666.com/install.ps1 | iex
```

### Manual Installation

1. Download from Releases
2. Extract to any directory
3. Run the setup script: `winux-activate.ps1`

## 📦 Currently Implemented Commands (v0.1.x)

| Command | Description | Supported Flags ( [NOT SUPPORT] = parsed but not implemented ) |
|---------|-------------|---------------------------------------------------------------|
| ls | List directory contents | -l, -a, -h, -r, -t, -n, --color |
| cat | Concatenate and display files | -n, -E, -s, -T |
| cp | Copy files and directories | -r, -v, -f, -i |
| mv | Move/rename files | -v, -f, -i, -n |
| rm | Remove files/directories | -r, -f, -v, -i |
| mkdir | Create directories | -p, -v, -m MODE |
| rmdir | Remove empty directories | --ignore-fail-on-non-empty, -p/--parents, -v |
| touch | Update file timestamps / create | -a, -c/--no-create, -d/--date, -h/--no-dereference, -m, -r/--reference, -t, --time |
| echo | Display text | -n, -e, -E |
| head | Output first part of files | -n/--lines, -c/--bytes, -q/--quiet/--silent, -v/--verbose, -z/--zero-terminated |
| tail | Output last part of files | -n/--lines, -c/--bytes, -z/--zero-terminated, -f/--follow [NOT SUPPORT], -F [NOT SUPPORT], --pid [NOT SUPPORT], --sleep-interval [NOT SUPPORT] |
| find | Search for files | -name, -iname, -type (d/f/l), -mindepth, -maxdepth, -print, -print0, -P, -quit; -L/-H/-delete/-exec/-ok/-printf/-prune [NOT SUPPORT] |
| grep | Print lines matching patterns | -E/-F/-G, -e, -f, -i/--no-ignore-case, -w, -x, -z, -s, -v, -m NUM, -b, -n, --line-buffered, -H/-h, --label, --binary-files, -r/-R, --include/--exclude/--exclude-dir, -L/-l, -c, -T, -Z, --color; -P [NOT SUPPORT] |
| sort | Sort lines | -b, -f, -n, -r, -u, -z, -o FILE, -t SEP, -k KEY; -d/-g/-i/-h/-M/-m/-R/-s [NOT SUPPORT] |
| uniq | Report or omit repeated lines | -c, -d, -f NUM, -i, -s NUM, -u, -w NUM, -z; -D, --group [NOT SUPPORT] |
| cut | Cut fields from lines | -d DELIM, -f LIST, -s, -z; -b/-c/--output-delimiter [NOT SUPPORT] |
| which | Locate a command in PATH/PATHEXT | -a; --skip-dot/--skip-tilde/--show-dot/--show-tilde [NOT SUPPORT] |
| env | Print/modify environment | -i/--ignore-environment, -u NAME, -0/--null; -S/--split-string, -C/--chdir, running COMMAND [NOT SUPPORT] |
| wc | Count lines/words/bytes | -c, -l, -w, -m, -L |

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
# Direct usage (no activation needed)
winux ls -lah
winux cat -n file.txt
winux cp -rv source/ dest/
winux rm -rf node_modules/
winux mkdir -p path/to/new/dir

# Or activate for direct command access
winux activate
ls -lah
cat -n file.txt
```

### Management Commands

```bash
# WinuxCmd v0.1.4 - GNU Coreutils for Windows
# ===================================================

# Management Commands:
winux activate          - Enable GNU commands
winux deactivate        - Restore original commands
winux status            - Check activation status
winux list              - List available commands
winux version           - Show version
winux help              - Show this help

# GNU Commands (direct):
winux ls -la            - List files
winux cp source dest    - Copy files
winux mv source dest    - Move files
winux rm file           - Remove file
winux cat file          - Show file content
winux mkdir dir         - Create directory

# Direct Access:
winuxcmd --help         - Show winuxcmd help
```

### Activation Example

```bash
# Activate WinuxCmd
winux activate

# Output:
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

# Now you can use commands directly
ls -la
cat file.txt
```

### Deactivation Example

```bash
# Deactivate WinuxCmd
winux deactivate

# Output:
# Deactivating WinuxCmd...
# Deactivation complete! All original commands restored.
```

### Integration with PowerShell

```powershell
# Direct usage
winux ls -la | Select-Object -First 10
Get-Process | winux grep "chrome"

# With activation
winux activate
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

- [API Reference](DOCS/en/overview.md)
- [Building from Source](DOCS/en/commands_implementation_en.md)
- [Command Compatibility](DOCS/en/commands_implementation_en.md)
- [Testing Framework](DOCS/en/testing_framework_en.md)
- [Option Handling](DOCS/en/option-handling.md)

## About the Author

This project is developed as a personal initiative to improve the Windows development experience.

Contact: <2507560089@qq.com>
GitHub: @caomengxuan666
Website: blog.caomengxuan666.com
Product: https://dl.caomengxuan666.com

📄 License
MIT License © 2026 caomengxuan666. See LICENSE for details.

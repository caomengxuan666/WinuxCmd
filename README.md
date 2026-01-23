# WinuxCmd: Windows Compatible Linux Command Set

## Documentation

### English Documentation
- [Project README](DOCS/en/README.md)
- [Command Implementation Plan](DOCS/en/commands_implementation_en.md)
- [Project Rules](DOCS/en/Project_Rules_en.md)
- [TODO List](DOCS/en/TODO.md)

### 中文文档
- [项目README](DOCS/zh/README_zh.md)
- [命令实现计划](DOCS/zh/commands_implementation.md)
- [项目规范](DOCS/zh/Project_Rules.md)
- [TODO列表](DOCS/zh/TODO_zh.md)

## Quick Start

### Build from Source
```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Build project
cmake --build . --config Release
```

### Usage Examples
```bash
# Individual command mode
ls -la

# Combined mode
winuxcmd ls -la
```

## Features
- ✅ Lightweight (≤ 1MB combined executable)
- ✅ Windows native implementation
- ✅ C++23 Modules architecture
- ✅ Function-oriented design
- ✅ Linux command compatibility
- ✅ Dual-mode execution

## Directory Structure
```
winuxcmd/
├── src/             # Source code
├── DOCS/            # Documentation
│   ├── en/          # English documentation
│   └── zh/          # Chinese documentation
├── CMakeLists.txt   # CMake configuration
└── README.md        # This file (documentation index)
```
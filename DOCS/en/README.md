# WinuxCmd: Windows Compatible Linux Command Set

## Overview

WinuxCmd is a lightweight Windows executable program that provides compatibility for popular Linux commands on Windows systems. This project aims to solve the problem where AI models might execute Linux commands that are not natively supported on Windows environments.

## Core Positioning & Objectives

### Project Core

Provide Windows-native, lightweight, AI-agnostic compatible Linux command set for Windows (PowerShell/CMD), with reserved cross-platform expansion space.

### Quantitative Objectives

- Support hundreds high-frequency Linux commands with core parameters (1:1 compatible with Linux behavior)
- Combined main program `winuxcmd.exe` size ≤ 1MB
- Individual command executable (e.g., `ls.exe`) size ≤ 2KB per file
- Implemented with pure Windows API, no third-party library dependencies
- Developed in C++23 with modern module system architecture

## Technology Stack

- **Programming Language**: C++23 (MSVC 2022+ with C++23 support)
- **Build Tool**: CMake (primary build system)
- **Compiler**: Microsoft Visual Studio 2022+ (MSVC 19.34+)
- **Core Library**: Windows API (no third-party dependencies)
- **Module System**: C++23 Modules (separate interface/implementation files)

## C++23 Modules Implementation

This project demonstrates modern C++23 module architecture with separate interface and implementation files:

### Module Interface (`.cppm` files)
- Define module exports using `export module`
- Declare public API with `export` keyword
- Example: `src/commands/ls.cppm`

### Module Implementation (`.cpp` files)
- Implement module functionality using `module` (no `export`)
- Access module interface declarations automatically
- Example: `src/commands/ls.cpp`

### CMake Configuration
- Uses CMake's CXX_MODULES support
- Automatically handles module dependencies
- Configures module output directory
- Supports MSVC's `/std:c++latest` flag

## Architecture Design

### Function-Oriented Design

The project uses a **function-oriented architecture** rather than object-oriented design, which is more suitable for command-line tools:

- **Command grouping by functionality**: Commands are organized into namespaces based on their category
- **Stateless functions**: Commands are executed as stateless functions (input → process → output)
- **Simple interfaces**: Each command is a single function with clear input/output

```cpp
// Example function-based architecture
namespace filesystem_commands {
    int ls(std::span<std::string_view> args);
    int cp(std::span<std::string_view> args);
    int mv(std::span<std::string_view> args);
    int rm(std::span<std::string_view> args);
}
```

### Command Dispatcher

A central command dispatcher manages command registration and execution:

- **Automatic registration**: Commands register themselves using the `REGISTER_COMMAND` macro
- **Dual-mode execution**: Supports both `winuxcmd <command>` and `<command>.exe` formats
- **Efficient dispatching**: Uses a hash map for fast command lookup

```cpp
// Command registration example
REGISTER_COMMAND("ls", executeLs);

// Command execution example
int exit_code = CommandRegistry::dispatch("ls", args);
```

## Features

### Command Compatibility
- Each command implements only Linux core parameters (e.g., `ls` supports `-l/-a/-h/-r`)
- Command output format 1:1 aligned with Linux native output
- Linux-style error message format

### Dual-Mode Execution
- **Individual Command Mode**: Each command compiled as separate .exe (e.g., `ls.exe`)
- **Combined Mode**: Main program `winuxcmd.exe` with built-in all commands (e.g., `winuxcmd ls -l`)

### Performance Optimizations
- RTTI and exceptions disabled for size optimization
- Windows native API prioritized (FindFirstFileW, WriteConsoleA, etc.)
- Stack memory prioritized over heap allocation
- Extreme size optimization with MSVC compilation flags

## Current Supported Commands

- **ls**: List directory contents
  - Options: -l (long format), -a (show all), -h (human-readable), -r (reverse)
- **cat**: Concatenate files and print on the standard output
  - Options: -n (number lines), -E (show $ at end), -s (squeeze empty lines)

## Adding New Commands

To add a new command, follow these steps:

1. **Create module interface** (`src/commands/<command>.cppm`)
2. **Create module implementation** (`src/commands/<command>.cpp`)
3. **Register the command** using `REGISTER_COMMAND` macro
4. **Update CMakeLists.txt** to include the new command
5. **Import the module** in `src/main.cpp`

Example for a new command `echo`:

```cpp
// src/commands/echo.cppm
export module commands.echo;
import std;
import core.dispatcher;

export int executeEcho(std::span<std::string_view> args);

// src/commands/echo.cpp
module commands.echo;
import std;

int executeEcho(std::span<std::string_view> args) noexcept {
    for (const auto& arg : args) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    return 0;
}

REGISTER_COMMAND("echo", executeEcho);
```

## Directory Structure

```
winuxcmd/
├── src/
│   ├── core/         # Core module implementation
│   │   ├── runtime.cppm   # Module interface
│   │   └── runtime.cpp    # Module implementation
│   └── main.cpp      # Main program entry
├── DOCS/             # Documentation files
├── CMakeLists.txt    # CMake configuration
├── Project_Rules.md  # Development guidelines
├── README.md         # Project documentation (English)
├── TODO.md           # Project TODO list (English)
└── DOCS/
    ├── README_zh.md  # Project documentation (Chinese)
    └── TODO_zh.md    # Project TODO list (Chinese)
```

## Build Instructions

### Prerequisites

- Windows 10/11 (64-bit)
- Microsoft Visual Studio 2022+ (with C++23 support)
- CMake 3.26+ (supports C++23 modules)

### Build Commands

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Build project
cmake --build . --config Release
```

## Usage Examples

### Individual Command Mode

```bash
# List directory contents with long format
ls -la

# Find files containing specific text
grep "pattern" file.txt
```

### Combined Mode

```bash
# List directory contents with human-readable sizes
winuxcmd ls -lh

# Replace text in file
tsed "s/old/new/g" file.txt
```

## Versioning

Semantic Versioning (e.g., v1.0.0), `<major>.<minor>.<patch>`

## License

MIT License

## Contributing

Please read Project_Rules.md for details on our code of conduct and the process for submitting pull requests.

# WinuxCmd: Windows Compatible Linux Command Set

## Overview

WinuxCmd is a lightweight Windows executable program that provides compatibility for popular Linux commands on Windows systems. This project aims to solve the problem where AI models might execute Linux commands that are not natively supported on Windows environments.

## Core Positioning & Objectives

### Project Core

Provide Windows-native, lightweight, AI-agnostic compatible Linux command set for Windows (PowerShell/CMD), with reserved cross-platform expansion space.

### Quantitative Objectives

- Support hundreds high-frequency Linux commands with core parameters (1:1 compatible with Linux behavior)
- Combined main program `winuxcmd.exe` size < 1MB
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

### Pipeline-Based Architecture

The project uses a **pipeline-based architecture** that separates command processing into distinct stages, providing better modularity and testability:

- **Pipeline components**: Commands are implemented as a series of pipeline components
- **CommandContext**: Provides type-safe access to command options and arguments
- **Declarative option definitions**: Uses constexpr arrays to define command options

```cpp
// Example pipeline-based architecture
namespace ls_pipeline {
  namespace cp = core::pipeline;
  
  // Validate arguments
  auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>>;
  
  // Main pipeline
  template<size_t N>
  auto process_command(const CommandContext<N>& ctx) -> cp::Result<std::vector<std::string>>;
}
```

### Command Dispatcher

A central command dispatcher manages command registration and execution:

- **Automatic registration**: Commands register themselves using the `REGISTER_COMMAND` macro
- **Dual-mode execution**: Supports both `winuxcmd <command>` and `<command>.exe` formats
- **Efficient dispatching**: Uses a hash map for fast command lookup

```cpp
// Command registration example
REGISTER_COMMAND(ls,
                 /* name */ "ls",
                 /* synopsis */ "list directory contents",
                 /* description */ "List information about the FILEs",
                 /* examples */ "  ls -la",
                 /* see_also */ "cp, mv, rm",
                 /* author */ "WinuxCmd Team",
                 /* copyright */ "Copyright © 2026 WinuxCmd",
                 /* options */ LS_OPTIONS
) {
  // Command implementation
}

// Command execution example
int exit_code = CommandRegistry::dispatch("ls", args);
```

### CommandContext

The `CommandContext` class provides a type-safe way to access command options and arguments:

- **Type-safe option access**: Uses `ctx.get<bool>("--verbose", false)` to access options
- **Positionals access**: Provides access to positional arguments via `ctx.positionals`
- **Automatic option parsing**: Handles option parsing and validation automatically

### Pipeline Components

Each command is structured with pipeline components:

1. **Validation**: Validates command arguments and options
2. **Processing**: Contains the main command logic
3. **Output**: Handles command output

This structure improves code organization, testability, and maintainability.

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

1. **Use the scaffold tool** to generate a command template
2. **Update the command options** in the generated file
3. **Implement the pipeline components** for validation and processing
4. **Add command logic** in the registered command function
5. **Build the project** to include the new command

Example for a new command `echo`:

```cpp
// src/commands/echo.cppm
export module cmd:echo;

import std;
import core;
import utils;
namespace fs = std::filesystem;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

// ======================================================
// Constants
// ======================================================
namespace echo_constants {
  // Add constants here
}

// ======================================================
// Options (constexpr)
// ======================================================

export auto constexpr ECHO_OPTIONS =
    std::array{OPTION("-n", "--no-newline", "do not output the trailing newline"),
               // Add more options here
              };

// ======================================================
// Pipeline components
// ======================================================
namespace echo_pipeline {
   namespace cp=core::pipeline;
  
  // ----------------------------------------------
  // 1. Validate arguments
  // ----------------------------------------------
  auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
    std::vector<std::string> texts;
    for (auto arg : args) {
      texts.push_back(std::string(arg));
    }
    return texts;
  }

  // ----------------------------------------------
  // 2. Main pipeline
  // ----------------------------------------------
  template<size_t N>
  auto process_command(const CommandContext<N>& ctx)
      -> cp::Result<std::vector<std::string>>
  {
    return validate_arguments(ctx.positionals)
        // Add more pipeline steps here
        ;
  }

}

// ======================================================
// Command registration
// ======================================================

REGISTER_COMMAND(echo,
                 /* name */
                 "echo",

                 /* synopsis */
                 "echo [OPTION]... [STRING]...",

                 /* description */
                 "Display a line of text. With no arguments, outputs a blank line.",

                 /* examples */
                 "  echo Hello World        Display 'Hello World'\n"
                 "  echo -n Hello           Display 'Hello' without trailing newline",

                 /* see_also */
                 "printf",

                 /* author */
                 "WinuxCmd Team",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 ECHO_OPTIONS
) {
  using namespace echo_pipeline;

  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"echo");
    return 1;
  }

  auto texts = *result;

  // Command logic
  for (size_t i = 0; i < texts.size(); ++i) {
    safePrint(utf8_to_wstring(texts[i]));
    if (i != texts.size() - 1) {
      safePrint(L" ");
    }
  }

  if (!ctx.get<bool>("--no-newline", false)) {
    safePrintLn(L"");
  }

  return 0;
}
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
    ├── TODO_zh.md    # Project TODO list (Chinese)
    ├── scaffold_and_dsl_en.md  # Scaffold and DSL design documentation (English)
```

## Build Instructions

### Prerequisites

- Windows 10/11 (64-bit)
- Microsoft Visual Studio 2022+ (with C++23 support)
- CMake 3.30+ (supports C++23 modules)

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

### Direct Usage (No Activation Required)

```bash
# Use winux prefix for direct command access
winux ls -la
winux grep "pattern" file.txt
winux cp source.txt dest.txt
```

### With Activation

```bash
# Activate for direct command access
winux activate

# Now use commands directly
ls -la
grep "pattern" file.txt
cp source.txt dest.txt

# Deactivate when done
winux deactivate
```

### Combined Mode

```bash
# Use main executable directly
winuxcmd ls -lh
winuxcmd grep "pattern" file.txt
```

## Versioning

Semantic Versioning (e.g., v1.0.0), `<major>.<minor>.<patch>`

## License

MIT License

## Contributing

Please read Project_Rules.md for details on our code of conduct and the process for submitting pull requests.

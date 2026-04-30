# Overview

CLI11 is a header-only C++11 command line parser that provides a modern, intuitive API for parsing command line arguments. It supports options, flags, positional arguments, subcommands, validators, and configuration files (TOML/INI format). The library is designed to be easy to include (single header file available), has no external dependencies, and works on Linux, macOS, and Windows.

**When to use CLI11:**
- You need a simple, modern C++ command line parser
- Your project requires subcommands (like git)
- You want automatic help generation and error messages
- You need validation of input values (range checks, type validation)
- You want configuration file support

**When NOT to use CLI11:**
- You need to parse extremely complex command line syntax (use Boost.ProgramOptions)
- You're limited to C++98 (CLI11 requires C++11)
- You need a minimal parser with no features (use getopt directly)

**Key design principles:**
- Fluent interface: options are configured through method chaining
- Type-safe: values are directly bound to C++ variables
- Header-only: no compilation step needed
- Extensible: custom validators, formatters, and type converters
# Overview

`cxxopts` is a lightweight, header-only C++ option parser library that follows GNU-style syntax for command-line argument parsing. It provides a clean, modern C++ interface for defining and parsing command-line options without external dependencies beyond the C++ standard library.

**When to use cxxopts:**
- You need simple, GNU-style command-line parsing (e.g., `--option`, `-o`, `--option=value`)
- You want a header-only library with no external dependencies
- You need basic type parsing (int, string, bool, etc.)
- You want positional argument support alongside named options
- You need help text generation

**When NOT to use cxxopts:**
- You need complex subcommand parsing (consider CLI11 or Boost.ProgramOptions)
- You require advanced argument validation or custom parsing rules
- You need Windows-style `/option` syntax
- You're in a performance-critical path where parsing overhead matters
- You need thread-safe parsing (cxxopts is not thread-safe)

**Key design features:**
- Header-only: just `#include <cxxopts.hpp>`
- GNU-style syntax: `--long`, `-s`, `--key=value`
- Type-safe with `cxxopts::value<T>()`
- Default and implicit values
- Positional argument support
- Help text generation with grouping
- Unrecognized option handling
- Exception-based error handling
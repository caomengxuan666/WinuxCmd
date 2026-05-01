# Overview

Loguru is a lightweight, single-header C++ logging library designed for simplicity and performance. It provides printf-style and stream-style logging with verbosity levels, assertions, stack traces, and scope-based indentation. The library is contained in just two files (`loguru.hpp` and `loguru.cpp`) with no external dependencies, making it ideal for projects that need a minimal logging solution.

**When to use Loguru:**
- You need a simple, fast logging library with minimal compile-time overhead
- You want printf-style or stream-style logging with compile-time format checking
- You need thread-safe logging with file output and stderr support
- You want automatic stack traces on crashes and signal handling
- You need scope-based log indentation for readability
- You want error context capture for debugging crashes

**When NOT to use Loguru:**
- You need a full-featured logging framework with log rotation, compression, or network logging
- You require structured logging (JSON, XML) out of the box
- You need asynchronous logging with guaranteed delivery
- You want a logging library with extensive configuration options
- You need support for custom log levels beyond verbosity integers

**Key design principles:**
- **Minimal header includes**: `loguru.hpp` has zero `#include` directives, keeping compile times fast
- **Performance**: Optimized for speed, often 25-75% faster than GLOG
- **Thread safety**: All logging operations are thread-safe by default
- **Flexibility**: Supports callbacks for logging and fatal errors, multiple file outputs, and custom formatting
- **Grepability**: Log format is designed for easy filtering with tools like `grep`
- **Scope awareness**: Automatic indentation for nested scopes improves log readability
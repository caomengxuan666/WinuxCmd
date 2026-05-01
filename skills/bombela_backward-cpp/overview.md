# Overview

Backward-cpp is a header-only C++ library that provides beautiful, detailed stack trace printing for debugging and error reporting. It captures the current call stack and resolves symbol names, source file locations, and line numbers, presenting them in a human-readable format with color coding and code snippets.

**When to use:**
- Debugging crashes (segfaults, aborts) in development builds
- Adding detailed error reporting to logging systems
- Understanding complex call chains during development
- Creating diagnostic tools that need stack traces

**When NOT to use:**
- In production builds where performance is critical (stack trace capture is expensive)
- On embedded systems with limited memory (stack traces can be large)
- When you need cross-platform portability (Linux/macOS/Windows support varies)
- In signal handlers that must be async-signal-safe (Backward's printer is not)

**Key design:**
- `backward::StackTrace` captures raw stack frames
- `backward::TraceResolver` resolves symbols and source locations
- `backward::Printer` formats and outputs the trace
- `backward::SignalHandling` registers crash handlers automatically
- Supports multiple backends: libbfd, libdw, libdwarf, and basic backtrace_symbols
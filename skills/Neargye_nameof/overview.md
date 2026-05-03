# Overview

`Neargye/nameof` is a header-only C++17 library that provides compile-time string representations of names for variables, types, functions, enums, and macros. It uses macros and template metaprogramming to extract names without runtime overhead.

**When to use:**
- Logging and debugging to print variable/function names
- Serialization frameworks needing field names
- Enum-to-string conversion
- Reflection-like functionality in template code
- Error messages that include type names

**When NOT to use:**
- When you need runtime-dynamic names (e.g., from user input)
- In performance-critical hot paths where string operations matter
- With compilers that don't support C++17 fully
- When binary size is extremely constrained (macros expand to string literals)

**Key design:**
- All name extraction happens at compile time via `__PRETTY_FUNCTION__` or `__FUNCSIG__` parsing
- Enum names use a lookup table generated at compile time within a configurable range
- Type names are extracted from template function signatures
- No RTTI required (except `NAMEOF_TYPE_RTTI` variants)
- Zero runtime allocation for basic operations
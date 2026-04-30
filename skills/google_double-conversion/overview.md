# Overview

The double-conversion library provides efficient, accurate binary-decimal and decimal-binary conversion routines for IEEE 754 double-precision floating-point numbers. Extracted from the V8 JavaScript engine, it offers high-performance conversion with full control over formatting options.

**When to use:**
- You need fast, accurate double-to-string or string-to-double conversions
- You require consistent formatting across platforms (avoiding sprintf/iostream inconsistencies)
- You're working with performance-critical code that does many conversions
- You need to handle special values (infinity, NaN) explicitly

**When not to use:**
- For simple, occasional conversions where standard library functions suffice
- When you need arbitrary-precision decimal arithmetic (use a decimal library instead)
- In environments where you can't add external dependencies

**Key design:**
- Header-only interface with separate implementation
- Configurable flags for formatting control (UNIQUE_ZERO, EMIT_POSITIVE_EXPONENT_SIGN, etc.)
- Buffer-based output to avoid dynamic allocation
- Support for shortest representation, fixed precision, and exponential notation
- Handles all IEEE 754 special values (infinity, NaN, negative zero)
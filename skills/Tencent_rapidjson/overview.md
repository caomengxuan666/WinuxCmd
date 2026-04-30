# Overview

RapidJSON is a header-only C++ library for parsing and generating JSON data. It provides both DOM (Document Object Model) and SAX (Simple API for XML) style APIs, making it suitable for different use cases. The library is designed for high performance, with parsing speeds comparable to `strlen()`, and optionally supports SSE2/SSE4.2 acceleration.

**When to use RapidJSON:**
- You need fast JSON parsing/generation in C++
- You want a header-only library with no external dependencies
- You need Unicode support (UTF-8, UTF-16, UTF-32)
- You require both DOM and SAX parsing capabilities
- You need JSON Schema validation

**When NOT to use RapidJSON:**
- You need streaming JSON parsing for extremely large files (consider SAX API instead)
- You require strict IEEE 754 floating-point precision (RapidJSON may lose precision for some values)
- You need thread-safe document modification (documents are not thread-safe for writes)

**Key design features:**
- Header-only: just copy `include/rapidjson` to your project
- Memory-efficient: each JSON value occupies only 16 bytes on 64-bit systems
- Self-contained: no dependency on STL or Boost
- Unicode-friendly: automatic encoding detection and transcoding
- Supports JSON Pointer and JSON Schema (v1.1+)
# Overview

`nlohmann/json` (JSON for Modern C++) is a header-only C++11 library for parsing, manipulating, and serializing JSON data. It provides an intuitive syntax that makes JSON feel like a first-class data type in C++, similar to Python's json module.

**When to use:**
- You need simple JSON parsing/serialization in C++ projects
- You want a single-header, dependency-free solution
- You need STL-like access patterns (iterators, range-based for loops)
- You're working with JSON configuration files, REST APIs, or data exchange

**When NOT to use:**
- Maximum performance is critical (use simdjson or rapidjson instead)
- Memory-constrained environments (each JSON value has overhead)
- You need streaming/SAX-style parsing for huge files (though SAX interface is available)
- You need strict schema validation (use a dedicated JSON schema validator)

**Key design:**
- Single header file (`json.hpp`) - no build system needed
- Uses C++11 features extensively (variadic templates, move semantics)
- Provides `json::parse()` for deserialization and `dump()` for serialization
- Supports JSON Pointer, JSON Patch, and JSON Merge Patch
- Can convert to/from STL containers automatically
- Supports binary formats: CBOR, MessagePack, UBJSON, BSON, BJData
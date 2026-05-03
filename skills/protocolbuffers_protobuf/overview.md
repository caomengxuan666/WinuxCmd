# Overview

Protocol Buffers (protobuf) is Google's language-neutral, platform-neutral serialization framework for structured data. It uses a schema definition language (.proto files) to define data structures, then generates code in multiple languages for efficient serialization/deserialization.

**When to use protobuf:**
- High-performance data serialization between services
- Cross-language data exchange (C++, Java, Python, Go, etc.)
- Schema evolution with backward/forward compatibility
- Binary format for storage or network transmission
- When you need smaller payloads than JSON/XML

**When NOT to use protobuf:**
- Human-readable data (use JSON/XML instead)
- Very small datasets where overhead matters
- When schema management is impractical
- Browser-based applications without gRPC-Web

**Key design principles:**
- Schema-first approach with .proto files
- Field numbers for binary encoding (not field names)
- Backward/forward compatibility through field numbering
- Generated code provides type-safe accessors
- Zero-copy deserialization in some implementations

```cpp
// Example .proto file
syntax = "proto3";

message Person {
  string name = 1;
  int32 id = 2;
  string email = 3;
}
```
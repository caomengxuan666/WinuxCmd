# Overview

msgpack-c is a C++ implementation of MessagePack, a binary serialization format that is more compact than JSON and faster to parse. It supports automatic serialization of C++ types including primitives, containers, and custom classes via the `MSGPACK_DEFINE` macro.

**When to use:**
- Need efficient binary serialization for network protocols or storage
- Working with systems that use MessagePack format (e.g., Redis, some RPC frameworks)
- Need cross-language serialization (MessagePack has implementations in many languages)
- Want smaller payloads compared to JSON or XML

**When NOT to use:**
- Human-readable format is required (use JSON instead)
- Need schema validation or versioning (consider Protocol Buffers or Cap'n Proto)
- Working with very large messages (>2GB) without streaming support
- Need zero-copy deserialization for complex nested structures

**Key design:**
- Header-only library (no compilation needed for basic use)
- Uses `msgpack::pack()` for serialization and `msgpack::unpack()` for deserialization
- Supports streaming via `msgpack::unpacker` for incremental parsing
- Object model uses `msgpack::object` which can reference original buffer data (zero-copy) or own its data via `msgpack::zone`
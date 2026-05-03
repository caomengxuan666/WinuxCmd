# Overview

simdjson is a high-performance C++ library for parsing JSON that leverages SIMD (Single Instruction, Multiple Data) instructions to achieve parsing speeds of gigabytes per second. It is designed to be the fastest JSON parser available, often 4x faster than RapidJSON and 25x faster than JSON for Modern C++. The library provides full JSON and UTF-8 validation, lossless parsing, and automatic CPU-tailored parser selection at runtime.

**When to use simdjson:**
- You need to parse large JSON files or streams at maximum speed
- You are building high-throughput data processing pipelines
- You need strict JSON validation with no compromises
- You want a simple, modern C++ API with on-demand parsing

**When NOT to use simdjson:**
- You are on a 32-bit system (only 64-bit is supported)
- You need to modify JSON in-place (simdjson is read-only)
- You are working with extremely small JSON snippets where overhead matters
- You cannot use C++17 or later

**Key design principles:**
- On-demand parsing: JSON is parsed lazily as you access elements, not all at once
- Padded strings: All input must be padded with SIMDJSON_PADDING (64 bytes) for SIMD safety
- Zero-copy access: String values are returned as string_view pointing into the original buffer
- Error handling via simdjson_result: Functions return results that can be checked for errors
- Automatic implementation selection: The library picks the best parser for your CPU at runtime
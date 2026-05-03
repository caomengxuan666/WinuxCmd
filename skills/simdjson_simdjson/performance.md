# Performance

**Performance Characteristics**

simdjson achieves its speed through several key techniques:

```cpp
// 1. SIMD instructions process 64 bytes at a time
// 2. On-demand parsing avoids processing unused fields
// 3. Zero-copy string access eliminates allocations
// 4. Automatic CPU dispatch selects optimal implementation

// Example: Selective access is faster than full parsing
void fast_selective_access() {
    padded_string json = padded_string::load("large.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    
    // Only parse the fields we need
    auto name = doc["name"].get_string(); // Fast: skips other fields
    auto count = doc["count"].get_uint64(); // Fast: already positioned
}
```

**Allocation Patterns**

```cpp
// Minimize allocations by reusing buffers
void efficient_loading() {
    // Pre-allocate buffer for reuse
    std::vector<char> buffer;
    buffer.reserve(1024 * 1024); // 1MB
    
    for (int i = 0; i < 1000; ++i) {
        // Load into pre-allocated buffer
        std::ifstream file("data_" + std::to_string(i) + ".json");
        buffer.assign(std::istreambuf_iterator<char>(file), {});
        
        // Create padded_string from existing buffer
        padded_string json(buffer.data(), buffer.size());
        
        ondemand::parser parser;
        auto doc = parser.iterate(json);
        process(doc);
    }
}
```

**Optimization Tips**

```cpp
// 1. Use iterate_many for NDJSON files (faster than individual parses)
void fast_ndjson() {
    padded_string json = padded_string::load("lines.json");
    ondemand::parser parser;
    
    // iterate_many is optimized for batch processing
    auto stream = parser.iterate_many(json);
    for (auto doc : stream) {
        process(doc.value());
    }
}

// 2. Access fields in order for better cache performance
void ordered_access() {
    padded_string json = padded_string::load("data.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    
    // Access fields in document order
    auto first = doc["first"].get_string(); // Sequential access
    auto second = doc["second"].get_uint64(); // Fast
    auto third = doc["third"].get_bool(); // Fast
}

// 3. Use get_object() for explicit object access
void explicit_object() {
    padded_string json = padded_string::load("data.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    
    // Explicit object access can be faster
    ondemand::object obj = doc.get_object();
    for (auto field : obj) {
        auto key = field.key;
        auto value = field.value;
        process(key, value);
    }
}
```

**Memory Considerations**

```cpp
// padded_string allocates extra padding bytes
// For a 1MB file, expect ~1MB + 64 bytes overhead

// Use minify to reduce memory footprint
void reduce_memory() {
    padded_string json = padded_string::load("large.json");
    
    // Minify can reduce size by 30-50%
    std::string minified;
    simdjson::minify(json.data(), json.size(), minified);
    
    // Parse the minified version
    padded_string minified_padded(minified);
    ondemand::parser parser;
    auto doc = parser.iterate(minified_padded);
}
```
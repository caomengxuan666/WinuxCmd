# Performance

```cpp
// Performance characteristics and optimization
#include <msgpack.hpp>
#include <chrono>
#include <vector>

// Performance Pattern 1: Pre-allocate buffer for repeated serialization
void preallocated_buffer() {
    msgpack::sbuffer buffer;
    buffer.reserve(1024 * 1024); // Pre-allocate 1MB
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        buffer.clear(); // Reuse buffer, no reallocation
        msgpack::pack(buffer, i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    // Much faster than creating new sbuffer each time
}

// Performance Pattern 2: Use raw_ref for binary data (zero-copy)
void zero_copy_binary() {
    std::vector<char> large_data(1024 * 1024);
    
    // BAD: Copies data
    msgpack::sbuffer buffer1;
    msgpack::pack(buffer1, large_data);
    
    // GOOD: Zero-copy reference
    msgpack::sbuffer buffer2;
    msgpack::type::raw_ref ref(large_data.data(), large_data.size());
    msgpack::pack(buffer2, ref);
    // No copy of large_data, just stores pointer and size
}

// Performance Pattern 3: Batch serialization
void batch_serialization() {
    std::vector<int> values(100000);
    std::iota(values.begin(), values.end(), 0);
    
    // BAD: Serialize each element individually
    msgpack::sbuffer buffer1;
    for (int v : values) {
        msgpack::pack(buffer1, v);
    }
    
    // GOOD: Serialize entire vector at once
    msgpack::sbuffer buffer2;
    msgpack::pack(buffer2, values);
    // More compact and faster
}

// Performance Pattern 4: Streaming deserialization for large data
void streaming_deserialization() {
    msgpack::sbuffer buffer;
    for (int i = 0; i < 100000; ++i) {
        msgpack::pack(buffer, i);
    }
    
    // BAD: Unpack entire buffer at once
    auto start = std::chrono::high_resolution_clock::now();
    auto oh = msgpack::unpack(buffer.data(), buffer.size());
    // Creates full object tree in memory
    
    // GOOD: Stream process
    msgpack::unpacker pac;
    pac.reserve_buffer(buffer.size());
    memcpy(pac.buffer(), buffer.data(), buffer.size());
    pac.buffer_consumed(buffer.size());
    
    msgpack::unpacked result;
    while (pac.next(result)) {
        int value = result.get().as<int>();
        // Process each value immediately
    }
    // Lower peak memory usage
}

// Performance Pattern 5: Avoid unnecessary type conversions
void avoid_conversions() {
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, 42);
    
    auto oh = msgpack::unpack(buffer.data(), buffer.size());
    msgpack::object obj = oh.get();
    
    // BAD: Multiple type conversions
    // int64_t val1 = obj.as<int64_t>();
    // uint64_t val2 = obj.as<uint64_t>();
    
    // GOOD: Direct conversion to expected type
    int value = obj.as<int>(); // Single conversion
}

// Performance Pattern 6: Use object directly instead of converting
void direct_object_usage() {
    msgpack::sbuffer buffer;
    std::map<std::string, int> data = {{"a", 1}, {"b", 2}};
    msgpack::pack(buffer, data);
    
    auto oh = msgpack::unpack(buffer.data(), buffer.size());
    msgpack::object obj = oh.get();
    
    // BAD: Convert to map first, then iterate
    auto map = obj.as<std::map<std::string, int>>();
    for (auto& [k, v] : map) { /* ... */ }
    
    // GOOD: Iterate object directly
    if (obj.type == msgpack::type::MAP) {
        for (uint32_t i = 0; i < obj.via.map.size; ++i) {
            auto& kv = obj.via.map.ptr[i];
            std::string key = kv.key.as<std::string>();
            int value = kv.val.as<int>();
        }
    }
}

// Performance Pattern 7: Measure and optimize
void benchmark_serialization() {
    auto start = std::chrono::high_resolution_clock::now();
    
    msgpack::sbuffer buffer;
    for (int i = 0; i < 100000; ++i) {
        buffer.clear();
        msgpack::pack(buffer, i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Use results to guide optimization decisions
    std::cout << "Serialization time: " << duration.count() << " μs" << std::endl;
}
```
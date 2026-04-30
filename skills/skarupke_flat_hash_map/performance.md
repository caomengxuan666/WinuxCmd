# Performance

### Memory Layout and Cache Performance
```cpp
// flat_hash_map stores all data contiguously
ska::flat_hash_map<int, int> map;
map.reserve(1000000);

// Iteration is cache-friendly
for (const auto& [key, value] : map) {
    // Sequential memory access, good cache utilization
    process(key, value);
}

// Compare with std::unordered_map (linked list)
std::unordered_map<int, int> umap;
// Iteration jumps through memory, poor cache performance
```

### Allocation Patterns
```cpp
// Single allocation for all data
ska::flat_hash_map<int, int> map;
map.reserve(1000);  // One allocation for 1000 elements

// Rehashing causes reallocation
for (int i = 0; i < 10000; ++i) {
    map[i] = i;  // Multiple rehashes, each reallocates
}

// Optimal: Reserve exact size
map.reserve(10000);  // Single allocation, no rehashes
for (int i = 0; i < 10000; ++i) {
    map[i] = i;  // No allocations during insertion
}
```

### Performance Optimization Tips
```cpp
// 1. Use reserve() for known sizes
ska::flat_hash_map<int, int> map;
map.reserve(1000000);  // Prevents rehashes

// 2. Use try_emplace() to avoid unnecessary copies
ska::flat_hash_map<int, std::string> map;
map.try_emplace(1, "very_long_string");  // No copy if key exists

// 3. Use heterogeneous lookup to avoid temporary objects
ska::flat_hash_map<std::string, int, ska::transparent_hash> map;
map.find(std::string_view("key"));  // No std::string allocation

// 4. Batch operations for better performance
std::vector<std::pair<int, int>> batch;
for (int i = 0; i < 1000; ++i) {
    batch.emplace_back(i, i * 2);
}
map.insert(batch.begin(), batch.end());  // Single rehash if needed
```

### Benchmarking Considerations
```cpp
// Measure real-world performance, not microbenchmarks
void benchmark() {
    ska::flat_hash_map<int, int> map;
    std::unordered_map<int, int> umap;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; ++i) {
        map[i] = i;
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // flat_hash_map typically 2-5x faster for insertion
    // and 3-10x faster for iteration
}
```
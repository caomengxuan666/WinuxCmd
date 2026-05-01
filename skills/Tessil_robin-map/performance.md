# Performance

**Performance characteristics**
```cpp
// Robin hood hashing provides O(1) average case operations
// Worst case O(n) with poor hash functions

// Fast insertion and lookup
tsl::robin_map<int, int> map;
map.reserve(1000000);

// Benchmark: 1 million insertions
auto start = std::chrono::high_resolution_clock::now();
for (int i = 0; i < 1000000; ++i) {
    map[i] = i;
}
auto end = std::chrono::high_resolution_clock::now();
std::cout << "Insertion time: " 
          << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
          << "ms\n";
```

**Allocation patterns**
```cpp
// Pre-allocation reduces rehash overhead
tsl::robin_map<int, int> map;

// BAD: Multiple rehashes
for (int i = 0; i < 10000; ++i) {
    map[i] = i;  // May trigger multiple rehashes
}

// GOOD: Single allocation
tsl::robin_map<int, int> map2;
map2.reserve(10000);
for (int i = 0; i < 10000; ++i) {
    map2[i] = i;  // No rehashes
}

// Memory overhead: ~2-3x the size of stored elements
// Each bucket stores: key, value, metadata (1 byte), optional hash
```

**Optimization tips**
```cpp
// 1. Use pre-calculated hash for repeated lookups
tsl::robin_map<int, std::string> map = {{1, "one"}};
std::size_t hash = map.hash_function()(1);
for (int i = 0; i < 1000; ++i) {
    auto it = map.find(1, hash);  // 20-30% faster than without hash
}

// 2. Use StoreHash for expensive hash functions
struct ExpensiveHash {
    std::size_t operator()(const std::string& s) const {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        return std::hash<std::string>{}(s);
    }
};
tsl::robin_map<std::string, int, ExpensiveHash, std::equal_to<std::string>,
               std::true_type> map;  // StoreHash = true for faster rehash

// 3. Use emplace for complex types
tsl::robin_map<int, std::array<int, 100>> map;
map.emplace(1);  // Default constructs value in-place, no copy

// 4. Batch operations for better cache behavior
std::vector<std::pair<int, int>> batch;
for (int i = 0; i < 1000; ++i) {
    batch.emplace_back(i, i * 2);
}
tsl::robin_map<int, int> map;
map.reserve(batch.size());
for (const auto& [key, value] : batch) {
    map.insert({key, value});  // Sequential insertion is cache-friendly
}
```

**Memory usage considerations**
```cpp
// Memory layout: open addressing means no separate chaining
// Each bucket stores key-value pair + metadata byte
// Load factor typically 0.5-0.7 before rehash

// Example: 1 million int-int pairs
// Each pair: 8 bytes (2 ints) + 1 byte metadata = 9 bytes
// With load factor 0.5: ~18 MB for 1 million elements
tsl::robin_map<int, int> map;
map.reserve(2000000);  // Reserve 2x for load factor 0.5
for (int i = 0; i < 1000000; ++i) {
    map[i] = i;
}
std::cout << "Memory: " << map.capacity() * sizeof(std::pair<int, int>) / 1024 / 1024 << " MB\n";
```
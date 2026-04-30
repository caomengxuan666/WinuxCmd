# Performance


**Performance characteristics**

The Parallel Hashmap uses closed hashing with SSE2/AVX2 instructions to check 16 slots in parallel, enabling high load factors (up to 87.5%) without performance degradation. Lookup operations are O(1) average case, with insertion being amortized O(1). The flat variants provide better cache locality than node-based alternatives.

```cpp
#include <parallel_hashmap/phmap.h>
#include <chrono>
#include <iostream>

void benchmark_lookup() {
    phmap::flat_hash_map<int, int> map;
    constexpr int SIZE = 1000000;
    
    // Pre-allocate to avoid rehashing during benchmark
    map.reserve(SIZE);
    for (int i = 0; i < SIZE; ++i) {
        map[i] = i;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    volatile int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        sum += map[i];  // Fast lookup due to SSE2 optimization
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Lookup time: " << duration.count() << "ms\n";
}
```

**Allocation patterns and optimization**

```cpp
#include <parallel_hashmap/phmap.h>

void allocation_optimization() {
    // BAD: Multiple reallocations
    phmap::flat_hash_map<int, int> bad;
    for (int i = 0; i < 100000; ++i) {
        bad[i] = i;  // Multiple rehashes as map grows
    }
    
    // GOOD: Single allocation with reserve
    phmap::flat_hash_map<int, int> good;
    good.reserve(100000);  // Single allocation
    for (int i = 0; i < 100000; ++i) {
        good[i] = i;  // No rehashes
    }
    
    // BEST: Use max_load_factor to control memory/performance trade-off
    phmap::flat_hash_map<int, int> best;
    best.max_load_factor(0.75);  // Lower = more memory, faster lookups
    best.reserve(100000);
    for (int i = 0; i < 100000; ++i) {
        best[i] = i;
    }
}
```

**Choosing between flat and node variants**

```cpp
#include <parallel_hashmap/phmap.h>
#include <iostream>

void variant_comparison() {
    constexpr int SIZE = 100000;
    
    // flat_hash_map: Better cache locality, faster iteration
    phmap::flat_hash_map<int, int> flat;
    flat.reserve(SIZE);
    for (int i = 0; i < SIZE; ++i) flat[i] = i;
    
    // node_hash_map: Pointer stability, slightly slower
    phmap::node_hash_map<int, int> node;
    node.reserve(SIZE);
    for (int i = 0; i < SIZE; ++i) node[i] = i;
    
    // Iteration over flat is faster due to contiguous memory
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& [k, v] : flat) { volatile int x = v; }
    auto flat_time = std::chrono::high_resolution_clock::now();
    
    for (const auto& [k, v] : node) { volatile int x = v; }
    auto node_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Flat iteration faster by factor: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(node_time - start).count() /
                 std::chrono::duration_cast<std::chrono::microseconds>(flat_time - start).count()
              << "\n";
}
```

**Serialization performance for trivially copyable types**

```cpp
#include <parallel_hashmap/phmap.h>
#include <fstream>
#include <chrono>

void serialization_performance() {
    phmap::flat_hash_map<int, double> data;
    data.reserve(1000000);
    for (int i = 0; i < 1000000; ++i) {
        data[i] = i * 1.5;
    }
    
    // Fast bulk serialization (10x faster than element-wise)
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::ofstream file("data.bin", std::ios::binary);
        data.serialize(file);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Serialization time: " << duration.count() << "ms\n";
}
```
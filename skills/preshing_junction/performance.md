# Performance

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>
#include <chrono>
#include <vector>

// Performance characteristics - Read-heavy workloads
void readHeavyWorkload() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    // Pre-populate
    for (uintptr_t i = 0; i < 100000; ++i) {
        map.assign(i, i * 10);
    }
    
    // Read-heavy: ~10x faster than write-heavy
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000000; ++i) {
        volatile uintptr_t val = map.get(i % 100000);
    }
    auto end = std::chrono::high_resolution_clock::now();
}
```

```cpp
// Performance optimization - Batch operations
void batchOptimization() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    // BAD: Individual operations
    auto start = std::chrono::high_resolution_clock::now();
    for (uintptr_t i = 0; i < 10000; ++i) {
        map.assign(i, i * 10);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // GOOD: Batch operations with Mutator
    start = std::chrono::high_resolution_clock::now();
    auto mutator = map.makeMutator();
    for (uintptr_t i = 0; i < 10000; ++i) {
        mutator.assign(i, i * 10);
    }
    end = std::chrono::high_resolution_clock::now();
    // Mutator is ~2-3x faster for batch operations
}
```

```cpp
// Performance optimization - Choose right map type
void chooseOptimalMap() {
    // Crude: Best for single-threaded (lowest overhead)
    junction::ConcurrentMap_Crude<uintptr_t, uintptr_t> crudeMap;
    
    // Leapfrog: Best all-around for multi-threaded
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> leapfrogMap;
    
    // Grampa: Best for write-heavy multi-threaded
    junction::ConcurrentMap_Grampa<uintptr_t, uintptr_t> grampaMap;
    
    // Performance comparison (relative):
    // Crude: 1x (baseline)
    // Linear: 0.8x
    // Leapfrog: 0.7x
    // Grampa: 0.5x
}
```

```cpp
// Performance optimization - Memory allocation patterns
void memoryOptimization() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    
    // Pre-allocate capacity to avoid rehashing
    Map map(1000000); // Reserve space for 1M entries
    
    // BAD: Frequent rehashing
    for (uintptr_t i = 0; i < 1000000; ++i) {
        map.assign(i, i * 10);
        // May trigger multiple rehashes
    }
    
    // GOOD: Pre-allocated capacity
    Map map2(1000000);
    for (uintptr_t i = 0; i < 1000000; ++i) {
        map2.assign(i, i * 10);
        // No rehashing needed
    }
}
```
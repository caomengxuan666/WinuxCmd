# Performance

### Grain size optimization
```cpp
// BAD: Too small grain size causes overhead
oneapi::tbb::parallel_for(0, 1000000, [](int i) {
    data[i] = data[i] * 2;  // Very cheap operation
});

// GOOD: Larger grain size reduces overhead
oneapi::tbb::parallel_for(
    oneapi::tbb::blocked_range<int>(0, 1000000, 10000),
    [](const oneapi::tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            data[i] = data[i] * 2;
        }
    }
);
```

### Cache-friendly access patterns
```cpp
// BAD: Non-contiguous memory access
std::vector<std::vector<int>> matrix(1000, std::vector<int>(1000));
oneapi::tbb::parallel_for(0, 1000, [&](int i) {
    for (int j = 0; j < 1000; ++j) {
        matrix[j][i] = i * j;  // Column-major access (cache misses)
    }
});

// GOOD: Contiguous memory access
oneapi::tbb::parallel_for(0, 1000, [&](int i) {
    for (int j = 0; j < 1000; ++j) {
        matrix[i][j] = i * j;  // Row-major access (cache friendly)
    }
});
```

### Memory allocation patterns
```cpp
// BAD: Frequent allocations in parallel loop
oneapi::tbb::parallel_for(0, 1000, [](int i) {
    std::vector<int> temp(1000);  // Allocated per iteration
    process(temp, i);
});

// GOOD: Pre-allocate and reuse
std::vector<std::vector<int>> temps(1000, std::vector<int>(1000));
oneapi::tbb::parallel_for(0, 1000, [&](int i) {
    process(temps[i], i);
});
```

### Using scalable allocator
```cpp
// BAD: Default allocator may cause contention
std::vector<int> data(1000000);
oneapi::tbb::parallel_for(0, 1000000, [&](int i) {
    data[i] = i;
});

// GOOD: Use TBB's scalable allocator
#include <oneapi/tbb/scalable_allocator.h>
std::vector<int, oneapi::tbb::scalable_allocator<int>> data(1000000);
oneapi::tbb::parallel_for(0, 1000000, [&](int i) {
    data[i] = i;
});
```

### Profiling and tuning
```cpp
// Use global_control to limit parallelism for testing
oneapi::tbb::global_control gc(
    oneapi::tbb::global_control::max_allowed_parallelism, 1
);

// Measure performance with different thread counts
for (int threads : {1, 2, 4, 8}) {
    oneapi::tbb::task_arena arena(threads);
    auto start = std::chrono::high_resolution_clock::now();
    arena.execute([] {
        oneapi::tbb::parallel_for(0, 1000000, [](int i) {
            process(i);
        });
    });
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Threads: " << threads 
              << " Time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}
```
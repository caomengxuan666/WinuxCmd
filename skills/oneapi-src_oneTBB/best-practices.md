# Best Practices

### Use grain size for better performance
```cpp
// BAD: Default grain size may be too small
oneapi::tbb::parallel_for(0, 1000000, [](int i) {
    process(i);
});

// GOOD: Specify grain size for better cache utilization
oneapi::tbb::parallel_for(
    oneapi::tbb::blocked_range<int>(0, 1000000, 1000),
    [](const oneapi::tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            process(i);
        }
    }
);
```

### Use parallel_reduce for associative operations
```cpp
// BAD: Atomic counter for reduction
std::atomic<int> sum{0};
oneapi::tbb::parallel_for(0, 1000, [&](int i) {
    sum += i;  // Atomic overhead
});

// GOOD: Use parallel_reduce
int sum = oneapi::tbb::parallel_reduce(
    oneapi::tbb::blocked_range<int>(0, 1000),
    0,
    [](const oneapi::tbb::blocked_range<int>& r, int init) {
        for (int i = r.begin(); i < r.end(); ++i)
            init += i;
        return init;
    },
    std::plus<int>()
);
```

### Use concurrent containers for shared state
```cpp
// BAD: Mutex-protected std::vector
std::vector<int> results;
std::mutex mtx;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    std::lock_guard<std::mutex> lock(mtx);
    results.push_back(i);
});

// GOOD: Use concurrent_vector
oneapi::tbb::concurrent_vector<int> results;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    results.push_back(i);
});
```

### Use task_arena for affinity control
```cpp
// BAD: No control over thread placement
oneapi::tbb::parallel_for(0, 100, [](int i) {
    process(i);
});

// GOOD: Use task_arena for NUMA-aware execution
oneapi::tbb::task_arena arena(4);  // 4 threads
arena.execute([&] {
    oneapi::tbb::parallel_for(0, 100, [](int i) {
        process(i);
    });
});
```

### Use flow graph for complex dependencies
```cpp
// BAD: Manual synchronization for pipeline
std::vector<int> data;
std::mutex mtx;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    int processed = process(i);
    std::lock_guard<std::mutex> lock(mtx);
    data.push_back(processed);
});

// GOOD: Use flow graph
oneapi::tbb::flow::graph g;
oneapi::tbb::flow::function_node<int, int> process_node(g, 1, [](int i) {
    return process(i);
});
oneapi::tbb::flow::function_node<int, int> collect_node(g, 1, [](int i) {
    return i;
});
oneapi::tbb::flow::make_edge(process_node, collect_node);
```
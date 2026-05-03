# Safety

### Condition 1: Never destroy a task_group before all tasks complete
```cpp
// BAD: Destructor throws
void unsafe() {
    oneapi::tbb::task_group tg;
    tg.run([] { do_work(); });
}  // Throws tbb::task_group::exception

// GOOD: Always wait
void safe() {
    oneapi::tbb::task_group tg;
    tg.run([] { do_work(); });
    tg.wait();
}
```

### Condition 2: Never access non-thread-safe containers from parallel algorithms
```cpp
// BAD: std::map is not thread-safe
std::map<int, int> results;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    results[i] = i * 2;  // Undefined behavior
});

// GOOD: Use concurrent_hash_map
oneapi::tbb::concurrent_hash_map<int, int> results;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    results.insert({i, i * 2});
});
```

### Condition 3: Never use parallel algorithms with non-copyable functors
```cpp
// BAD: unique_ptr cannot be copied
auto ptr = std::make_unique<int>(42);
oneapi::tbb::parallel_for(0, 10, [ptr = std::move(ptr)](int) {
    // Error: lambda is not copyable
});

// GOOD: Use shared_ptr or capture by reference
auto ptr = std::make_shared<int>(42);
oneapi::tbb::parallel_for(0, 10, [ptr](int) {
    // OK: shared_ptr is copyable
});
```

### Condition 4: Never call parallel algorithms from within a task without proper nesting
```cpp
// BAD: Nested parallel_for without task_arena
oneapi::tbb::parallel_for(0, 10, [](int) {
    oneapi::tbb::parallel_for(0, 10, [](int) {
        // May cause deadlock or oversubscription
    });
});

// GOOD: Use task_arena for nesting
oneapi::tbb::task_arena arena;
arena.execute([] {
    oneapi::tbb::parallel_for(0, 10, [](int) {
        oneapi::tbb::parallel_for(0, 10, [](int) {
            // Safe
        });
    });
});
```

### Condition 5: Never use parallel_sort with non-total-order comparisons
```cpp
// BAD: Weak ordering comparator
auto cmp = [](int a, int b) {
    return (a % 2) < (b % 2);  // Not a total order
};
std::vector<int> data = {1, 2, 3, 4};
oneapi::tbb::parallel_sort(data.begin(), data.end(), cmp);  // Undefined behavior

// GOOD: Use strict weak ordering
auto cmp = [](int a, int b) {
    return a < b;  // Total order
};
oneapi::tbb::parallel_sort(data.begin(), data.end(), cmp);
```
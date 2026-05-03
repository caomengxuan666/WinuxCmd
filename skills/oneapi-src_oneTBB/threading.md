# Threading

### Thread safety guarantees
```cpp
// concurrent_queue is fully thread-safe
oneapi::tbb::concurrent_queue<int> queue;

// Multiple producers
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    queue.push(i);
});

// Multiple consumers
oneapi::tbb::parallel_for(0, 100, [&](int) {
    int value;
    if (queue.try_pop(value)) {
        process(value);
    }
});

// concurrent_hash_map supports concurrent reads and writes
oneapi::tbb::concurrent_hash_map<int, std::string> map;

// Reader access
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    oneapi::tbb::concurrent_hash_map<int, std::string>::const_accessor acc;
    if (map.find(acc, i)) {
        process(acc->second);
    }
});

// Writer access
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    oneapi::tbb::concurrent_hash_map<int, std::string>::accessor acc;
    map.insert(acc, i);
    acc->second = "value_" + std::to_string(i);
});
```

### Concurrent access patterns
```cpp
// Safe: Multiple readers, single writer
oneapi::tbb::concurrent_vector<int> vec;
vec.push_back(1);  // Writer

oneapi::tbb::parallel_for(0, 100, [&](int i) {
    int val = vec[i];  // Reader (safe if index exists)
});

// Safe: Concurrent reads and writes to different elements
oneapi::tbb::concurrent_vector<int> vec2(100);
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    vec2[i] = i * 2;  // Each thread writes to different index
});
```

### Task-level thread safety
```cpp
// Tasks are isolated by default
oneapi::tbb::parallel_for(0, 100, [](int i) {
    // Each iteration has its own stack
    int local_var = i;
    process(local_var);
});

// Shared state requires synchronization
std::atomic<int> counter{0};
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    counter.fetch_add(1, std::memory_order_relaxed);
});
```

### Thread pool management
```cpp
// Default thread pool (one per process)
oneapi::tbb::task_arena default_arena;

// Custom thread pool with specific number of threads
oneapi::tbb::task_arena custom_arena(4);

// Threads are reused across tasks
custom_arena.execute([] {
    oneapi::tbb::parallel_for(0, 100, [](int i) {
        // Uses threads from custom_arena
        process(i);
    });
});

// Threads are terminated when arena is destroyed
{
    oneapi::tbb::task_arena temp_arena(2);
    temp_arena.execute([] {
        oneapi::tbb::parallel_for(0, 10, [](int i) {
            process(i);
        });
    });
}  // Threads terminated
```

### Exception safety in parallel contexts
```cpp
// Exceptions in parallel algorithms are propagated
try {
    oneapi::tbb::parallel_for(0, 100, [](int i) {
        if (i == 50) {
            throw std::runtime_error("Error at index 50");
        }
        process(i);
    });
} catch (const std::runtime_error& e) {
    std::cerr << "Caught: " << e.what() << std::endl;
}

// task_group exceptions are propagated on wait
oneapi::tbb::task_group tg;
tg.run([] {
    throw std::runtime_error("Task error");
});
try {
    tg.wait();
} catch (const std::runtime_error& e) {
    std::cerr << "Caught: " << e.what() << std::endl;
}
```
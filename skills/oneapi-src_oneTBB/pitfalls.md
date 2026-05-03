# Pitfalls

### Pitfall 1: Using raw threads instead of tasks
```cpp
// BAD: Manual thread management
std::vector<std::thread> threads;
for (int i = 0; i < 100; ++i) {
    threads.emplace_back([i] { process(i); });
}
for (auto& t : threads) t.join();

// GOOD: Use parallel_for
oneapi::tbb::parallel_for(0, 100, [](int i) {
    process(i);
});
```

### Pitfall 2: Ignoring thread safety in lambdas
```cpp
// BAD: Data race on shared counter
int counter = 0;
oneapi::tbb::parallel_for(0, 1000, [&](int) {
    ++counter;  // Data race!
});

// GOOD: Use parallel_reduce or atomic
std::atomic<int> counter{0};
oneapi::tbb::parallel_for(0, 1000, [&](int) {
    ++counter;
});
```

### Pitfall 3: Blocking in task_group without wait
```cpp
// BAD: Task group destroyed before tasks complete
{
    oneapi::tbb::task_group tg;
    tg.run([] { do_work(); });
}  // Destructor throws if tasks not done!

// GOOD: Always wait
{
    oneapi::tbb::task_group tg;
    tg.run([] { do_work(); });
    tg.wait();
}
```

### Pitfall 4: Using non-thread-safe containers
```cpp
// BAD: std::vector push_back from multiple threads
std::vector<int> results;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    results.push_back(i);  // Undefined behavior!
});

// GOOD: Use concurrent_vector
oneapi::tbb::concurrent_vector<int> results;
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    results.push_back(i);
});
```

### Pitfall 5: Nested parallelism without task_arena
```cpp
// BAD: Nested parallel_for may oversubscribe
oneapi::tbb::parallel_for(0, 10, [&](int i) {
    oneapi::tbb::parallel_for(0, 100, [&](int j) {
        process(i, j);
    });
});

// GOOD: Use task_arena for nested parallelism
oneapi::tbb::task_arena arena;
arena.execute([&] {
    oneapi::tbb::parallel_for(0, 10, [&](int i) {
        oneapi::tbb::parallel_for(0, 100, [&](int j) {
            process(i, j);
        });
    });
});
```

### Pitfall 6: Forgetting to link TBB libraries
```cpp
// BAD: Missing linker flags
// g++ -std=c++17 main.cpp -o program

// GOOD: Link with TBB
// g++ -std=c++17 main.cpp -o program -ltbb
```

### Pitfall 7: Using blocking operations in parallel algorithms
```cpp
// BAD: Blocking in parallel_for
oneapi::tbb::parallel_for(0, 100, [&](int i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Blocks worker thread
});

// GOOD: Use task_group for blocking operations
oneapi::tbb::task_group tg;
for (int i = 0; i < 100; ++i) {
    tg.run([i] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
}
tg.wait();
```
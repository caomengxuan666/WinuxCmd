# Threading

### CLI11 is NOT thread-safe for parsing
```cpp
// BAD: Parsing from multiple threads
std::thread t1([&]() { app.parse(argc1, argv1); });
std::thread t2([&]() { app.parse(argc2, argv2); });
// Undefined behavior - app is not thread-safe

// GOOD: Parse sequentially or use separate App instances
CLI::App app1, app2;
std::thread t1([&]() { app1.parse(argc1, argv1); });
std::thread t2([&]() { app2.parse(argc2, argv2); });
t1.join();
t2.join();
```

### Configuration is safe before parsing
```cpp
// Safe to configure from one thread before parsing
CLI::App app;
// Thread 1: Configure
app.add_option("--value", value);
app.add_flag("--verbose", verbose);

// Thread 2: Parse (must wait until configuration is complete)
// Use synchronization
std::mutex mtx;
std::lock_guard<std::mutex> lock(mtx);
app.parse(argc, argv);
```

### Reading results after parsing is safe
```cpp
// After parsing, reading bound variables is safe
CLI::App app;
int value;
app.add_option("--value", value);
app.parse(argc, argv);

// Multiple threads can read the result
std::thread reader([&]() {
    std::cout << "Value: " << value << "\n";  // Safe
});
reader.join();
```

### Use separate App instances for concurrent parsing
```cpp
// Thread-safe approach for multiple parse operations
void parseInThread(int argc, char** argv, int& result) {
    CLI::App app;  // Each thread gets its own App
    app.add_option("--value", result);
    app.parse(argc, argv);
}

std::vector<std::thread> threads;
std::vector<int> results(4);
for(int i = 0; i < 4; ++i) {
    threads.emplace_back(parseInThread, argc, argv, std::ref(results[i]));
}
for(auto& t : threads) t.join();
```
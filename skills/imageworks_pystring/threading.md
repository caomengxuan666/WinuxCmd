# Threading

**Thread safety guarantees**
```cpp
// Pystring functions are NOT thread-safe for shared mutable data
std::string shared = "hello";

// Thread 1
std::thread t1([&]() {
    std::string result = pystring::upper(shared); // Read
});

// Thread 2
std::thread t2([&]() {
    std::string result = pystring::lower(shared); // Read
});
// This is safe - both threads only read shared
```

**Safe concurrent access patterns**
```cpp
// Safe: Each thread works on its own copy
void process_chunk(const std::string& chunk) {
    std::vector<std::string> parts;
    pystring::split(chunk, parts, ",");
    // Process parts...
}

std::vector<std::string> chunks = {"a,b", "c,d", "e,f"};
std::vector<std::thread> threads;
for (const auto& chunk : chunks) {
    threads.emplace_back(process_chunk, chunk);
}
for (auto& t : threads) t.join();
```

**Using mutex for shared data**
```cpp
// Safe: Mutex protects shared string
std::mutex mtx;
std::string shared_result;

void process_and_accumulate(const std::string& input) {
    std::string processed = pystring::upper(input);
    std::lock_guard<std::mutex> lock(mtx);
    shared_result += processed;
}
```

**Thread-local storage for temporary results**
```cpp
// Efficient: Thread-local storage avoids contention
thread_local std::vector<std::string> local_parts;

void process_data(const std::string& data) {
    local_parts.clear();
    pystring::split(data, local_parts, ",");
    // Use local_parts...
}
```

**Atomic operations for counters**
```cpp
// Safe: Atomic counter for tracking
std::atomic<size_t> total_parts{0};

void count_parts(const std::string& data) {
    std::vector<std::string> parts;
    pystring::split(data, parts, ",");
    total_parts += parts.size();
}
```
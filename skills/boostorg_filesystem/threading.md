# Threading

**Thread safety: Boost.Filesystem operations are NOT thread-safe by default**
```cpp
// BAD: Concurrent modification of same path
// Thread 1
fs::remove("/tmp/test.txt");
// Thread 2 (concurrent)
fs::create_directory("/tmp/test.txt");

// GOOD: Use external synchronization
std::mutex fs_mutex;
// Thread 1
{
    std::lock_guard<std::mutex> lock(fs_mutex);
    fs::remove("/tmp/test.txt");
}
// Thread 2
{
    std::lock_guard<std::mutex> lock(fs_mutex);
    fs::create_directory("/tmp/test.txt");
}
```

**Path objects are safe to copy between threads**
```cpp
fs::path p = "/shared/path";
std::thread t([p]() { // p is copied, safe
    if (fs::exists(p)) {
        // process
    }
});
t.join();
```

**Directory iterators are NOT thread-safe**
```cpp
// BAD: Sharing iterator between threads
fs::directory_iterator it("/tmp");
std::thread t1([&it]() { ++it; }); // Race condition
std::thread t2([&it]() { ++it; }); // Race condition

// GOOD: Each thread gets its own iterator
std::thread t1([]() {
    fs::directory_iterator it("/tmp");
    ++it;
});
std::thread t2([]() {
    fs::directory_iterator it("/tmp");
    ++it;
});
```

**Use atomic operations for shared filesystem state**
```cpp
std::atomic<bool> file_created{false};

void create_file_safely(const fs::path& p) {
    if (!file_created.load()) {
        std::lock_guard<std::mutex> lock(fs_mutex);
        if (!file_created.load()) { // Double-checked locking
            fs::create_directory(p);
            file_created.store(true);
        }
    }
}
```
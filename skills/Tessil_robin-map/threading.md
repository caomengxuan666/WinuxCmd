# Threading

**Thread safety guarantees**
```cpp
// Same guarantees as std::unordered_map:
// - Multiple concurrent readers are safe
// - Any writer requires exclusive access
// - No concurrent readers with writers

// BAD: Concurrent read and write
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}};

// Thread 1
void reader() {
    for (int i = 0; i < 100; ++i) {
        auto it = map.find(1);  // May read while thread 2 writes
    }
}

// Thread 2
void writer() {
    for (int i = 0; i < 100; ++i) {
        map[3] = 3;  // May write while thread 1 reads
    }
}

// GOOD: Use mutex for synchronization
std::mutex mtx;
tsl::robin_map<int, int> safe_map = {{1, 1}, {2, 2}};

void safe_reader() {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = safe_map.find(1);  // Safe
}

void safe_writer() {
    std::lock_guard<std::mutex> lock(mtx);
    safe_map[3] = 3;  // Safe
}
```

**Concurrent read pattern**
```cpp
// Multiple readers are safe without synchronization
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}, {3, 3}};

// Thread 1: Read only
void reader1() {
    for (int i = 0; i < 1000; ++i) {
        auto it = map.find(1);
        if (it != map.end()) {
            // Safe to read
        }
    }
}

// Thread 2: Read only
void reader2() {
    for (int i = 0; i < 1000; ++i) {
        auto it = map.find(2);
        if (it != map.end()) {
            // Safe to read
        }
    }
}
```

**Read-write lock pattern for better performance**
```cpp
#include <shared_mutex>

class ThreadSafeRobinMap {
    mutable std::shared_mutex mtx;
    tsl::robin_map<int, int> map;
    
public:
    void insert(int key, int value) {
        std::unique_lock lock(mtx);
        map[key] = value;
    }
    
    std::optional<int> find(int key) const {
        std::shared_lock lock(mtx);  // Multiple readers allowed
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    void erase(int key) {
        std::unique_lock lock(mtx);
        map.erase(key);
    }
};
```

**Thread-safe iteration pattern**
```cpp
// BAD: Iterating without synchronization
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}};

void unsafe_iteration() {
    for (auto it = map.begin(); it != map.end(); ++it) {
        // Another thread may modify map during iteration
    }
}

// GOOD: Copy then iterate
class SafeRobinMap {
    mutable std::mutex mtx;
    tsl::robin_map<int, int> map;
    
public:
    std::vector<std::pair<int, int>> snapshot() const {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<std::pair<int, int>> result;
        result.reserve(map.size());
        for (const auto& [key, value] : map) {
            result.emplace_back(key, value);
        }
        return result;
    }
};
```

**Thread-safe batch operations**
```cpp
class BatchProcessor {
    std::mutex mtx;
    tsl::robin_map<int, int> map;
    
public:
    void process_batch(const std::vector<std::pair<int, int>>& items) {
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& [key, value] : items) {
            map[key] = value;
        }
    }
    
    std::vector<int> get_keys() const {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<int> keys;
        keys.reserve(map.size());
        for (const auto& [key, _] : map) {
            keys.push_back(key);
        }
        return keys;
    }
};
```
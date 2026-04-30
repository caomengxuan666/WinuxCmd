# Threading


**Thread safety guarantees**

The non-parallel variants (`flat_hash_map`, `node_hash_map`) provide no thread safety guarantees. Concurrent reads are safe, but any concurrent write requires external synchronization. The parallel variants (`parallel_flat_hash_map`, `parallel_node_hash_map`) use internal sub-maps to allow concurrent writes to different sub-maps.

```cpp
#include <parallel_hashmap/phmap.h>
#include <thread>
#include <mutex>

// Non-parallel variant requires external mutex
class ThreadSafeMap {
    phmap::flat_hash_map<int, int> map;
    mutable std::mutex mtx;
    
public:
    void insert(int key, int value) {
        std::lock_guard<std::mutex> lock(mtx);
        map[key] = value;
    }
    
    int find(int key) const {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = map.find(key);
        return it != map.end() ? it->second : -1;
    }
};
```

**Using parallel variants for concurrent access**

```cpp
#include <parallel_hashmap/phmap.h>
#include <thread>
#include <vector>

void parallel_concurrent_access() {
    phmap::parallel_flat_hash_map<int, int> parallel_map;
    
    // Multiple threads can write concurrently
    std::vector<std::thread> writers;
    for (int t = 0; t < 4; ++t) {
        writers.emplace_back([&parallel_map, t]() {
            for (int i = 0; i < 10000; ++i) {
                parallel_map.try_emplace(t * 10000 + i, i);
            }
        });
    }
    
    // Multiple threads can read concurrently
    std::vector<std::thread> readers;
    for (int t = 0; t < 4; ++t) {
        readers.emplace_back([&parallel_map]() {
            for (int i = 0; i < 40000; ++i) {
                auto it = parallel_map.find(i);
                if (it != parallel_map.end()) {
                    volatile int val = it->second;
                }
            }
        });
    }
    
    for (auto& t : writers) t.join();
    for (auto& t : readers) t.join();
}
```

**Sub-map configuration for parallel variants**

```cpp
#include <parallel_hashmap/phmap.h>
#include <iostream>

void submap_configuration() {
    // Default: 8 sub-maps (4 bits for sub-map selection)
    phmap::parallel_flat_hash_map<int, int> default_map;
    
    // Custom: 16 sub-maps (use more sub-maps for higher concurrency)
    // Template params: Key, Value, Hash, Equal, Allocator, SubMapCount (as power of 2)
    phmap::parallel_flat_hash_map<int, int, 
        std::hash<int>, 
        std::equal_to<int>, 
        std::allocator<std::pair<const int, int>>,
        4> custom_map;  // 2^4 = 16 sub-maps
    
    // Benchmark concurrent access
    auto worker = [](auto& map, int id) {
        for (int i = 0; i < 10000; ++i) {
            map[id * 10000 + i] = i;
        }
    };
    
    std::thread t1(worker, std::ref(default_map), 0);
    std::thread t2(worker, std::ref(default_map), 1);
    t1.join(); t2.join();
    
    std::cout << "Default map size: " << default_map.size() << "\n";
}
```

**Read-only concurrent access patterns**

```cpp
#include <parallel_hashmap/phmap.h>
#include <thread>
#include <vector>
#include <atomic>

class ConcurrentReadCache {
    phmap::flat_hash_map<int, std::string> cache;  // Populated once
    std::atomic<bool> initialized{false};
    
public:
    void initialize() {
        if (!initialized.exchange(true)) {
            // Single-threaded initialization
            for (int i = 0; i < 1000; ++i) {
                cache.try_emplace(i, "value_" + std::to_string(i));
            }
        }
    }
    
    std::string_view lookup(int key) const {
        // Safe: concurrent reads after initialization
        auto it = cache.find(key);
        return it != cache.end() ? it->second : "not found";
    }
};

void read_only_example() {
    ConcurrentReadCache cache;
    cache.initialize();
    
    std::vector<std::thread> readers;
    for (int i = 0; i < 8; ++i) {
        readers.emplace_back([&cache]() {
            for (int j = 0; j < 1000; ++j) {
                volatile auto val = cache.lookup(j);
            }
        });
    }
    
    for (auto& t : readers) t.join();
}
```
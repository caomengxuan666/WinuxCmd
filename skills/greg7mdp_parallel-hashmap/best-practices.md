# Best Practices


**Always reserve capacity when size is known**

```cpp
#include <parallel_hashmap/phmap.h>

void process_large_dataset(const std::vector<int>& data) {
    phmap::flat_hash_map<int, int> frequency;
    frequency.reserve(data.size());  // Pre-allocate exact capacity
    
    for (int value : data) {
        frequency[value]++;
    }
}
```

**Use try_emplace for conditional insertion**

```cpp
phmap::flat_hash_map<int, std::unique_ptr<ExpensiveObject>> cache;

void cache_object(int id, std::unique_ptr<ExpensiveObject> obj) {
    // try_emplace won't create if key exists, avoids unnecessary allocation
    auto [it, inserted] = cache.try_emplace(id, std::move(obj));
    if (!inserted) {
        // Object already cached, obj is still valid
        std::cout << "Cache hit for id: " << id << "\n";
    }
}
```

**Prefer parallel variants for multi-threaded workloads**

```cpp
#include <parallel_hashmap/phmap.h>
#include <thread>
#include <vector>

class ConcurrentCounter {
    phmap::parallel_flat_hash_map<std::string, int> counts;
    
public:
    void increment(const std::string& key) {
        // Safe for concurrent calls from multiple threads
        counts[key]++;
    }
    
    int get_count(const std::string& key) {
        auto it = counts.find(key);
        return it != counts.end() ? it->second : 0;
    }
};
```

**Use heterogeneous lookup to avoid string copies**

```cpp
#include <parallel_hashmap/phmap.h>
#include <string_view>

class Dictionary {
    phmap::flat_hash_map<std::string, std::string> words;
    
public:
    // Accept string_view to avoid copies from C strings or substrings
    std::string_view lookup(std::string_view word) const {
        auto it = words.find(word);  // Heterogeneous lookup
        return it != words.end() ? it->second : "not found";
    }
};
```

**Combine btree for ordered iteration with hash map for fast lookup**

```cpp
#include <parallel_hashmap/phmap.h>
#include <parallel_hashmap/btree.h>

class HybridCache {
    phmap::flat_hash_map<int, std::string> fast_lookup;
    phmap::btree_map<int, std::string> ordered_access;
    
public:
    void insert(int key, std::string value) {
        fast_lookup[key] = value;
        ordered_access[key] = value;
    }
    
    std::string* find_fast(int key) {
        auto it = fast_lookup.find(key);
        return it != fast_lookup.end() ? &it->second : nullptr;
    }
    
    void iterate_ordered() const {
        for (const auto& [key, value] : ordered_access) {
            // Iterates in key order
        }
    }
};
```
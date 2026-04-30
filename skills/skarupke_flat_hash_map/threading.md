# Threading

### Thread Safety Guarantees
```cpp
// flat_hash_map provides NO thread safety guarantees
// All operations require external synchronization

ska::flat_hash_map<int, int> map;

// BAD: Concurrent read and write
// Thread 1
map[1] = 10;  // Write
// Thread 2
auto it = map.find(1);  // Read - RACE CONDITION

// GOOD: External synchronization
std::mutex mutex;
ska::flat_hash_map<int, int> safe_map;

// Thread 1
{
    std::lock_guard<std::mutex> lock(mutex);
    safe_map[1] = 10;
}
// Thread 2
{
    std::lock_guard<std::mutex> lock(mutex);
    auto it = safe_map.find(1);
}
```

### Concurrent Read Access Pattern
```cpp
// Multiple readers are safe with shared lock
std::shared_mutex rw_mutex;
ska::flat_hash_map<int, int> map;

void reader_thread() {
    std::shared_lock<std::shared_mutex> lock(rw_mutex);
    auto it = map.find(42);
    if (it != map.end()) {
        process(it->second);
    }
}

void writer_thread() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex);
    map[42] = 100;
}
```

### Thread-Safe Wrapper Pattern
```cpp
template<typename Key, typename Value>
class ThreadSafeFlatHashMap {
    ska::flat_hash_map<Key, Value> map_;
    mutable std::shared_mutex mutex_;
    
public:
    bool find(const Key& key, Value& value) const {
        std::shared_lock lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            value = it->second;
            return true;
        }
        return false;
    }
    
    void insert(const Key& key, const Value& value) {
        std::unique_lock lock(mutex_);
        map_[key] = value;
    }
    
    void reserve(size_t capacity) {
        std::unique_lock lock(mutex_);
        map_.reserve(capacity);
    }
};
```

### Performance Considerations for Concurrent Access
```cpp
// Fine-grained locking can improve performance
class StripedHashMap {
    static constexpr size_t NUM_STRIPES = 64;
    ska::flat_hash_map<int, int> maps_[NUM_STRIPES];
    std::mutex mutexes_[NUM_STRIPES];
    
    size_t get_stripe(int key) const {
        return std::hash<int>{}(key) % NUM_STRIPES;
    }
    
public:
    void insert(int key, int value) {
        size_t stripe = get_stripe(key);
        std::lock_guard<std::mutex> lock(mutexes_[stripe]);
        maps_[stripe][key] = value;
    }
    
    bool find(int key, int& value) {
        size_t stripe = get_stripe(key);
        std::lock_guard<std::mutex> lock(mutexes_[stripe]);
        auto it = maps_[stripe].find(key);
        if (it != maps_[stripe].end()) {
            value = it->second;
            return true;
        }
        return false;
    }
};
```

### Read-Copy-Update (RCU) Pattern for Read-Heavy Workloads
```cpp
// For extremely read-heavy workloads, consider RCU-like pattern
class RCUMap {
    std::shared_ptr<ska::flat_hash_map<int, int>> current_;
    std::mutex write_mutex_;
    
public:
    RCUMap() : current_(std::make_shared<ska::flat_hash_map<int, int>>()) {}
    
    int read(int key) {
        auto map = std::atomic_load(&current_);
        auto it = map->find(key);
        return it != map->end() ? it->second : -1;
    }
    
    void write(int key, int value) {
        std::lock_guard<std::mutex> lock(write_mutex_);
        auto new_map = std::make_shared<ska::flat_hash_map<int, int>>(*current_);
        (*new_map)[key] = value;
        std::atomic_store(&current_, new_map);
    }
};
```
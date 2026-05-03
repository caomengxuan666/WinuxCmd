# Threading

```cpp
#include <nlohmann/json.hpp>
#include <mutex>
#include <shared_mutex>
using json = nlohmann::json;

// Thread safety guarantees:
// - Individual json objects are NOT thread-safe
// - Const methods are safe for concurrent read access
// - Non-const methods require external synchronization

// 1. Read-only concurrent access (safe)
const json config = json::parse(config_string);

// Multiple threads can read simultaneously
void thread_read_config() {
    std::string host = config.value("host", "localhost");
    int port = config.value("port", 8080);
    // Safe: only const methods called
}

// 2. Read-write with mutex protection
class ThreadSafeJson {
    mutable std::shared_mutex mutex_;
    json data_;
    
public:
    void update(const std::string& key, const json& value) {
        std::unique_lock lock(mutex_);
        data_[key] = value;
    }
    
    json get(const std::string& key) const {
        std::shared_lock lock(mutex_);
        return data_.value(key, json());
    }
};

// 3. Thread-local storage for per-thread instances
thread_local json thread_local_data;

void worker_thread() {
    thread_local_data["thread_id"] = std::this_thread::get_id();
    // Each thread has its own copy
}

// 4. Atomic operations on shared json
std::atomic<json*> shared_json_ptr;

void update_shared() {
    json* new_json = new json({{"value", 42}});
    json* old = shared_json_ptr.exchange(new_json);
    delete old;  // Old instance safe to delete
}

// 5. Copy-on-write pattern
class CowJson {
    std::shared_ptr<json> data_;
    mutable std::mutex mutex_;
    
public:
    json read() const {
        std::lock_guard lock(mutex_);
        return *data_;  // Returns copy
    }
    
    void write(const json& new_data) {
        std::lock_guard lock(mutex_);
        data_ = std::make_shared<json>(new_data);
    }
};

// 6. NEVER share mutable json without synchronization
// BAD:
json shared_data;
void thread1() { shared_data["key1"] = "value1"; }
void thread2() { shared_data["key2"] = "value2"; }  // Data race!

// GOOD:
std::mutex mtx;
json safe_data;
void thread1() { std::lock_guard lock(mtx); safe_data["key1"] = "value1"; }
void thread2() { std::lock_guard lock(mtx); safe_data["key2"] = "value2"; }
```
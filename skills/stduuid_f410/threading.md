# Threading

```cpp
// Thread safety: UUID objects are thread-safe for reading
#include <uuid.h>
#include <thread>
#include <vector>

uuids::uuid shared_id;

void read_uuid() {
    // Reading UUID from multiple threads is safe
    std::string str = uuids::to_string(shared_id);
    bool is_nil = shared_id.is_nil();
}

int main() {
    shared_id = uuids::uuid_system_generator{}();
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(read_uuid);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
```

```cpp
// Thread safety: UUID generators are NOT thread-safe
#include <uuid.h>
#include <thread>
#include <mutex>

// BAD: Shared generator without synchronization
uuids::uuid_system_generator shared_gen;

void generate_bad() {
    uuids::uuid id = shared_gen(); // Race condition!
}

// GOOD: Use thread-local storage or mutex
thread_local uuids::uuid_system_generator tls_gen;

void generate_good_tls() {
    uuids::uuid id = tls_gen(); // Thread-local, no synchronization needed
}

// Alternative: Mutex protection
std::mutex gen_mutex;
uuids::uuid_system_generator protected_gen;

void generate_good_mutex() {
    std::lock_guard<std::mutex> lock(gen_mutex);
    uuids::uuid id = protected_gen();
}
```

```cpp
// Thread safety: Concurrent UUID storage
#include <uuid.h>
#include <unordered_set>
#include <shared_mutex>

class ThreadSafeUuidSet {
    std::unordered_set<uuids::uuid> uuids_;
    mutable std::shared_mutex mutex_;
    
public:
    void insert(const uuids::uuid& id) {
        std::unique_lock lock(mutex_);
        uuids_.insert(id);
    }
    
    bool contains(const uuids::uuid& id) const {
        std::shared_lock lock(mutex_);
        return uuids_.find(id) != uuids_.end();
    }
};
```

```cpp
// Thread safety: Atomic UUID operations
#include <uuid.h>
#include <atomic>
#include <optional>

class AtomicUuid {
    std::atomic<uint64_t> high_;
    std::atomic<uint64_t> low_;
    
public:
    AtomicUuid() : high_(0), low_(0) {}
    
    void store(const uuids::uuid& id) {
        auto bytes = id.as_bytes();
        uint64_t high, low;
        std::memcpy(&high, bytes.data(), 8);
        std::memcpy(&low, bytes.data() + 8, 8);
        high_.store(high, std::memory_order_release);
        low_.store(low, std::memory_order_release);
    }
    
    uuids::uuid load() const {
        uint64_t high = high_.load(std::memory_order_acquire);
        uint64_t low = low_.load(std::memory_order_acquire);
        std::array<uint8_t, 16> bytes;
        std::memcpy(bytes.data(), &high, 8);
        std::memcpy(bytes.data() + 8, &low, 8);
        return uuids::uuid(bytes);
    }
};
```
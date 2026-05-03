# Threading

**Thread Safety of absl::flat_hash_map**

```cpp
#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"

// flat_hash_map is NOT thread-safe for concurrent modifications
absl::flat_hash_map<int, int> shared_map;
absl::Mutex map_mutex;

void thread_safe_insert(int key, int value) {
    absl::MutexLock lock(&map_mutex);
    shared_map[key] = value; // Safe under mutex
}

void thread_safe_lookup(int key) {
    absl::MutexLock lock(&map_mutex);
    auto it = shared_map.find(key); // Safe under mutex
}
```

**Thread Safety of absl::BitGen**

```cpp
#include "absl/random/random.h"

// absl::BitGen is NOT thread-safe
thread_local absl::BitGen tls_gen; // Use thread_local for thread safety

int get_random_number() {
    return absl::Uniform(tls_gen, 1, 100); // Safe - each thread has its own generator
}
```

**Thread Safety of absl::Status**

```cpp
#include "absl/status/status.h"

// absl::Status is thread-safe for reading after construction
absl::Status status = absl::InternalError("Error");

// Multiple threads can read the status safely
void thread1() {
    if (!status.ok()) {
        // Safe to read
    }
}

void thread2() {
    std::string message = std::string(status.message()); // Safe to read
}
```

**Thread Safety of absl::Mutex**

```cpp
#include "absl/synchronization/mutex.h"

absl::Mutex mu;
int shared_data;

void writer_thread() {
    absl::MutexLock lock(&mu);
    shared_data = 42; // Exclusive access
}

void reader_thread() {
    absl::ReaderMutexLock lock(&mu);
    int value = shared_data; // Shared read access
}
```
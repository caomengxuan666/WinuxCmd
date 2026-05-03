# Threading

### Thread safety of folly::Optional
```cpp
#include <folly/Optional.h>
#include <mutex>

// NOT thread-safe for concurrent modification
folly::Optional<int> shared_opt;
std::mutex opt_mutex;

void threadSafeUpdate(int value) {
    std::lock_guard<std::mutex> lock(opt_mutex);
    shared_opt = value;  // Protected by mutex
}

void threadSafeRead() {
    std::lock_guard<std::mutex> lock(opt_mutex);
    if (shared_opt.hasValue()) {
        int val = shared_opt.value();  // Protected read
    }
}
```

### Thread safety of folly::dynamic
```cpp
#include <folly/dynamic.h>
#include <mutex>

// folly::dynamic is NOT thread-safe
folly::dynamic config = folly::dynamic::object;
std::mutex config_mutex;

void updateConfig(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(config_mutex);
    config[key] = value;
}

void readConfig(const std::string& key) {
    std::lock_guard<std::mutex> lock(config_mutex);
    if (config.count(key)) {
        auto val = config[key];
        // Use val...
    }
}
```

### Thread safety of fbstring
```cpp
#include <folly/FBString.h>
#include <shared_mutex>

// fbstring is NOT thread-safe for concurrent modification
folly::fbstring shared_string;
std::shared_mutex string_mutex;

void writeString(const std::string& data) {
    std::unique_lock<std::shared_mutex> lock(string_mutex);
    shared_string = data;
}

std::string readString() {
    std::shared_lock<std::shared_mutex> lock(string_mutex);
    return shared_string.toStdString();
}
```

### Concurrent access patterns
```cpp
#include <folly/FBVector.h>
#include <mutex>

// fbvector is NOT thread-safe
folly::fbvector<int> shared_vector;
std::mutex vec_mutex;

void addElement(int value) {
    std::lock_guard<std::mutex> lock(vec_mutex);
    shared_vector.push_back(value);
}

void processElements() {
    std::lock_guard<std::mutex> lock(vec_mutex);
    for (auto& elem : shared_vector) {
        // Process element
    }
}

// For read-heavy workloads, consider copy-on-read
folly::fbvector<int> getSnapshot() {
    std::lock_guard<std::mutex> lock(vec_mutex);
    return shared_vector;  // Returns a copy for safe iteration
}
```
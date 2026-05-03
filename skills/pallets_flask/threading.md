# Threading

**Thread safety guarantees**

Flask provides the following thread safety guarantees:
- Route handlers run in separate threads from the thread pool
- Request objects are thread-local and not shared between handlers
- Response objects must be constructed within the handler thread
- App configuration is read-only after `run()` is called

**Concurrent access patterns**

```cpp
#include <flask/flask.hpp>
#include <shared_mutex>
#include <unordered_map>

class ThreadSafeCache {
public:
    void put(const std::string& key, const std::string& value) {
        std::unique_lock lock(mutex_);
        cache_[key] = value;
    }
    
    std::optional<std::string> get(const std::string& key) {
        std::shared_lock lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
private:
    std::shared_mutex mutex_;
    std::unordered_map<std::string, std::string> cache_;
};

flask::app create_thread_safe_app() {
    flask::app app;
    auto cache = std::make_shared<ThreadSafeCache>();
    
    app.route("/cache/<key>", [cache](const flask::request& req) {
        std::string key = req.path_param("key");
        auto value = cache->get(key);
        if (value) {
            return flask::response(*value);
        }
        return flask::response("Not found", 404);
    });
    
    app.route("/cache/<key>/<value>", flask::method::PUT, [cache](const flask::request& req) {
        std::string key = req.path_param("key");
        std::string value = req.path_param("value");
        cache->put(key, value);
        return flask::response("Stored");
    });
    
    return app;
}
```

**Atomic operations for counters**

```cpp
#include <flask/flask.hpp>
#include <atomic>

flask::app create_counter_app() {
    flask::app app;
    auto counter = std::make_shared<std::atomic<uint64_t>>(0);
    
    app.route("/increment", [counter](const flask::request& req) {
        counter->fetch_add(1, std::memory_order_relaxed);
        return flask::response("Count: " + std::to_string(counter->load()));
    });
    
    app.route("/count", [counter](const flask::request& req) {
        return flask::response("Total: " + std::to_string(counter->load()));
    });
    
    return app;
}
```

**Thread pool configuration**

```cpp
#include <flask/flask.hpp>

int main() {
    flask::app app;
    
    // Configure thread pool size based on hardware
    unsigned int thread_count = std::thread::hardware_concurrency();
    app.thread_pool_size(thread_count);
    
    // Set request timeout
    app.request_timeout(std::chrono::seconds(30));
    
    // Configure max concurrent connections
    app.max_connections(1000);
    
    app.route("/", [](const flask::request& req) {
        return flask::response("Hello");
    });
    
    app.run(8080);
}
```

**Thread-local storage for per-request data**

```cpp
#include <flask/flask.hpp>
#include <thread_local>

thread_local std::string request_id;

flask::app create_tls_app() {
    flask::app app;
    
    app.before_request([](flask::request& req) {
        request_id = generate_uuid();
        req.set_attribute("request_id", request_id);
    });
    
    app.route("/process", [](const flask::request& req) {
        // request_id is thread-local and unique per request
        return flask::response("Processing request: " + request_id);
    });
    
    return app;
}
```
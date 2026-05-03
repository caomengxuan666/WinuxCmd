# Threading

> 🔗 See [safety.md](safety.md) for thread-safety red lines and [pitfalls.md](pitfalls.md) for threading pitfalls.
> See [lifecycle.md](lifecycle.md) for server lifecycle management.
> See [best-practices.md](best-practices.md) for connection pooling patterns.

**Thread safety guarantees**
```cpp
// Server is NOT thread-safe for configuration
httplib::Server svr;

// BAD: Configuring server from multiple threads
std::thread t1([&svr]() {
    svr.Get("/a", handler_a); // Race condition
});

std::thread t2([&svr]() {
    svr.Get("/b", handler_b); // Race condition
});

// GOOD: Configure server before starting
svr.Get("/a", handler_a);
svr.Get("/b", handler_b);
svr.listen("0.0.0.0", 8080);
```

**Thread-safe client usage**
```cpp
// Client is NOT thread-safe
httplib::Client cli("http://example.com");

// BAD: Using same client from multiple threads
std::thread t1([&cli]() {
    cli.Get("/api1"); // Race condition
});

std::thread t2([&cli]() {
    cli.Get("/api2"); // Race condition
});

// GOOD: Use separate clients per thread
std::thread t1([]() {
    httplib::Client cli("http://example.com");
    cli.Get("/api1");
});

std::thread t2([]() {
    httplib::Client cli("http://example.com");
    cli.Get("/api2");
});
```

**Thread pool for request handling**
```cpp
// Server uses a thread pool for handling requests
httplib::Server svr;

// Configure thread pool size
svr.new_task_queue = [] {
    return new httplib::ThreadPool(8); // 8 threads
};

svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
    // This runs on a thread from the pool
    res.set_content("OK", "text/plain");
});
```

**Thread-safe shared state**
```cpp
#include <shared_mutex>

class SharedState {
    std::map<std::string, std::string> data_;
    mutable std::shared_mutex mutex_;
    
public:
    void update(const std::string& key, const std::string& value) {
        std::unique_lock lock(mutex_);
        data_[key] = value;
    }
    
    std::string get(const std::string& key) const {
        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        return it != data_.end() ? it->second : "";
    }
};

// Usage in server
SharedState state;
httplib::Server svr;

svr.Get("/get", [&state](const httplib::Request& req, httplib::Response& res) {
    auto key = req.get_param_value("key");
    res.set_content(state.get(key), "text/plain");
});

svr.Post("/set", [&state](const httplib::Request& req, httplib::Response& res) {
    auto key = req.get_param_value("key");
    auto value = req.get_param_value("value");
    state.update(key, value);
    res.set_content("OK", "text/plain");
});
```
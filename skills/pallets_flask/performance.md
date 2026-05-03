# Performance

**Performance characteristics**

Flask is designed for moderate throughput with low latency for typical web applications. Key performance considerations:

- Route matching is O(n) where n is the number of routes
- Request parsing is O(body_size)
- Response serialization is O(response_size)
- Thread pool overhead adds ~1ms per request

**Optimization tips**

```cpp
// Use string views for read-only access
#include <flask/flask.hpp>
#include <string_view>

flask::app create_optimized_app() {
    flask::app app;
    
    // Pre-allocate common responses
    static const std::string OK_RESPONSE = "OK";
    static const std::string NOT_FOUND = "Not Found";
    
    app.route("/health", [](const flask::request& req) {
        return flask::response(OK_RESPONSE); // No allocation
    });
    
    return app;
}
```

**Reduce allocations with response pooling**

```cpp
#include <flask/flask.hpp>
#include <vector>
#include <mutex>

class ResponsePool {
public:
    flask::response acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_.empty()) {
            return flask::response();
        }
        auto resp = std::move(pool_.back());
        pool_.pop_back();
        return resp;
    }
    
    void release(flask::response&& resp) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push_back(std::move(resp));
    }
    
private:
    std::vector<flask::response> pool_;
    std::mutex mutex_;
};
```

**Use connection pooling for database access**

```cpp
#include <flask/flask.hpp>
#include <pqxx/pqxx>

class DatabasePool {
public:
    DatabasePool(size_t size, const std::string& conn_string) {
        for (size_t i = 0; i < size; ++i) {
            pool_.push(std::make_unique<pqxx::connection>(conn_string));
        }
    }
    
    std::unique_ptr<pqxx::connection> acquire() {
        std::unique_ptr<pqxx::connection> conn;
        pool_.wait_pop(conn);
        return conn;
    }
    
    void release(std::unique_ptr<pqxx::connection> conn) {
        pool_.push(std::move(conn));
    }
    
private:
    moodycamel::ConcurrentQueue<std::unique_ptr<pqxx::connection>> pool_;
};
```

**Profile and benchmark**

```cpp
#include <flask/flask.hpp>
#include <chrono>
#include <iostream>

void benchmark_routes() {
    flask::app app;
    
    app.route("/fast", [](const flask::request& req) {
        return flask::response("Fast response");
    });
    
    app.route("/slow", [](const flask::request& req) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return flask::response("Slow response");
    });
    
    auto start = std::chrono::steady_clock::now();
    // Simulate 1000 requests
    for (int i = 0; i < 1000; ++i) {
        flask::request req;
        req.method("GET");
        req.path("/fast");
        app.handle_request(req);
    }
    auto end = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "1000 requests in " << duration.count() << "ms" << std::endl;
}
```
# Performance

**Performance Characteristics**

Actix Web is designed for high throughput with minimal overhead. Key characteristics include:
- Zero-copy request parsing where possible
- Efficient connection pooling with keep-alive
- Asynchronous I/O using Tokio's work-stealing scheduler
- Minimal memory allocation per request

```cpp
// Optimized handler with minimal allocations
int main() {
    actix_web::HttpServer server;
    server.route("/fast", [](auto req) {
        // Pre-allocate response buffer
        static const std::string response = "Fast response";
        return actix_web::HttpResponse::Ok().body(response);
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Allocation Patterns**

Avoid unnecessary allocations in hot paths:
```cpp
// BAD: Allocating per request
server.route("/bad", [](auto req) {
    std::string result = "Result: " + std::to_string(42); // Allocation
    return actix_web::HttpResponse::Ok().body(result);
});

// GOOD: Pre-allocate and reuse
static const std::string cached_result = "Result: 42";
server.route("/good", [](auto req) {
    return actix_web::HttpResponse::Ok().body(cached_result);
});
```

**Optimization Tips**

1. Use connection pooling for database connections
2. Enable compression for large responses
3. Configure appropriate worker count (usually CPU cores)
4. Use streaming for large payloads

```cpp
// Optimized server configuration
int main() {
    actix_web::HttpServer server;
    server.set_workers(std::thread::hardware_concurrency());
    server.set_backlog(2048);
    server.set_keep_alive(std::chrono::seconds(75));
    server.wrap(actix_web::middleware::Compress::default());
    server.route("/", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Optimized");
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```
# Performance

> 🔧 See [best-practices.md](best-practices.md) for connection pooling.
> See [overview.md](overview.md) for architecture considerations.
> See [threading.md](threading.md) for thread pool configuration.

**Memory allocation patterns**
```cpp
// BAD: Frequent allocations in request handler
svr.Get("/data", [](const httplib::Request&, httplib::Response& res) {
    std::string data;
    for (int i = 0; i < 1000; ++i) {
        data += "item" + std::to_string(i) + "\n"; // Many allocations
    }
    res.set_content(data, "text/plain");
});

// GOOD: Pre-allocate and reuse
svr.Get("/data", [](const httplib::Request&, httplib::Response& res) {
    std::string data;
    data.reserve(10000); // Pre-allocate
    for (int i = 0; i < 1000; ++i) {
        data += "item" + std::to_string(i) + "\n";
    }
    res.set_content(data, "text/plain");
});
```

**Connection reuse for performance**
```cpp
// BAD: Creating new client for each request
for (int i = 0; i < 100; ++i) {
    httplib::Client cli("http://example.com"); // New connection each time
    cli.Get("/api");
}

// GOOD: Reuse client with keep-alive
httplib::Client cli("http://example.com");
cli.set_keep_alive(true);
for (int i = 0; i < 100; ++i) {
    cli.Get("/api"); // Reuses connection
}
```

**Optimizing response generation**
```cpp
// BAD: String concatenation in hot path
svr.Get("/large", [](const httplib::Request&, httplib::Response& res) {
    std::string content;
    for (int i = 0; i < 10000; ++i) {
        content += "line " + std::to_string(i) + "\n"; // O(n²)
    }
    res.set_content(content, "text/plain");
});

// GOOD: Use string stream
svr.Get("/large", [](const httplib::Request&, httplib::Response& res) {
    std::ostringstream oss;
    for (int i = 0; i < 10000; ++i) {
        oss << "line " << i << "\n";
    }
    res.set_content(oss.str(), "text/plain");
});
```

**Performance considerations for local connections**
```cpp
// For local connections, consider using Unix domain sockets
// This avoids TCP overhead
// Note: This requires platform-specific implementation

// BAD: Using TCP for local communication
httplib::Client cli("http://localhost:8080");

// GOOD: Consider using shared memory or pipes for local IPC
// if performance is critical
```
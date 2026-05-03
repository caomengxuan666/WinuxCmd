# Lifecycle

> 🏗️ See [best-practices.md](best-practices.md) for RAII server patterns.
> See [threading.md](threading.md) for thread pool lifecycle.
> See [quickstart.md](quickstart.md) for basic server construction.

**Construction and initialization**
```cpp
// Default construction
httplib::Server svr;
httplib::Client cli("http://example.com");

// SSL construction
httplib::SSLServer ssl_svr("./cert.pem", "./key.pem");
httplib::SSLClient ssl_cli("https://example.com");

// Move construction
httplib::Server svr2 = std::move(svr);
```

**Resource management and cleanup**
```cpp
class ManagedServer {
    httplib::Server server_;
    std::thread server_thread_;
    
public:
    void start() {
        server_thread_ = std::thread([this]() {
            server_.listen("0.0.0.0", 8080);
        });
    }
    
    void stop() {
        server_.stop();
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
    }
    
    ~ManagedServer() {
        stop();
    }
};
```

**Move semantics**
```cpp
// Move assignment
httplib::Server svr1;
svr1.Get("/", [](const httplib::Request&, httplib::Response& res) {
    res.set_content("Hello", "text/plain");
});

httplib::Server svr2;
svr2 = std::move(svr1); // svr1 is now in valid but unspecified state

// Move with client
httplib::Client cli1("http://example.com");
httplib::Client cli2 = std::move(cli1); // cli1 should not be used
```

**Resource cleanup patterns**
```cpp
// Proper cleanup with stop
httplib::Server svr;
svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
    res.set_content("OK", "text/plain");
});

// Start server in background
std::thread t([&svr]() {
    svr.listen("0.0.0.0", 8080);
});

// Later, stop the server
svr.stop();
t.join(); // Wait for server to finish
```
# Best Practices

> 📖 See [lifecycle.md](lifecycle.md) for server lifecycle patterns.
> See [performance.md](performance.md) for optimization guidance.
> See [threading.md](threading.md) for thread-safe shared state patterns.

**Use RAII for server lifecycle management**
```cpp
class HttpServer {
    httplib::Server server_;
    
public:
    void start() {
        server_.Get("/health", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("OK", "text/plain");
        });
        
        if (!server_.listen("0.0.0.0", 8080)) {
            throw std::runtime_error("Failed to start server");
        }
    }
    
    void stop() {
        server_.stop();
    }
};
```

**Implement proper logging middleware**
```cpp
#include <chrono>

class LoggingMiddleware {
public:
    static void log_request(const httplib::Request& req, 
                           const httplib::Response& res,
                           std::chrono::microseconds duration) {
        std::cout << req.method << " " << req.path 
                  << " -> " << res.status 
                  << " (" << duration.count() / 1000.0 << "ms)" 
                  << std::endl;
    }
};

// Usage
httplib::Server svr;
svr.Get("/api", [](const httplib::Request& req, httplib::Response& res) {
    auto start = std::chrono::high_resolution_clock::now();
    
    res.set_content("Data", "application/json");
    
    auto end = std::chrono::high_resolution_clock::now();
    LoggingMiddleware::log_request(req, res, 
        std::chrono::duration_cast<std::chrono::microseconds>(end - start));
});
```

**Use connection pooling for clients**
```cpp
class HttpClientPool {
    std::vector<std::unique_ptr<httplib::Client>> clients_;
    std::mutex mutex_;
    size_t index_ = 0;
    
public:
    HttpClientPool(const std::string& host, size_t pool_size = 4) {
        for (size_t i = 0; i < pool_size; ++i) {
            auto client = std::make_unique<httplib::Client>(host);
            client->set_keep_alive(true);
            client->set_read_timeout(5, 0);
            clients_.push_back(std::move(client));
        }
    }
    
    httplib::Client& get() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& client = clients_[index_];
        index_ = (index_ + 1) % clients_.size();
        return *client;
    }
};
```

**Implement retry logic with exponential backoff**
```cpp
class RetryClient {
    httplib::Client client_;
    int max_retries_ = 3;
    
public:
    RetryClient(const std::string& host) : client_(host) {
        client_.set_read_timeout(5, 0);
        client_.set_connection_timeout(5, 0);
    }
    
    std::shared_ptr<httplib::Response> get_with_retry(const std::string& path) {
        for (int i = 0; i < max_retries_; ++i) {
            if (auto res = client_.Get(path)) {
                return res;
            }
            
            if (i < max_retries_ - 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * (1 << i)));
            }
        }
        return nullptr;
    }
};
```
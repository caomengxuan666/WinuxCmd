# Best Practices

### Reuse Client Instances
```cpp
// Create a reusable client with common configuration
class ApiClient {
private:
    reqwest::client client_;
    
public:
    ApiClient() : client_(
        reqwest::client_builder()
            .timeout(std::chrono::seconds(30))
            .default_headers({
                {"User-Agent", "MyApp/1.0"},
                {"Accept", "application/json"}
            })
            .build()
    ) {}
    
    reqwest::Response get(const std::string& path) {
        return client_.get(base_url_ + path).send();
    }
};
```

### Use Connection Pooling
```cpp
// Enable connection pooling for better performance
auto client = reqwest::client_builder()
    .pool_max_idle_per_host(10) // Keep 10 idle connections per host
    .pool_idle_timeout(std::chrono::seconds(90))
    .build();
```

### Implement Retry Logic
```cpp
template<typename Func>
auto with_retry(Func func, int max_retries = 3) -> decltype(func()) {
    for (int i = 0; i < max_retries; ++i) {
        try {
            auto result = func();
            if (result.status_code() < 500) return result;
        } catch (const std::exception& e) {
            if (i == max_retries - 1) throw;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1 << i));
    }
    throw std::runtime_error("Max retries exceeded");
}
```

### Proper Resource Cleanup
```cpp
// RAII-style resource management
class ScopedResponse {
private:
    reqwest::Response response_;
public:
    ScopedResponse(reqwest::Response&& resp) : response_(std::move(resp)) {}
    ~ScopedResponse() {
        if (response_.status_code() != 0) {
            response_.text(); // Ensure body is consumed
        }
    }
    // Prevent copying, allow moving
    ScopedResponse(const ScopedResponse&) = delete;
    ScopedResponse& operator=(const ScopedResponse&) = delete;
    ScopedResponse(ScopedResponse&&) = default;
};
```

### Logging and Monitoring
```cpp
// Add request/response logging
reqwest::Response logged_request(reqwest::client& client, const std::string& url) {
    auto start = std::chrono::steady_clock::now();
    auto response = client.get(url).send();
    auto end = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    log_info("Request to {} took {}ms, status: {}", url, duration.count(), response.status_code());
    
    return response;
}
```
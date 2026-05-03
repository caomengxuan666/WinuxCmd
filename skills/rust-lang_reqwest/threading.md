# Threading

### Thread Safety Guarantees
```cpp
// reqwest::client is NOT thread-safe for concurrent modification
reqwest::client client;

// BAD: Concurrent access from multiple threads
std::thread t1([&]() {
    auto r1 = client.get("https://api1.example.com").send();
});
std::thread t2([&]() {
    auto r2 = client.get("https://api2.example.com").send();
});
t1.join();
t2.join(); // Undefined behavior!

// GOOD: Use separate clients per thread
void thread_function(const std::string& url) {
    reqwest::client thread_client; // Each thread has its own client
    auto response = thread_client.get(url).send();
    // Process response...
}

std::thread t1(thread_function, "https://api1.example.com");
std::thread t2(thread_function, "https://api2.example.com");
t1.join();
t2.join();
```

### Thread Pool Pattern
```cpp
// Thread-safe wrapper for reqwest
class ThreadSafeHttpClient {
private:
    reqwest::client client_;
    std::mutex mutex_;
    
public:
    reqwest::Response get(const std::string& url) {
        std::lock_guard<std::mutex> lock(mutex_);
        return client_.get(url).send();
    }
};

// Or better: use thread-local storage
class ThreadLocalClient {
public:
    reqwest::client& get() {
        thread_local reqwest::client client;
        return client;
    }
};
```

### Async Operations
```cpp
// reqwest::Response is NOT thread-safe
auto response = client.get("https://example.com").send();

// BAD: Accessing response from multiple threads
std::thread t1([&]() {
    std::cout << response.status_code(); // Race condition
});
std::thread t2([&]() {
    std::cout << response.text(); // Race condition
});

// GOOD: Move response to single thread
std::thread t([response = std::move(response)]() mutable {
    std::cout << response.status_code();
    std::cout << response.text();
});
```

### Synchronization Best Practices
```cpp
// Use async with proper synchronization
std::mutex results_mutex;
std::vector<std::string> results;

std::vector<std::future<void>> futures;
for (int i = 0; i < 10; ++i) {
    futures.push_back(std::async(std::launch::async, [&]() {
        auto client = reqwest::client(); // Thread-local client
        auto response = client.get("https://api.example.com/data").send();
        
        std::lock_guard<std::mutex> lock(results_mutex);
        results.push_back(response.text());
    }));
}

for (auto& future : futures) {
    future.get();
}
```
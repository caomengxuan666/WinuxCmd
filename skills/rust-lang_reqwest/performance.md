# Performance

### Connection Pooling
```cpp
// BAD: Creating new client for each request
for (int i = 0; i < 100; ++i) {
    auto client = reqwest::client(); // Expensive: new TCP connection each time
    auto response = client.get("https://api.example.com/data").send();
}

// GOOD: Reuse client with connection pooling
auto client = reqwest::client_builder()
    .pool_max_idle_per_host(10)
    .build();
for (int i = 0; i < 100; ++i) {
    auto response = client.get("https://api.example.com/data").send();
    response.text(); // Consume body to allow connection reuse
}
```

### Minimize Allocations
```cpp
// BAD: Multiple string allocations
auto response = client.get("https://api.example.com/data").send();
std::string data = response.text();
nlohmann::json json = nlohmann::json::parse(data);

// GOOD: Reuse buffers
std::string buffer;
buffer.reserve(4096); // Pre-allocate
auto response = client.get("https://api.example.com/data").send();
buffer = response.text();
nlohmann::json json = nlohmann::json::parse(buffer);
```

### Async Performance
```cpp
// BAD: Sequential async requests
for (const auto& url : urls) {
    auto future = client.get(url).send_async();
    auto response = future.get(); // Blocks until complete
}

// GOOD: Parallel async requests
std::vector<std::future<reqwest::Response>> futures;
for (const auto& url : urls) {
    futures.push_back(client.get(url).send_async());
}
for (auto& future : futures) {
    auto response = future.get(); // Process as they complete
}
```

### Compression
```cpp
// Enable compression for bandwidth savings
auto client = reqwest::client_builder()
    .gzip(true) // Enable gzip decompression
    .build();

// Server must support compression
auto response = client.get("https://api.example.com/large-data")
    .header("Accept-Encoding", "gzip, deflate")
    .send();
```
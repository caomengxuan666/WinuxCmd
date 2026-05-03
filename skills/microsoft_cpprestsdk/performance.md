# Performance

```cpp
// Reuse http_client instances for the same base URI
// BAD: Creating new client for each request
for (int i = 0; i < 100; ++i) {
    http_client client(U("https://api.example.com"));
    client.request(methods::GET, U("/data")).wait();
}

// GOOD: Reuse client instance
http_client client(U("https://api.example.com"));
for (int i = 0; i < 100; ++i) {
    client.request(methods::GET, U("/data")).wait();
}
```

```cpp
// Use connection pooling with http_client_config
http_client_config config;
config.set_timeout(std::chrono::seconds(30));
config.set_validate_certificates(true); // Disable in dev for speed

http_client client(U("https://api.example.com"), config);
```

```cpp
// Batch requests to reduce overhead
pplx::task<void> sendBatchRequests(http_client& client) {
    std::vector<pplx::task<http_response>> tasks;
    
    for (int i = 0; i < 10; ++i) {
        tasks.push_back(client.request(methods::GET, 
            U("/items/") + utility::conversions::to_string_t(i)));
    }
    
    return pplx::when_all(tasks.begin(), tasks.end())
        .then([](std::vector<http_response> responses) {
            for (auto& response : responses) {
                // Process each response
            }
        });
}
```

```cpp
// Use streams for large data instead of string extraction
// BAD: Loading entire response into memory
client.request(methods::GET, U("/large-file"))
    .then([](http_response response) {
        return response.extract_string(); // Memory intensive
    })
    .wait();

// GOOD: Stream the response
auto outputStream = std::make_shared<concurrency::streams::ostream>();
concurrency::streams::fstream::open_ostream(U("output.bin"))
    .then([&client, outputStream](concurrency::streams::ostream file) {
        *outputStream = file;
        return client.request(methods::GET, U("/large-file"));
    })
    .then([outputStream](http_response response) {
        return response.body().read_to_end(outputStream->streambuf());
    })
    .wait();
```
# Pitfalls

BAD: Blocking on async operations in a synchronous context
```cpp
// BAD: This blocks the thread unnecessarily
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

auto future = client.request_async(req);
auto response = future.get(); // Blocks until complete
```

GOOD: Use async callbacks properly
```cpp
// GOOD: Use callbacks for non-blocking operations
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

client.request_async(req, [](hyper::Result<hyper::Response> result) {
    if (result.is_ok()) {
        auto response = result.unwrap();
        // Process response asynchronously
    }
});
```

BAD: Ignoring connection pool exhaustion
```cpp
// BAD: Creating new client for each request
for (int i = 0; i < 1000; ++i) {
    hyper::client::Client client;
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    auto response = client.request(req).get();
}
```

GOOD: Reuse client instance
```cpp
// GOOD: Single client with connection pooling
hyper::client::Client client;
for (int i = 0; i < 1000; ++i) {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    auto response = client.request(req).get();
}
```

BAD: Not handling timeouts
```cpp
// BAD: No timeout configuration
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://slow-server.com")
    .build();

auto response = client.request(req).get(); // May hang forever
```

GOOD: Configure timeouts
```cpp
// GOOD: Set appropriate timeouts
hyper::client::Config config;
config.set_connect_timeout(std::chrono::seconds(10));
config.set_read_timeout(std::chrono::seconds(30));

hyper::client::Client client(config);
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://slow-server.com")
    .build();

auto response = client.request(req).get();
```

BAD: Memory leak with large response bodies
```cpp
// BAD: Reading entire body into memory
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com/large-file")
    .build();

auto response = client.request(req).get();
auto body = response.into_body();
std::string all_data = body.to_string(); // May OOM
```

GOOD: Stream large responses
```cpp
// GOOD: Stream body in chunks
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com/large-file")
    .build();

auto response = client.request(req).get();
auto body = response.into_body();

while (auto chunk = body.data()) {
    process_chunk(chunk->as_bytes());
}
```
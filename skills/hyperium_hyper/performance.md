# Performance

Connection pooling reduces overhead:
```cpp
#include <hyper/client.hpp>

// BAD: Creating new connections for each request
hyper::client::Client client;
for (int i = 0; i < 100; ++i) {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    auto response = client.request(req).get();
    // Each request creates a new connection
}

// GOOD: Reuse connections with pooling
hyper::client::Config config;
config.set_keep_alive(true);
config.set_max_connections_per_host(10);

hyper::client::Client pooled_client(config);
for (int i = 0; i < 100; ++i) {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    auto response = pooled_client.request(req).get();
    // Reuses existing connections
}
```

Streaming reduces memory allocation:
```cpp
#include <hyper/client.hpp>

// BAD: Allocating large buffers
hyper::client::Client client;
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com/large-file")
    .build();

auto response = client.request(req).get();
std::vector<char> buffer(1024 * 1024); // 1MB allocation
response.into_body().read(buffer.data(), buffer.size());

// GOOD: Streaming without large allocations
auto body = response.into_body();
while (auto chunk = body.data()) {
    process_chunk(chunk->as_bytes()); // Process in small chunks
}
```

Batch requests for throughput:
```cpp
#include <hyper/client.hpp>
#include <vector>
#include <future>

hyper::client::Client client;

std::vector<std::future<hyper::Response>> futures;
for (const auto& url : urls) {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri(url)
        .build();
    
    futures.push_back(client.request_async(req));
}

// Process responses as they complete
for (auto& future : futures) {
    auto response = future.get();
    process_response(response);
}
```

Optimize header size:
```cpp
#include <hyper/client.hpp>

// BAD: Large headers slow down parsing
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .header("X-Long-Header", std::string(10000, 'a')) // 10KB header
    .build();

// GOOD: Keep headers minimal
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .header("X-Short-Header", "value") // Small header
    .build();
```
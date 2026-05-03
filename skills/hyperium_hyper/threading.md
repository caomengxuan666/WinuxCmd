# Threading

Client instances are thread-safe for concurrent requests:
```cpp
#include <hyper/client.hpp>
#include <thread>
#include <vector>

hyper::client::Client client;

void worker_thread(int id) {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    
    auto response = client.request(req).get();
    // Process response safely
}

// Multiple threads can use the same client
std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back(worker_thread, i);
}

for (auto& t : threads) {
    t.join();
}
```

Request objects are NOT thread-safe:
```cpp
#include <hyper/client.hpp>
#include <thread>

hyper::client::Client client;

// BAD: Sharing request across threads
auto req = hyper::client::Request::builder()
    .method(hyper::Method::GET)
    .uri("http://example.com")
    .build();

std::thread t1([&]() {
    auto response = client.request(req).get(); // RACE CONDITION
});

std::thread t2([&]() {
    req.set_header("X-Custom", "value"); // RACE CONDITION
});

t1.join();
t2.join();

// GOOD: Each thread creates its own request
std::thread t1([&client]() {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    auto response = client.request(req).get();
});

std::thread t2([&client]() {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    auto response = client.request(req).get();
});

t1.join();
t2.join();
```

Use thread-local clients for maximum performance:
```cpp
#include <hyper/client.hpp>
#include <thread>
#include <vector>

thread_local hyper::client::Client tls_client;

void worker_thread() {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    
    auto response = tls_client.request(req).get();
    // No synchronization needed
}

std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back(worker_thread);
}

for (auto& t : threads) {
    t.join();
}
```

Async callbacks execute on internal thread pool:
```cpp
#include <hyper/client.hpp>
#include <mutex>

hyper::client::Client client;
std::mutex mutex;

void make_async_requests() {
    auto req = hyper::client::Request::builder()
        .method(hyper::Method::GET)
        .uri("http://example.com")
        .build();
    
    // Callback may execute on any thread
    client.request_async(req, [](hyper::Result<hyper::Response> result) {
        std::lock_guard<std::mutex> lock(mutex); // Must synchronize
        if (result.is_ok()) {
            auto response = result.unwrap();
            // Process response
        }
    });
}
```
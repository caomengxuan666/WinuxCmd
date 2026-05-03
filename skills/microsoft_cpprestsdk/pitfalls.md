# Pitfalls

```cpp
// BAD: Blocking on async operations can cause deadlocks
http_client client(U("https://api.example.com"));
auto response = client.request(methods::GET, U("/data")).get(); // Deadlock risk
std::cout << response.extract_string().get() << std::endl;

// GOOD: Use .wait() or proper async chaining
http_client client(U("https://api.example.com"));
client.request(methods::GET, U("/data"))
    .then([](http_response response) {
        return response.extract_string();
    })
    .then([](std::string body) {
        std::cout << body << std::endl;
    })
    .wait();
```

```cpp
// BAD: Ignoring HTTP response status codes
http_client client(U("https://api.example.com"));
client.request(methods::GET, U("/data"))
    .then([](http_response response) {
        return response.extract_json(); // Might fail on error responses
    })
    .wait();

// GOOD: Always check status codes
http_client client(U("https://api.example.com"));
client.request(methods::GET, U("/data"))
    .then([](http_response response) {
        if (response.status_code() == status_codes::OK) {
            return response.extract_json();
        }
        throw std::runtime_error("HTTP error: " + 
            std::to_string(response.status_code()));
    })
    .wait();
```

```cpp
// BAD: Memory leak with shared pointers in lambda captures
auto stream = std::make_shared<concurrency::streams::ostream>();
// ... use stream in multiple lambdas without proper cleanup

// GOOD: Use proper resource management
auto stream = std::make_shared<concurrency::streams::ostream>();
auto cleanup = [stream]() { stream->close().wait(); };
// Ensure cleanup is called
```

```cpp
// BAD: Not handling WebSocket close properly
websocket_client client;
client.connect(U("wss://example.com")).wait();
// ... use connection
// Forgot to close - resource leak

// GOOD: Always close WebSocket connections
websocket_client client;
client.connect(U("wss://example.com"))
    .then([&client]() {
        // Use connection
        return client.close();
    })
    .wait();
```

```cpp
// BAD: Using .get() on tasks in UI threads
auto task = client.request(methods::GET, U("/data"));
auto response = task.get(); // Blocks UI thread

// GOOD: Use continuation tasks
client.request(methods::GET, U("/data"))
    .then([](http_response response) {
        // Handle response asynchronously
    });
```

```cpp
// BAD: Not handling JSON parsing exceptions
auto jsonStr = U("{\"invalid\": json}");
auto value = json::value::parse(jsonStr); // May throw

// GOOD: Use try-catch for JSON operations
try {
    auto jsonStr = U("{\"valid\": \"json\"}");
    auto value = json::value::parse(jsonStr);
} catch (const json::json_exception& e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
}
```
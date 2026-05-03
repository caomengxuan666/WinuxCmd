# Safety

```cpp
// RED LINE 1: NEVER call .wait() or .get() on a task from within a continuation of the same task chain
// BAD: Will cause deadlock
client.request(methods::GET, U("/data"))
    .then([&client]() {
        auto response = client.request(methods::GET, U("/other")).get(); // DEADLOCK
        return response;
    })
    .wait();

// GOOD: Use proper chaining
client.request(methods::GET, U("/data"))
    .then([&client]() {
        return client.request(methods::GET, U("/other"));
    })
    .then([](http_response response) {
        return response.extract_string();
    })
    .wait();
```

```cpp
// RED LINE 2: NEVER use http_listener after calling close() without reopening
// BAD: Undefined behavior
http_listener listener(U("http://localhost:8080"));
listener.open().wait();
listener.close().wait();
listener.support(methods::GET, [](http_request request) { }); // Using closed listener

// GOOD: Only use listener while open
listener.open().wait();
listener.support(methods::GET, [](http_request request) { });
// ... use listener
listener.close().wait();
```

```cpp
// RED LINE 3: NEVER ignore exceptions from async operations
// BAD: Silently swallows errors
client.request(methods::GET, U("/data"))
    .then([](pplx::task<http_response> previousTask) {
        try {
            auto response = previousTask.get();
        } catch (...) {
            // Empty catch - errors disappear
        }
    })
    .wait();

// GOOD: Handle or propagate exceptions
client.request(methods::GET, U("/data"))
    .then([](pplx::task<http_response> previousTask) {
        auto response = previousTask.get(); // Exception propagates
        return response.extract_string();
    })
    .then([](pplx::task<std::string> previousTask) {
        try {
            auto body = previousTask.get();
            std::cout << body << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    })
    .wait();
```

```cpp
// RED LINE 4: NEVER share http_client instances across threads without synchronization
// BAD: Race condition
http_client client(U("https://api.example.com"));
std::thread t1([&client]() { client.request(methods::GET, U("/a")).wait(); });
std::thread t2([&client]() { client.request(methods::GET, U("/b")).wait(); });
t1.join();
t2.join();

// GOOD: Use separate clients or proper synchronization
http_client client1(U("https://api.example.com"));
http_client client2(U("https://api.example.com"));
std::thread t1([&client1]() { client1.request(methods::GET, U("/a")).wait(); });
std::thread t2([&client2]() { client2.request(methods::GET, U("/b")).wait(); });
t1.join();
t2.join();
```

```cpp
// RED LINE 5: NEVER use invalid URIs with http_client or http_listener
// BAD: Undefined behavior
http_client client(U("invalid uri with spaces")); // May crash

// GOOD: Validate URIs before use
auto uri = web::uri(U("https://api.example.com"));
if (uri.is_valid()) {
    http_client client(uri);
} else {
    throw std::invalid_argument("Invalid URI");
}
```
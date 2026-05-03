# Threading

```cpp
// http_client is NOT thread-safe for concurrent requests
// BAD: Concurrent access to same client
http_client client(U("https://api.example.com"));
std::thread t1([&client]() { client.request(methods::GET, U("/a")).wait(); });
std::thread t2([&client]() { client.request(methods::GET, U("/b")).wait(); });
t1.join();
t2.join(); // Undefined behavior

// GOOD: Use separate clients per thread
void workerThread(const std::string& baseUri) {
    http_client client(U(baseUri));
    client.request(methods::GET, U("/data")).wait();
}

std::thread t1(workerThread, "https://api.example.com");
std::thread t2(workerThread, "https://api.example.com");
t1.join();
t2.join();
```

```cpp
// Thread-safe pattern with mutex protection
class ThreadSafeClient {
private:
    http_client m_client;
    std::mutex m_mutex;
    
public:
    ThreadSafeClient(const std::string& baseUri) 
        : m_client(U(baseUri)) {}
    
    pplx::task<http_response> request(const method& mtd, 
        const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_client.request(mtd, U(path));
    }
};
```

```cpp
// PPL tasks handle thread scheduling automatically
http_client client(U("https://api.example.com"));

// Tasks run on internal thread pool
auto task1 = client.request(methods::GET, U("/data1"));
auto task2 = client.request(methods::GET, U("/data2"));

// when_all waits for all tasks to complete
pplx::when_all(task1, task2)
    .then([](std::vector<http_response> responses) {
        // This continuation runs on a thread pool thread
        for (auto& response : responses) {
            // Process responses
        }
    })
    .wait();
```

```cpp
// Use task schedulers for specific threading requirements
#include <pplx/pplxtasks.h>

http_client client(U("https://api.example.com"));

// Create a custom scheduler for I/O operations
auto scheduler = pplx::scheduler::make();
auto task = pplx::create_task([&client]() {
    return client.request(methods::GET, U("/data"));
}, scheduler);

task.then([](http_response response) {
    return response.extract_string();
}).wait();
```
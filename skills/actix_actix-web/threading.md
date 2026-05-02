# Threading

**Thread Safety Guarantees**

Actix Web uses an actor-based model where each worker runs in its own thread. The framework guarantees:
- Handler functions are called from a single worker thread at a time
- Shared state must be thread-safe if accessed from multiple handlers
- Middleware runs in the same thread as the handler

```cpp
// Thread-safe shared state using atomics
std::atomic<int> request_count{0};

int main() {
    actix_web::HttpServer server;
    server.route("/count", [](auto req) {
        request_count++;
        return actix_web::HttpResponse::Ok()
            .body("Requests: " + std::to_string(request_count.load()));
    });
    server.set_workers(4); // 4 worker threads
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Concurrent Access Patterns**

Use appropriate synchronization for shared resources:
```cpp
// Using mutex for complex shared state
std::mutex state_mutex;
std::map<std::string, int> user_scores;

int main() {
    actix_web::HttpServer server;
    server.route("/score/{user}", [](auto req) {
        std::string user = req.match_info().get("user");
        std::lock_guard<std::mutex> lock(state_mutex);
        user_scores[user]++;
        return actix_web::HttpResponse::Ok()
            .body("Score: " + std::to_string(user_scores[user]));
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Worker Thread Model**

Each worker thread handles multiple connections using async I/O:
```cpp
int main() {
    actix_web::HttpServer server;
    server.set_workers(4); // 4 worker threads
    
    // Each worker handles its own connections
    server.route("/", [](auto req) {
        // This runs on one of the worker threads
        auto thread_id = std::this_thread::get_id();
        return actix_web::HttpResponse::Ok()
            .body("Handled by thread: " + std::to_string(
                std::hash<std::thread::id>{}(thread_id)));
    });
    
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Avoiding Data Races**

Never assume handler execution order or thread affinity:
```cpp
// BAD: Assuming single-threaded execution
std::vector<int> unsafe_data;
server.route("/add", [](auto req) {
    unsafe_data.push_back(42); // Data race!
    return actix_web::HttpResponse::Ok().body("Added");
});

// GOOD: Thread-safe approach
std::mutex data_mutex;
std::vector<int> safe_data;
server.route("/add", [](auto req) {
    std::lock_guard<std::mutex> lock(data_mutex);
    safe_data.push_back(42);
    return actix_web::HttpResponse::Ok().body("Added");
});
```
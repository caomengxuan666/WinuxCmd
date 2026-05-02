# Pitfalls

**Pitfall 1: Blocking the Async Runtime**

BAD: Using blocking calls inside handlers
```cpp
server.route("/block", [](auto req) {
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Blocks the event loop
    return actix_web::HttpResponse::Ok().body("Done");
});
```

GOOD: Using async operations
```cpp
server.route("/async", [](auto req) {
    return actix_web::HttpResponse::Ok().body("Fast response");
});
```

**Pitfall 2: Incorrect Route Registration Order**

BAD: Registering specific routes after catch-all
```cpp
server.route("/{path}", [](auto req) { return ...; }); // Catch-all first
server.route("/users", [](auto req) { return ...; }); // Never reached
```

GOOD: Registering specific routes before catch-all
```cpp
server.route("/users", [](auto req) { return ...; }); // Specific first
server.route("/{path}", [](auto req) { return ...; }); // Catch-all last
```

**Pitfall 3: Memory Leaks with Shared State**

BAD: Capturing raw pointers in handlers
```cpp
int* counter = new int(0);
server.route("/count", [counter](auto req) {
    (*counter)++;
    return actix_web::HttpResponse::Ok().body(std::to_string(*counter));
}); // Memory leak!
```

GOOD: Using shared state management
```cpp
auto counter = std::make_shared<int>(0);
server.route("/count", [counter](auto req) {
    (*counter)++;
    return actix_web::HttpResponse::Ok().body(std::to_string(*counter));
});
```

**Pitfall 4: Not Handling Large Payloads**

BAD: Reading entire body into memory
```cpp
server.route("/upload", [](auto req) {
    std::string body = req.extract<std::string>(); // May crash with large files
    return actix_web::HttpResponse::Ok().body("Received");
});
```

GOOD: Streaming large payloads
```cpp
server.route("/upload", [](auto req) {
    auto stream = req.payload();
    // Process stream in chunks
    return actix_web::HttpResponse::Ok().body("Streaming received");
});
```

**Pitfall 5: Ignoring CORS Configuration**

BAD: No CORS headers for cross-origin requests
```cpp
server.route("/api/data", [](auto req) {
    return actix_web::HttpResponse::Ok().body("Data");
}); // Browser blocks cross-origin requests
```

GOOD: Proper CORS setup
```cpp
server.wrap(actix_web::middleware::Cors::permissive());
server.route("/api/data", [](auto req) {
    return actix_web::HttpResponse::Ok().body("Data");
});
```

**Pitfall 6: Not Setting Timeouts**

BAD: No timeout configuration
```cpp
server.route("/slow", [](auto req) {
    // Handler may hang indefinitely
    return actix_web::HttpResponse::Ok().body("Slow");
});
```

GOOD: Configuring timeouts
```cpp
server.set_client_timeout(std::chrono::seconds(30));
server.set_keep_alive(std::chrono::seconds(75));
server.route("/slow", [](auto req) {
    return actix_web::HttpResponse::Ok().body("Slow but timed");
});
```
# Safety

**Red Line 1: Never use blocking I/O in handlers**

BAD:
```cpp
server.route("/read-file", [](auto req) {
    std::ifstream file("data.txt"); // Blocking I/O
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return actix_web::HttpResponse::Ok().body(content);
});
```

GOOD:
```cpp
server.route("/read-file", [](auto req) {
    // Use async file operations or preload data
    return actix_web::HttpResponse::Ok().body("Use async I/O instead");
});
```

**Red Line 2: Never throw exceptions from handlers**

BAD:
```cpp
server.route("/unsafe", [](auto req) {
    throw std::runtime_error("Something went wrong"); // Crashes the server
    return actix_web::HttpResponse::Ok().body("Never reached");
});
```

GOOD:
```cpp
server.route("/safe", [](auto req) {
    try {
        // Risky operation
        return actix_web::HttpResponse::Ok().body("Success");
    } catch (...) {
        return actix_web::HttpResponse::InternalServerError().body("Error occurred");
    }
});
```

**Red Line 3: Never share mutable state without synchronization**

BAD:
```cpp
int global_counter = 0; // Unsynchronized mutable state
server.route("/increment", [](auto req) {
    global_counter++; // Data race!
    return actix_web::HttpResponse::Ok().body(std::to_string(global_counter));
});
```

GOOD:
```cpp
std::atomic<int> global_counter{0};
server.route("/increment", [](auto req) {
    global_counter++;
    return actix_web::HttpResponse::Ok().body(std::to_string(global_counter.load()));
});
```

**Red Line 4: Never ignore TLS/SSL errors in production**

BAD:
```cpp
server.bind("0.0.0.0:8080"); // No TLS in production
```

GOOD:
```cpp
server.bind_ssl("0.0.0.0:443", "cert.pem", "key.pem"); // Always use TLS in production
```

**Red Line 5: Never expose internal error details to clients**

BAD:
```cpp
server.route("/api", [](auto req) {
    try {
        // Some operation
    } catch (const std::exception& e) {
        return actix_web::HttpResponse::InternalServerError()
            .body(std::string("Error: ") + e.what()); // Exposes internals
    }
});
```

GOOD:
```cpp
server.route("/api", [](auto req) {
    try {
        // Some operation
    } catch (...) {
        return actix_web::HttpResponse::InternalServerError()
            .body("An internal error occurred"); // Safe message
    }
});
```
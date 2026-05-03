# Pitfalls

**Pitfall 1: Not validating path parameters**

BAD:
```cpp
app.route("/user/<id>", [](const flask::request& req) {
    std::string id = req.path_param("id");
    // Directly using id without validation
    return flask::response("User " + id);
});
```

GOOD:
```cpp
app.route("/user/<id>", [](const flask::request& req) {
    std::string id = req.path_param("id");
    if (id.empty() || !std::all_of(id.begin(), id.end(), ::isdigit)) {
        return flask::response("Invalid user ID", 400);
    }
    return flask::response("User " + id);
});
```

**Pitfall 2: Memory leaks with raw pointers in handlers**

BAD:
```cpp
app.route("/data", [](const flask::request& req) {
    char* buffer = new char[1024];
    // Fill buffer
    return flask::response(buffer); // Memory leak!
});
```

GOOD:
```cpp
app.route("/data", [](const flask::request& req) {
    std::string buffer(1024, '\0');
    // Fill buffer
    return flask::response(buffer); // Automatic cleanup
});
```

**Pitfall 3: Blocking the event loop**

BAD:
```cpp
app.route("/process", [](const flask::request& req) {
    std::this_thread::sleep_for(std::chrono::seconds(10)); // Blocks all requests
    return flask::response("Done");
});
```

GOOD:
```cpp
app.route("/process", [](const flask::request& req) {
    auto future = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return "Done";
    });
    return flask::response(future.get());
});
```

**Pitfall 4: Not handling CORS headers**

BAD:
```cpp
app.route("/api/data", [](const flask::request& req) {
    return flask::response("data");
});
```

GOOD:
```cpp
app.route("/api/data", [](const flask::request& req) {
    flask::response resp("data");
    resp.header("Access-Control-Allow-Origin", "*");
    resp.header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    return resp;
});
```

**Pitfall 5: Exposing sensitive information in error messages**

BAD:
```cpp
app.error_handler(500, [](const flask::request& req) {
    return flask::response("Database connection failed: " + get_db_error(), 500);
});
```

GOOD:
```cpp
app.error_handler(500, [](const flask::request& req) {
    log_error(get_db_error()); // Log internally
    return flask::response("Internal server error", 500);
});
```

**Pitfall 6: Not setting content type for binary responses**

BAD:
```cpp
app.route("/image", [](const flask::request& req) {
    std::vector<char> image_data = load_image();
    return flask::response(std::string(image_data.begin(), image_data.end()));
});
```

GOOD:
```cpp
app.route("/image", [](const flask::request& req) {
    std::vector<char> image_data = load_image();
    flask::response resp(std::string(image_data.begin(), image_data.end()));
    resp.content_type("image/png");
    return resp;
});
```
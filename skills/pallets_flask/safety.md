# Safety

**Red-line 1: NEVER use untrusted input in file paths**

BAD:
```cpp
app.route("/files/<path>", [](const flask::request& req) {
    std::string path = req.path_param("path");
    std::ifstream file(path); // Path traversal vulnerability!
    // ...
});
```

GOOD:
```cpp
app.route("/files/<path>", [](const flask::request& req) {
    std::string path = req.path_param("path");
    // Sanitize path
    if (path.find("..") != std::string::npos || path[0] == '/') {
        return flask::response("Forbidden", 403);
    }
    std::string safe_path = "./safe_directory/" + path;
    std::ifstream file(safe_path);
    // ...
});
```

**Red-line 2: NEVER store passwords or secrets in plaintext**

BAD:
```cpp
app.route("/login", [](const flask::request& req) {
    std::string password = req.form("password");
    if (password == "admin123") { // Hardcoded password!
        return flask::response("Logged in");
    }
    return flask::response("Failed", 401);
});
```

GOOD:
```cpp
app.route("/login", [](const flask::request& req) {
    std::string password = req.form("password");
    if (verify_password(password, get_stored_hash())) {
        return flask::response("Logged in");
    }
    return flask::response("Failed", 401);
});
```

**Red-line 3: NEVER ignore SSL/TLS for production**

BAD:
```cpp
app.run(8080); // HTTP only
```

GOOD:
```cpp
app.ssl_certificate("/etc/ssl/certs/server.crt");
app.ssl_key("/etc/ssl/private/server.key");
app.run(443); // HTTPS
```

**Red-line 4: NEVER allow unbounded request body sizes**

BAD:
```cpp
app.route("/upload", [](const flask::request& req) {
    std::string body = req.body(); // No size limit
    // Process body
});
```

GOOD:
```cpp
app.max_content_length(10 * 1024 * 1024); // 10MB limit

app.route("/upload", [](const flask::request& req) {
    if (req.content_length() > app.max_content_length()) {
        return flask::response("Request too large", 413);
    }
    std::string body = req.body();
    // Process body
});
```

**Red-line 5: NEVER expose internal IP addresses or server details**

BAD:
```cpp
app.error_handler(500, [](const flask::request& req) {
    return flask::response("Error on server 192.168.1.100", 500);
});
```

GOOD:
```cpp
app.error_handler(500, [](const flask::request& req) {
    log_error("Internal error occurred");
    return flask::response("An error occurred", 500);
});
```
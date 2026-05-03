# Safety

> 🔒 See [pitfalls.md](pitfalls.md) for common mistakes with BAD/GOOD examples.
> See [threading.md](threading.md) for thread safety guarantees.
> See [quickstart.md](quickstart.md) for safe setup instructions.

**RED LINE 1: Never use on 32-bit platforms**
```cpp
// BAD: Using on 32-bit platform
// This can cause integer truncation and security vulnerabilities
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

// GOOD: Only use on 64-bit platforms
static_assert(sizeof(void*) >= 8, "cpp-httplib requires 64-bit platform");
```

**RED LINE 2: Never disable SSL verification in production**
```cpp
// BAD: Disabling SSL verification
httplib::Client cli("https://production-api.com");
cli.enable_server_certificate_verification(false); // Security risk!

// GOOD: Proper SSL verification
httplib::Client cli("https://production-api.com");
cli.set_ca_cert_path("/etc/ssl/certs/ca-certificates.crt");
cli.enable_server_certificate_verification(true);
```

**RED LINE 3: Never use untrusted filenames without sanitization**
```cpp
// BAD: Path traversal vulnerability
svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
    auto file = req.get_file_value("file");
    std::ofstream ofs(file.filename); // Security risk!
});

// GOOD: Sanitize and validate filenames
svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
    auto file = req.get_file_value("file");
    std::string safe_name = sanitize_filename(file.filename);
    if (safe_name.empty()) {
        res.status = 400;
        return;
    }
    std::ofstream ofs("./uploads/" + safe_name, std::ios::binary);
});
```

**RED LINE 4: Never ignore SSL errors**
```cpp
// BAD: Ignoring SSL errors
httplib::Client cli("https://example.com");
auto res = cli.Get("/");
// Not checking SSL errors

// GOOD: Handle SSL errors properly
httplib::Client cli("https://example.com");
auto res = cli.Get("/");
if (!res) {
    switch (res.error()) {
        case httplib::Error::SSLConnection:
        case httplib::Error::SSLServerVerification:
            std::cerr << "SSL error occurred" << std::endl;
            break;
        default:
            break;
    }
}
```

**RED LINE 5: Never use without proper error handling for network operations**
```cpp
// BAD: No error handling
httplib::Server svr;
svr.listen("0.0.0.0", 8080); // No error check

// GOOD: Check return values
httplib::Server svr;
if (!svr.listen("0.0.0.0", 8080)) {
    std::cerr << "Failed to start server" << std::endl;
    return 1;
}
```

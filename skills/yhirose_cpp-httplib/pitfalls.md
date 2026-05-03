# Pitfalls

> ⚠️ See [safety.md](safety.md) for red-line conditions that must NEVER occur.
> See [quickstart.md](quickstart.md) for correct usage patterns.
> See [threading.md](threading.md) for thread safety pitfalls.
> See [best-practices.md](best-practices.md) for recommended patterns.

## 1. Not Checking Client Response

**BAD: Not checking client response**
```cpp
httplib::Client cli("http://example.com");
auto res = cli.Get("/");
// BAD: No null check before accessing res
std::cout << res->body << std::endl; // Crash if request failed
```

**GOOD: Always check response validity**
```cpp
httplib::Client cli("http://example.com");
if (auto res = cli.Get("/")) {
    std::cout << res->body << std::endl;
} else {
    std::cerr << "Request failed: " << httplib::to_string(res.error()) << std::endl;
}
```

## 2. Ignoring SSL Certificate Verification

**BAD: Ignoring SSL certificate verification**
```cpp
httplib::Client cli("https://example.com");
// BAD: Disabling verification without understanding risks
cli.enable_server_certificate_verification(false);
auto res = cli.Get("/");
```

**GOOD: Proper SSL certificate handling**
```cpp
httplib::Client cli("https://example.com");
cli.set_ca_cert_path("/etc/ssl/certs/ca-certificates.crt");
if (auto res = cli.Get("/")) {
    std::cout << res->body << std::endl;
}
```

## 3. Path Traversal via Unsanitized Filenames

**BAD: Using unvalidated file paths from client**
```cpp
svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
    auto file = req.get_file_value("file");
    // BAD: Path traversal vulnerability
    std::ofstream ofs("../uploads/" + file.filename);
    ofs << file.content;
});
```

**GOOD: Sanitize file paths**
```cpp
svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
    auto file = req.get_file_value("file");
    // Sanitize filename
    std::string safe_filename = file.filename;
    // Remove path separators
    safe_filename.erase(std::remove(safe_filename.begin(), safe_filename.end(), '/'), safe_filename.end());
    safe_filename.erase(std::remove(safe_filename.begin(), safe_filename.end(), '\\'), safe_filename.end());
    
    std::ofstream ofs("./uploads/" + safe_filename, std::ios::binary);
    ofs << file.content;
});
```

## 4. Not Handling SIGPIPE

When a client disconnects while the server is writing a response, the OS sends SIGPIPE to the server process. If unhandled, SIGPIPE kills the process by default — this is a common crash source in production HTTP servers.

**BAD: Not handling SIGPIPE**
```cpp
httplib::Client cli("http://example.com");
// BAD: No SIGPIPE handling, can crash on broken connections
auto res = cli.Get("/");
```

**GOOD: Handle SIGPIPE**
```cpp
#include <signal.h>

int main() {
    signal(SIGPIPE, SIG_IGN);
    
    httplib::Client cli("http://example.com");
    if (auto res = cli.Get("/")) {
        std::cout << res->body << std::endl;
    }
    return 0;
}
```

## 5. Missing Timeouts

**BAD: Not setting read timeout**
```cpp
httplib::Client cli("http://example.com");
// BAD: No timeout, can block forever on slow connections
auto res = cli.Get("/large-file");
```

**GOOD: Set appropriate timeouts**
```cpp
httplib::Client cli("http://example.com");
cli.set_read_timeout(5, 0);  // 5 seconds
cli.set_write_timeout(5, 0); // 5 seconds

if (auto res = cli.Get("/large-file")) {
    std::cout << res->body << std::endl;
}
```

# Quickstart

## Setup

cpp-httplib is a **single-file, header-only** library. To use it, just download [`httplib.h`](https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h) and include it in your project. No build system or compilation needed.

### Compilation

```bash
# Download the header
wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h

# Compile with C++11 or later, link pthread for server mode
g++ -std=c++11 -pthread -I. my_app.cpp -o my_app

# With OpenSSL support
g++ -std=c++11 -pthread -I. -DCPPHTTPLIB_OPENSSL_SUPPORT my_app.cpp -o my_app -lssl -lcrypto
```

### Basic HTTP Server

```cpp
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::Server svr;
    
    svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
    });
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}
```

> 📚 See [overview.md](overview.md) for when to use (and not use) this library.
> See [safety.md](safety.md) for critical security rules.

```cpp
// Basic HTTP Client
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::Client cli("http://example.com");
    
    if (auto res = cli.Get("/")) {
        std::cout << res->status << std::endl;
        std::cout << res->body << std::endl;
    }
    return 0;
}
```

```cpp
// HTTPS Server with SSL
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::SSLServer svr("./cert.pem", "./key.pem");
    
    svr.Get("/secure", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Secure content", "text/plain");
    });
    
    svr.listen("0.0.0.0", 443);
    return 0;
}
```

```cpp
// HTTPS Client with Certificate Verification
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::Client cli("https://api.example.com");
    cli.set_ca_cert_path("./ca-bundle.crt");
    
    if (auto res = cli.Get("/data")) {
        std::cout << res->body << std::endl;
    }
    return 0;
}
```

```cpp
// POST Request with JSON Body
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>

int main() {
    httplib::Client cli("http://localhost:3000");
    
    nlohmann::json json = {{"name", "John"}, {"age", 30}};
    auto body = json.dump();
    
    auto res = cli.Post("/api/users", body, "application/json");
    if (res) {
        std::cout << res->status << std::endl;
    }
    return 0;
}
```

```cpp
// Server with Query Parameters
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::Server svr;
    
    svr.Get("/search", [](const httplib::Request& req, httplib::Response& res) {
        auto q = req.get_param_value("q");
        auto page = req.get_param_value("page");
        
        res.set_content("Search: " + q + ", Page: " + page, "text/plain");
    });
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}
```

```cpp
// File Upload Server
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::Server svr;
    
    svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
        auto file = req.get_file_value("file");
        // IMPORTANT: file.filename is an untrusted value from the client
        std::ofstream ofs(file.filename, std::ios::binary);
        ofs << file.content;
        res.set_content("Uploaded", "text/plain");
    });
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}
```

> 🔍 For thread safety guarantees, see [threading.md](threading.md).
> For common mistakes, see [pitfalls.md](pitfalls.md).

```cpp
// Server with Error Handling
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int main() {
    httplib::Server svr;
    
    svr.Get("/data", [](const httplib::Request&, httplib::Response& res) {
        try {
            // Simulate data processing
            throw std::runtime_error("Processing failed");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(e.what(), "text/plain");
        }
    });
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}
```
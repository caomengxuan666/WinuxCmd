# Quickstart

```cpp
// Basic Hello World Server
#include <actix_web.hpp>
#include <iostream>

int main() {
    actix_web::HttpServer server;
    server.route("/", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Hello World!");
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// JSON Response Handler
#include <actix_web.hpp>
#include <nlohmann/json.hpp>

int main() {
    actix_web::HttpServer server;
    server.route("/api/data", [](auto req) {
        nlohmann::json json = {{"name", "John"}, {"age", 30}};
        return actix_web::HttpResponse::Ok()
            .content_type("application/json")
            .body(json.dump());
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// Path Parameters
#include <actix_web.hpp>

int main() {
    actix_web::HttpServer server;
    server.route("/users/{id}", [](auto req) {
        std::string user_id = req.match_info().get("id");
        return actix_web::HttpResponse::Ok()
            .body("User ID: " + user_id);
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// Query Parameters
#include <actix_web.hpp>

int main() {
    actix_web::HttpServer server;
    server.route("/search", [](auto req) {
        std::string query = req.query_string("q");
        return actix_web::HttpResponse::Ok()
            .body("Searching for: " + query);
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// POST Request with JSON Body
#include <actix_web.hpp>
#include <nlohmann/json.hpp>

int main() {
    actix_web::HttpServer server;
    server.route("/api/submit", actix_web::web::post(), [](auto req) {
        auto body = req.extract<std::string>();
        nlohmann::json json = nlohmann::json::parse(body);
        return actix_web::HttpResponse::Ok()
            .body("Received: " + json.dump());
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// Static File Serving
#include <actix_web.hpp>

int main() {
    actix_web::HttpServer server;
    server.service(actix_web::fs::Files::new("/static", "./public"));
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// Middleware Example (Logging)
#include <actix_web.hpp>

int main() {
    actix_web::HttpServer server;
    server.wrap(actix_web::middleware::Logger::default());
    server.route("/", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Hello with logging!");
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

```cpp
// Multiple Routes with Different Methods
#include <actix_web.hpp>

int main() {
    actix_web::HttpServer server;
    server.route("/", actix_web::web::get(), [](auto req) {
        return actix_web::HttpResponse::Ok().body("GET request");
    });
    server.route("/", actix_web::web::post(), [](auto req) {
        return actix_web::HttpResponse::Ok().body("POST request");
    });
    server.route("/", actix_web::web::put(), [](auto req) {
        return actix_web::HttpResponse::Ok().body("PUT request");
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```
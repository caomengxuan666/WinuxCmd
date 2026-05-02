# Lifecycle

**Construction**

The HttpServer is constructed and configured before binding:
```cpp
int main() {
    actix_web::HttpServer server; // Default construction
    server.set_workers(4); // Configure before binding
    server.set_backlog(1024);
    server.route("/", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Hello");
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Destruction**

The server shuts down gracefully when `run()` completes or when the application exits:
```cpp
int main() {
    actix_web::HttpServer server;
    server.route("/", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Hello");
    });
    server.bind("127.0.0.1:8080");
    server.run(); // Blocks until shutdown signal
    // Server resources are cleaned up here
    return 0;
}
```

**Move Semantics**

HttpServer supports move semantics for transferring ownership:
```cpp
actix_web::HttpServer create_server() {
    actix_web::HttpServer server;
    server.route("/", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Hello");
    });
    return server; // Move constructor called
}

int main() {
    auto server = create_server(); // Move assignment
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Resource Management**

Proper cleanup of resources using RAII:
```cpp
struct DatabaseConnection {
    DatabaseConnection(const std::string& conn_str) {
        // Open connection
    }
    ~DatabaseConnection() {
        // Close connection automatically
    }
};

int main() {
    auto db = std::make_shared<DatabaseConnection>("postgres://localhost/db");
    {
        actix_web::HttpServer server;
        server.app_data(db);
        server.route("/", [db](auto req) {
            return actix_web::HttpResponse::Ok().body("Using DB");
        });
        server.bind("127.0.0.1:8080");
        server.run();
    } // Server destroyed, db still alive if shared_ptr exists
    return 0;
}
```
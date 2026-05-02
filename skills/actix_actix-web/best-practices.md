# Best Practices

**Pattern 1: Application State Management**

Use shared state for database connections and configuration:
```cpp
struct AppState {
    std::shared_ptr<Database> db;
    std::shared_ptr<Config> config;
};

int main() {
    auto state = std::make_shared<AppState>();
    state->db = std::make_shared<Database>("connection_string");
    state->config = std::make_shared<Config>("config.json");
    
    actix_web::HttpServer server;
    server.app_data(state);
    server.route("/users", [state](auto req) {
        auto users = state->db->query("SELECT * FROM users");
        return actix_web::HttpResponse::Ok().body(users);
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Pattern 2: Middleware Composition**

Chain middleware for cross-cutting concerns:
```cpp
int main() {
    actix_web::HttpServer server;
    server.wrap(actix_web::middleware::Logger::default());
    server.wrap(actix_web::middleware::Compress::default());
    server.wrap(actix_web::middleware::Cors::permissive());
    server.route("/api/data", [](auto req) {
        return actix_web::HttpResponse::Ok().body("Data with middleware");
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Pattern 3: Error Handling with Custom Error Types**

Create a centralized error handling strategy:
```cpp
struct ApiError {
    int status_code;
    std::string message;
};

actix_web::HttpResponse error_response(const ApiError& err) {
    return actix_web::HttpResponse::builder()
        .status(err.status_code)
        .body(err.message);
}

int main() {
    actix_web::HttpServer server;
    server.route("/api/resource", [](auto req) {
        try {
            // Business logic
            return actix_web::HttpResponse::Ok().body("Success");
        } catch (const ApiError& err) {
            return error_response(err);
        }
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```

**Pattern 4: Request Validation**

Validate input before processing:
```cpp
bool validate_user_id(const std::string& id) {
    return !id.empty() && id.length() <= 36 && 
           std::all_of(id.begin(), id.end(), ::isalnum);
}

int main() {
    actix_web::HttpServer server;
    server.route("/users/{id}", [](auto req) {
        std::string user_id = req.match_info().get("id");
        if (!validate_user_id(user_id)) {
            return actix_web::HttpResponse::BadRequest()
                .body("Invalid user ID format");
        }
        return actix_web::HttpResponse::Ok()
            .body("Processing user: " + user_id);
    });
    server.bind("127.0.0.1:8080");
    server.run();
    return 0;
}
```
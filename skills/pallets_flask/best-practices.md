# Best Practices

**Use middleware for cross-cutting concerns**

```cpp
#include <flask/flask.hpp>
#include <chrono>

flask::app create_app_with_middleware() {
    flask::app app;
    
    // Request logging middleware
    app.before_request([](flask::request& req) {
        req.set_attribute("start_time", std::chrono::steady_clock::now());
    });
    
    app.after_request([](flask::request& req, flask::response& resp) {
        auto start = std::any_cast<std::chrono::steady_clock::time_point>(
            req.attribute("start_time"));
        auto duration = std::chrono::steady_clock::now() - start;
        log_request(req.method(), req.path(), resp.status_code(), duration);
        return resp;
    });
    
    return app;
}
```

**Organize routes in separate files**

```cpp
// routes/users.cpp
#include <flask/flask.hpp>

void register_user_routes(flask::app& app) {
    app.route("/users", [](const flask::request& req) {
        return flask::response("List users");
    });
    
    app.route("/users/<id>", [](const flask::request& req) {
        return flask::response("Get user " + req.path_param("id"));
    });
}

// main.cpp
#include "routes/users.cpp"

int main() {
    flask::app app;
    register_user_routes(app);
    app.run(8080);
}
```

**Use configuration files for environment-specific settings**

```cpp
#include <flask/flask.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

flask::app load_config(const std::string& env) {
    std::ifstream config_file("config/" + env + ".json");
    nlohmann::json config;
    config_file >> config;
    
    flask::app app;
    app.port(config["port"].get<int>());
    app.debug(config["debug"].get<bool>());
    app.max_content_length(config["max_content_length"].get<size_t>());
    
    return app;
}

int main(int argc, char* argv[]) {
    std::string env = argc > 1 ? argv[1] : "development";
    flask::app app = load_config(env);
    app.run();
}
```

**Implement proper error handling with custom exceptions**

```cpp
#include <flask/flask.hpp>
#include <stdexcept>

class NotFoundException : public std::runtime_error {
public:
    NotFoundException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ValidationException : public std::runtime_error {
public:
    ValidationException(const std::string& message) 
        : std::runtime_error(message) {}
};

flask::app create_robust_app() {
    flask::app app;
    
    app.error_handler(NotFoundException, [](const flask::request& req, const NotFoundException& e) {
        return flask::response(e.what(), 404);
    });
    
    app.error_handler(ValidationException, [](const flask::request& req, const ValidationException& e) {
        return flask::response(e.what(), 400);
    });
    
    return app;
}
```
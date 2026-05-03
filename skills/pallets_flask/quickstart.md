# Quickstart

```cpp
// Basic Flask application
#include <flask/flask.hpp>
#include <iostream>

int main() {
    flask::app app;
    
    app.route("/", [](const flask::request& req) {
        return flask::response("Hello, World!");
    });
    
    app.run(8080);
    return 0;
}
```

```cpp
// Route with path parameters
#include <flask/flask.hpp>

int main() {
    flask::app app;
    
    app.route("/user/<name>", [](const flask::request& req) {
        std::string name = req.path_param("name");
        return flask::response("Hello, " + name + "!");
    });
    
    app.run(8080);
    return 0;
}
```

```cpp
// JSON response
#include <flask/flask.hpp>
#include <nlohmann/json.hpp>

int main() {
    flask::app app;
    
    app.route("/api/data", [](const flask::request& req) {
        nlohmann::json data = {{"key", "value"}, {"number", 42}};
        return flask::response(data.dump(), "application/json");
    });
    
    app.run(8080);
    return 0;
}
```

```cpp
// POST request handling
#include <flask/flask.hpp>

int main() {
    flask::app app;
    
    app.route("/submit", flask::method::POST, [](const flask::request& req) {
        std::string body = req.body();
        // Process the body
        return flask::response("Received: " + body);
    });
    
    app.run(8080);
    return 0;
}
```

```cpp
// Static file serving
#include <flask/flask.hpp>

int main() {
    flask::app app;
    
    app.static_folder("/static", "./public");
    
    app.route("/", [](const flask::request& req) {
        return flask::response::file("./public/index.html");
    });
    
    app.run(8080);
    return 0;
}
```

```cpp
// Error handling
#include <flask/flask.hpp>

int main() {
    flask::app app;
    
    app.error_handler(404, [](const flask::request& req) {
        return flask::response("Page not found", 404);
    });
    
    app.error_handler(500, [](const flask::request& req) {
        return flask::response("Internal server error", 500);
    });
    
    app.run(8080);
    return 0;
}
```
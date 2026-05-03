# Overview

Flask is a lightweight web framework for C++ that provides a simple and intuitive API for building web applications and REST APIs. It is inspired by the Python Flask framework and follows a similar design philosophy of being minimal yet extensible.

**When to use Flask:**
- Building small to medium-sized web applications
- Creating REST APIs with minimal boilerplate
- Prototyping web services quickly
- Applications that need a simple routing system

**When not to use Flask:**
- High-performance production systems requiring maximum throughput
- Applications needing built-in authentication or database integration
- Systems requiring WebSocket support (use a dedicated library)
- Large-scale microservices architectures

**Key design principles:**
- Minimal core with optional extensions
- Decorator-style route registration
- Request/response cycle abstraction
- Middleware support for cross-cutting concerns

```cpp
// Basic structure
#include <flask/flask.hpp>

flask::app create_app() {
    flask::app app;
    app.route("/health", [](const flask::request& req) {
        return flask::response("OK");
    });
    return app;
}
```
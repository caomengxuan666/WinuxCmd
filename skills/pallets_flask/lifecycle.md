# Lifecycle

**Construction**

```cpp
#include <flask/flask.hpp>

// Default construction
flask::app app;

// Construction with configuration
flask::app app_with_config;
app_with_config.port(3000);
app_with_config.debug(true);
app_with_config.thread_pool_size(4);
```

**Resource management**

```cpp
#include <flask/flask.hpp>
#include <memory>

class DatabaseConnection {
public:
    DatabaseConnection(const std::string& conn_string) 
        : conn_(connect(conn_string)) {}
    ~DatabaseConnection() { disconnect(conn_); }
    
private:
    void* conn_;
};

flask::app create_app_with_resources() {
    flask::app app;
    
    // Store shared resources in app attributes
    auto db = std::make_shared<DatabaseConnection>("postgresql://localhost/mydb");
    app.set_attribute("database", db);
    
    // Cleanup on shutdown
    app.on_shutdown([db]() {
        // Database connection will be cleaned up when shared_ptr goes out of scope
    });
    
    return app;
}
```

**Move semantics**

```cpp
#include <flask/flask.hpp>

flask::app create_app() {
    flask::app app;
    app.route("/", [](const flask::request& req) {
        return flask::response("Hello");
    });
    return app; // Move is efficient
}

int main() {
    flask::app app = create_app(); // Move construction
    app.run(8080);
    
    // Move assignment
    flask::app app2;
    app2 = std::move(app); // app is now empty
    app2.run(8081);
}
```

**Destruction and cleanup**

```cpp
#include <flask/flask.hpp>

int main() {
    {
        flask::app app;
        app.route("/", [](const flask::request& req) {
            return flask::response("Hello");
        });
        app.run(8080);
        
        // When app goes out of scope:
        // 1. Server stops accepting new connections
        // 2. Active requests are completed
        // 3. Thread pool is shut down
        // 4. All resources are released
    } // app destructor called here
    
    return 0;
}
```
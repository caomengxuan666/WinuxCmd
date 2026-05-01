# Lifecycle

```cpp
// Proper construction of Drogon application
int main() {
    // Configure before calling run()
    app().setLogLevel(trantor::Logger::kDebug);
    app().setClientMaxBodySize(10 * 1024 * 1024); // 10MB
    app().setStaticFilesCacheTime(3600);
    
    // Register controllers and handlers
    app().registerController(std::make_shared<UserController>());
    
    // Start the server
    app().addListener("0.0.0.0", 8080);
    app().setThreadNum(4); // Number of I/O threads
    app().run(); // Blocks until quit() is called
    
    return 0;
}
```

```cpp
// Resource management with RAII
class DatabaseConnectionPool {
public:
    DatabaseConnectionPool() {
        // Connections are created lazily by Drogon
        dbClient_ = app().getDbClient("default");
        if (!dbClient_) {
            throw std::runtime_error("Failed to create database client");
        }
    }

    ~DatabaseConnectionPool() {
        // Drogon manages connection lifecycle
        // No explicit cleanup needed
    }

    drogon::orm::DbClientPtr getClient() {
        return dbClient_;
    }

private:
    drogon::orm::DbClientPtr dbClient_;
};
```

```cpp
// Move semantics with Drogon objects
class RequestHandler {
public:
    // Accept callback by value and move
    void handleRequest(HttpRequestPtr req,
                       std::function<void(HttpResponsePtr)> callback) {
        // Move capture in lambda for efficiency
        app().getLoop()->queueInLoop(
            [req = std::move(req), 
             callback = std::move(callback)]() {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody(req->getBody());
                callback(std::move(resp));
            });
    }
};
```

```cpp
// Proper cleanup of resources
class WebSocketManager {
public:
    void addConnection(const WebSocketConnectionPtr& conn) {
        connections_.push_back(conn);
        
        // Register cleanup on connection close
        conn->setCloseCallback([this](const WebSocketConnectionPtr& closed) {
            connections_.erase(
                std::remove(connections_.begin(), connections_.end(), closed),
                connections_.end());
        });
    }

    ~WebSocketManager() {
        // Close all connections on destruction
        for (auto& conn : connections_) {
            conn->shutdown();
        }
        connections_.clear();
    }

private:
    std::vector<WebSocketConnectionPtr> connections_;
};
```
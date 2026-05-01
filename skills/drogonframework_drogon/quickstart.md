# Quickstart

```cpp
// Basic HTTP server with a simple controller
#include <drogon/drogon.h>
using namespace drogon;

int main() {
    // Register a simple route handler
    app().registerHandler("/hello",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("Hello, World!");
            callback(resp);
        },
        {Get});

    // Start server on port 8080
    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// JSON API endpoint with automatic serialization
#include <drogon/drogon.h>
using namespace drogon;

int main() {
    app().registerHandler("/api/user/{id}",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback,
           int id) {
            Json::Value json;
            json["user_id"] = id;
            json["name"] = "John Doe";
            
            auto resp = HttpResponse::newHttpJsonResponse(json);
            callback(resp);
        },
        {Get});

    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// Using a controller class with Drogon's AOP
#include <drogon/drogon.h>
using namespace drogon;

class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::getUser, "/user/{id}", Get);
        ADD_METHOD_TO(UserController::createUser, "/user", Post);
    METHOD_LIST_END

    void getUser(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback,
                 int id) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("User ID: " + std::to_string(id));
        callback(resp);
    }

    void createUser(const HttpRequestPtr& req,
                    std::function<void(const HttpResponsePtr&)>&& callback) {
        auto json = req->getJsonObject();
        auto resp = HttpResponse::newHttpJsonResponse(*json);
        resp->setStatusCode(k201Created);
        callback(resp);
    }
};

int main() {
    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// WebSocket endpoint
#include <drogon/drogon.h>
using namespace drogon;

class WebSocketChat : public drogon::WebSocketController<WebSocketChat> {
public:
    void handleNewMessage(const WebSocketConnectionPtr& wsConnPtr,
                          std::string&& message,
                          const WebSocketMessageType& type) override {
        // Broadcast message to all connections
        app().getLoop()->queueInLoop([msg = std::move(message)]() {
            for (auto& conn : app().getWebSocketConnections()) {
                conn->send(msg);
            }
        });
    }

    void handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr) override {
        LOG_INFO << "WebSocket connection closed";
    }
};

int main() {
    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// Database integration with ORM
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
using namespace drogon;
using namespace drogon::orm;

int main() {
    // Configure database
    app().createDbClient("postgresql", "localhost", 5432, "mydb", "user", "pass", 10);

    app().registerHandler("/users",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            auto db = app().getDbClient();
            auto f = db->execSqlCoro("SELECT * FROM users");
            
            f.then([callback](const Result& result) {
                Json::Value users(Json::arrayValue);
                for (auto& row : result) {
                    Json::Value user;
                    user["id"] = row["id"].as<int>();
                    user["name"] = row["name"].as<std::string>();
                    users.append(user);
                }
                auto resp = HttpResponse::newHttpJsonResponse(users);
                callback(resp);
            }).fail([callback](const DrogonDbException& e) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            });
        },
        {Get});

    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// Middleware for authentication
#include <drogon/drogon.h>
using namespace drogon;

class AuthMiddleware : public drogon::HttpMiddleware<AuthMiddleware> {
public:
    void invoke(const HttpRequestPtr& req,
                MiddlewareNextCallback&& nextCb,
                MiddlewareCallback&& mcb) override {
        auto token = req->getHeader("Authorization");
        if (token.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k401Unauthorized);
            mcb(resp);
            return;
        }
        // Validate token...
        nextCb();
    }
};

int main() {
    app().registerController(std::make_shared<AuthMiddleware>());
    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// File upload handling
#include <drogon/drogon.h>
using namespace drogon;

int main() {
    app().registerHandler("/upload",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            auto& uploads = req->getParameters();
            auto& files = req->getUploadFiles();
            
            for (auto& file : files) {
                file.saveAs("./uploads/" + file.getFileName());
            }
            
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("Upload successful");
            callback(resp);
        },
        {Post});

    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```

```cpp
// Static file serving and custom 404
#include <drogon/drogon.h>
using namespace drogon;

int main() {
    // Serve static files from ./public directory
    app().setStaticFilesPath("./public");
    app().setStaticFilesCacheTime(3600);

    // Custom 404 handler
    app().setCustom404Page([](const HttpRequestPtr& req) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k404NotFound);
        resp->setBody("<h1>Page Not Found</h1>");
        return resp;
    });

    app().addListener("0.0.0.0", 8080).run();
    return 0;
}
```
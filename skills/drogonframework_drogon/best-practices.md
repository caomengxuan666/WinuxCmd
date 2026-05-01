# Best Practices

```cpp
// Use dependency injection for testable controllers
class UserService {
public:
    virtual ~UserService() = default;
    virtual std::string getUserName(int id) = 0;
};

class UserController : public drogon::HttpController<UserController> {
public:
    explicit UserController(std::shared_ptr<UserService> service) 
        : service_(std::move(service)) {}

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::getUser, "/user/{id}", Get);
    METHOD_LIST_END

    void getUser(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback,
                 int id) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(service_->getUserName(id));
        callback(resp);
    }

private:
    std::shared_ptr<UserService> service_;
};
```

```cpp
// Implement proper error handling middleware
class ErrorHandler : public drogon::HttpMiddleware<ErrorHandler> {
public:
    void invoke(const HttpRequestPtr& req,
                MiddlewareNextCallback&& nextCb,
                MiddlewareCallback&& mcb) override {
        nextCb([mcb = std::move(mcb)](const HttpResponsePtr& resp) {
            if (resp->getStatusCode() >= 500) {
                LOG_ERROR << "Server error: " << resp->getStatusCode();
                // Log to monitoring system
            }
            mcb(resp);
        });
    }
};
```

```cpp
// Use connection pooling for database access
class DatabaseService {
public:
    DatabaseService() {
        // Configure connection pool with optimal size
        dbClient_ = app().getDbClient();
    }

    drogon::Task<std::vector<User>> getUsers() {
        auto result = co_await dbClient_->execSqlCoro("SELECT * FROM users");
        std::vector<User> users;
        for (auto& row : result) {
            users.push_back(User{
                .id = row["id"].as<int>(),
                .name = row["name"].as<std::string>()
            });
        }
        co_return users;
    }

private:
    drogon::orm::DbClientPtr dbClient_;
};
```

```cpp
// Implement request validation middleware
class ValidationMiddleware : public drogon::HttpMiddleware<ValidationMiddleware> {
public:
    void invoke(const HttpRequestPtr& req,
                MiddlewareNextCallback&& nextCb,
                MiddlewareCallback&& mcb) override {
        // Validate content type for POST requests
        if (req->getMethod() == Post) {
            auto contentType = req->getContentType();
            if (contentType != CT_APPLICATION_JSON) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k415UnsupportedMediaType);
                mcb(resp);
                return;
            }
        }
        nextCb();
    }
};
```

```cpp
// Use structured logging with correlation IDs
class LoggingMiddleware : public drogon::HttpMiddleware<LoggingMiddleware> {
public:
    void invoke(const HttpRequestPtr& req,
                MiddlewareNextCallback&& nextCb,
                MiddlewareCallback&& mcb) override {
        auto correlationId = drogon::utils::getUuid();
        req->setParameter("correlation_id", correlationId);
        
        LOG_INFO << "[" << correlationId << "] Request: " 
                 << req->getMethodString() << " " << req->getPath();
        
        auto start = std::chrono::steady_clock::now();
        nextCb([mcb = std::move(mcb), start, correlationId](const HttpResponsePtr& resp) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count();
            LOG_INFO << "[" << correlationId << "] Response: " 
                     << resp->getStatusCode() << " (" << duration << "ms)";
            mcb(resp);
        });
    }
};
```

```cpp
// Implement graceful shutdown
int main() {
    app().registerHandler("/shutdown",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("Shutting down...");
            callback(resp);
            
            // Schedule shutdown after response is sent
            app().getLoop()->queueInLoop([]() {
                app().quit();
            });
        },
        {Post});

    app().addListener("0.0.0.0", 8080);
    app().setIdleConnectionTimeout(30); // Close idle connections
    app().run();
}
```
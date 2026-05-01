# Pitfalls

```cpp
// BAD: Blocking the event loop with synchronous operations
app().registerHandler("/slow",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Blocks all requests!
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("Done");
        callback(resp);
    },
    {Get});

// GOOD: Using asynchronous operations
app().registerHandler("/fast",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto db = app().getDbClient();
        db->execSqlAsync("SELECT pg_sleep(5)", 
            [callback](const Result& result) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody("Done");
                callback(resp);
            },
            [callback](const DrogonDbException& e) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            });
    },
    {Get});
```

```cpp
// BAD: Capturing shared_ptr by reference in lambda
auto sharedData = std::make_shared<std::string>("data");
app().registerHandler("/bad",
    [&sharedData](const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback) {
        // sharedData may be destroyed before callback executes!
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(*sharedData);
        callback(resp);
    },
    {Get});

// GOOD: Capturing by value to extend lifetime
auto sharedData = std::make_shared<std::string>("data");
app().registerHandler("/good",
    [sharedData](const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(*sharedData);
        callback(resp);
    },
    {Get});
```

```cpp
// BAD: Not handling callback exceptions
app().registerHandler("/crash",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto json = req->getJsonObject();
        // If json is null, this throws!
        auto resp = HttpResponse::newHttpJsonResponse(*json);
        callback(resp);
    },
    {Post});

// GOOD: Proper null checking
app().registerHandler("/safe",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto json = req->getJsonObject();
        if (!json) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Invalid JSON");
            callback(resp);
            return;
        }
        auto resp = HttpResponse::newHttpJsonResponse(*json);
        callback(resp);
    },
    {Post});
```

```cpp
// BAD: Calling callback multiple times
app().registerHandler("/double",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto resp = HttpResponse::newHttpResponse();
        callback(resp);
        callback(resp); // Undefined behavior!
    },
    {Get});

// GOOD: Ensuring callback is called exactly once
app().registerHandler("/single",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto resp = HttpResponse::newHttpResponse();
        callback(resp);
        // No second call
    },
    {Get});
```

```cpp
// BAD: Using raw pointers for long-lived resources
app().registerHandler("/leak",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto* data = new std::string("leaked"); // Never freed!
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(*data);
        callback(resp);
    },
    {Get});

// GOOD: Using RAII for resource management
app().registerHandler("/safe",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto data = std::make_shared<std::string>("safe");
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(*data);
        callback(resp);
    },
    {Get});
```

```cpp
// BAD: Ignoring thread safety in shared state
std::unordered_map<int, std::string> cache;
app().registerHandler("/bad-cache",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        // Multiple threads can access cache simultaneously!
        cache[1] = "value";
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(cache[1]);
        callback(resp);
    },
    {Get});

// GOOD: Using thread-safe access through event loop
app().registerHandler("/good-cache",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        app().getLoop()->queueInLoop([callback]() {
            static std::unordered_map<int, std::string> cache;
            cache[1] = "value";
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(cache[1]);
            callback(resp);
        });
    },
    {Get});
```

```cpp
// BAD: Not setting content type for binary data
app().registerHandler("/binary-bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        std::vector<char> data = {0x00, 0x01, 0x02};
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(std::string(data.begin(), data.end()));
        callback(resp); // Client may misinterpret data
    },
    {Get});

// GOOD: Setting proper content type
app().registerHandler("/binary-good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        std::vector<char> data = {0x00, 0x01, 0x02};
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(std::string(data.begin(), data.end()));
        resp->setContentTypeCode(CT_APPLICATION_OCTET_STREAM);
        callback(resp);
    },
    {Get});
```
# Performance

```cpp
// BAD: Creating new HttpResponse objects unnecessarily
app().registerHandler("/bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("Hello");
        callback(resp);
    },
    {Get});

// GOOD: Reuse response objects when possible
static const auto cachedResponse = []() {
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("Hello");
    return resp;
}();

app().registerHandler("/good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        callback(cachedResponse); // Zero allocation
    },
    {Get});
```

```cpp
// BAD: String concatenation in hot path
app().registerHandler("/bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        std::string result;
        for (int i = 0; i < 1000; ++i) {
            result += std::to_string(i) + ","; // Many allocations
        }
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(result);
        callback(resp);
    },
    {Get});

// GOOD: Pre-allocate and use string stream
app().registerHandler("/good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        std::ostringstream ss;
        ss << "0";
        for (int i = 1; i < 1000; ++i) {
            ss << "," << i;
        }
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(ss.str());
        callback(resp);
    },
    {Get});
```

```cpp
// BAD: Synchronous database queries in handlers
app().registerHandler("/bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto db = app().getDbClient();
        auto result = db->execSqlSync("SELECT * FROM large_table"); // Blocks!
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(std::to_string(result.size()));
        callback(resp);
    },
    {Get});

// GOOD: Asynchronous database queries
app().registerHandler("/good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto db = app().getDbClient();
        db->execSqlAsync("SELECT * FROM large_table",
            [callback](const Result& result) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setBody(std::to_string(result.size()));
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
// BAD: Unnecessary JSON parsing
app().registerHandler("/bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto json = req->getJsonObject();
        // Parse again unnecessarily
        Json::Reader reader;
        Json::Value parsed;
        reader.parse(req->getBody(), parsed);
        auto resp = HttpResponse::newHttpJsonResponse(*json);
        callback(resp);
    },
    {Post});

// GOOD: Use parsed JSON directly
app().registerHandler("/good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        auto json = req->getJsonObject();
        if (!json) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        auto resp = HttpResponse::newHttpJsonResponse(*json);
        callback(resp);
    },
    {Post});
```
# Safety

```cpp
// RED LINE 1: NEVER call callback outside the event loop thread
// BAD: Calling callback from a different thread
std::thread t([callback = std::move(callback)]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto resp = HttpResponse::newHttpResponse();
    callback(resp); // CRASH: Not in event loop thread!
});
t.detach();

// GOOD: Always dispatch to event loop
app().getLoop()->queueInLoop([callback = std::move(callback)]() {
    auto resp = HttpResponse::newHttpResponse();
    callback(resp);
});
```

```cpp
// RED LINE 2: NEVER access request/response objects after callback is called
// BAD: Using request after callback
auto resp = HttpResponse::newHttpResponse();
callback(resp);
auto body = req->getBody(); // UNDEFINED BEHAVIOR: req may be destroyed

// GOOD: Extract all needed data before callback
auto body = req->getBody();
auto resp = HttpResponse::newHttpResponse();
resp->setBody(body);
callback(resp);
```

```cpp
// RED LINE 3: NEVER throw exceptions from handlers without catching
// BAD: Uncaught exception in handler
app().registerHandler("/bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        throw std::runtime_error("crash"); // Terminates process!
    },
    {Get});

// GOOD: Always catch exceptions
app().registerHandler("/good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        try {
            // Handler logic
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("OK");
            callback(resp);
        } catch (const std::exception& e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody(e.what());
            callback(resp);
        }
    },
    {Get});
```

```cpp
// RED LINE 4: NEVER use synchronous I/O in handlers
// BAD: Blocking file read
std::ifstream file("data.txt");
std::string content((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
// Blocks event loop for all connections!

// GOOD: Use asynchronous file operations or preload data
app().getLoop()->queueInLoop([callback]() {
    static std::string cachedContent = []() {
        std::ifstream file("data.txt");
        return std::string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }();
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody(cachedContent);
    callback(resp);
});
```

```cpp
// RED LINE 5: NEVER modify shared state without synchronization
// BAD: Unsynchronized counter
static int requestCount = 0;
app().registerHandler("/bad",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        requestCount++; // Data race!
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(std::to_string(requestCount));
        callback(resp);
    },
    {Get});

// GOOD: Thread-safe counter using atomic
static std::atomic<int> requestCount{0};
app().registerHandler("/good",
    [](const HttpRequestPtr& req,
       std::function<void(const HttpResponsePtr&)>&& callback) {
        requestCount.fetch_add(1, std::memory_order_relaxed);
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(std::to_string(requestCount.load()));
        callback(resp);
    },
    {Get});
```
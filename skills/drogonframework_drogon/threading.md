# Threading

```cpp
// Thread-safe handler registration
int main() {
    // All handlers must be registered before run()
    app().registerHandler("/safe",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            // This lambda runs on an I/O thread
            // Do not assume which thread
        },
        {Get});
    
    app().addListener("0.0.0.0", 8080);
    app().setThreadNum(4); // 4 I/O threads
    app().run();
}
```

```cpp
// Thread-safe access to shared state
class SharedState {
public:
    void update(const std::string& key, const std::string& value) {
        // Always dispatch to event loop
        app().getLoop()->queueInLoop([this, key, value]() {
            data_[key] = value;
        });
    }

    std::string get(const std::string& key) {
        std::string result;
        // Synchronous access from event loop
        app().getLoop()->queueInLoop([this, &key, &result]() {
            auto it = data_.find(key);
            if (it != data_.end()) {
                result = it->second;
            }
        });
        // Wait for completion (only for demonstration)
        while (result.empty() && !data_.empty()) {
            std::this_thread::yield();
        }
        return result;
    }

private:
    std::unordered_map<std::string, std::string> data_;
};
```

```cpp
// Thread-safe WebSocket broadcasting
class ChatRoom {
public:
    void broadcast(const std::string& message) {
        // Must be called from event loop
        app().getLoop()->assertInLoopThread();
        
        for (auto& conn : connections_) {
            if (conn->connected()) {
                conn->send(message);
            }
        }
    }

    void addConnection(const WebSocketConnectionPtr& conn) {
        app().getLoop()->queueInLoop([this, conn]() {
            connections_.push_back(conn);
        });
    }

private:
    std::vector<WebSocketConnectionPtr> connections_;
};
```

```cpp
// Thread-safe timer operations
class TimerExample {
public:
    void startPeriodicTask() {
        // Create timer on event loop
        timerId_ = app().getLoop()->runEvery(1.0, [this]() {
            // This runs on the event loop thread
            LOG_INFO << "Periodic task executed";
            processData();
        });
    }

    void stopPeriodicTask() {
        if (timerId_ != 0) {
            app().getLoop()->invalidateTimer(timerId_);
            timerId_ = 0;
        }
    }

private:
    void processData() {
        // Thread-safe: always on event loop
        static int counter = 0;
        counter++;
    }

    trantor::TimerId timerId_ = 0;
};
```
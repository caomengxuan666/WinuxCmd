# Best Practices

**Pattern 1: Connection Manager with RAII**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>

class ConnectionManager {
public:
    using server = websocketpp::server<websocketpp::config::asio>;
    
    ConnectionManager(server& srv) : m_server(srv) {}
    
    void add(websocketpp::connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.insert(hdl);
    }
    
    void remove(websocketpp::connection_hdl hdl) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connections.erase(hdl);
    }
    
    void broadcast(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& hdl : m_connections) {
            try {
                m_server.send(hdl, message, websocketpp::frame::opcode::text);
            } catch (const websocketpp::exception& e) {
                // Handle failed send
            }
        }
    }
    
    size_t count() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_connections.size();
    }

private:
    server& m_server;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> m_connections;
    std::mutex m_mutex;
};
```

**Pattern 2: Graceful Shutdown**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <atomic>
#include <csignal>

std::atomic<bool> shutdown_requested(false);
websocketpp::server<websocketpp::config::asio>* global_server = nullptr;

void signal_handler(int) {
    shutdown_requested = true;
    if (global_server) {
        global_server->stop_listening();
        global_server->stop();
    }
}

int main() {
    websocketpp::server<websocketpp::config::asio> server;
    global_server = &server;
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    server.set_open_handler([](websocketpp::connection_hdl hdl) {
        // Register connection
    });
    
    server.set_close_handler([](websocketpp::connection_hdl hdl) {
        // Cleanup connection
    });
    
    server.init_asio();
    server.listen(9002);
    server.start_accept();
    
    while (!shutdown_requested) {
        server.run_one();
    }
    
    // Graceful shutdown
    server.stop();
    return 0;
}
```

**Pattern 3: Message Queue with Backpressure**
```cpp
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.size() >= MAX_QUEUE_SIZE) {
            throw std::runtime_error("Queue full");
        }
        m_queue.push(std::move(item));
        m_cond.notify_one();
    }
    
    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() { return !m_queue.empty(); });
        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }
    
    size_t size() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    static constexpr size_t MAX_QUEUE_SIZE = 10000;
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};
```

**Pattern 4: Structured Logging**
```cpp
#include <websocketpp/logger/basic.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <spdlog/spdlog.h>

class CustomLogger : public websocketpp::log::basic {
public:
    CustomLogger() : websocketpp::log::basic() {}
    
    void write(websocketpp::log::level l, const std::string& msg) override {
        switch (l) {
            case websocketpp::log::elevel::fatal:
                spdlog::critical(msg);
                break;
            case websocketpp::log::elevel::error:
                spdlog::error(msg);
                break;
            case websocketpp::log::elevel::warn:
                spdlog::warn(msg);
                break;
            case websocketpp::log::elevel::info:
                spdlog::info(msg);
                break;
            default:
                spdlog::debug(msg);
        }
    }
};

// Usage:
struct CustomConfig : public websocketpp::config::asio {
    typedef CustomLogger elog_type;
    typedef CustomLogger alog_type;
};

websocketpp::server<CustomConfig> server;
```

**Pattern 5: Connection Rate Limiting**
```cpp
#include <chrono>
#include <unordered_map>

class RateLimiter {
public:
    bool allow(websocketpp::connection_hdl hdl) {
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto& entry = m_connections[hdl];
        if (now - entry.last_connect < std::chrono::seconds(1)) {
            entry.attempts++;
            if (entry.attempts > MAX_ATTEMPTS_PER_SECOND) {
                return false;
            }
        } else {
            entry.last_connect = now;
            entry.attempts = 1;
        }
        return true;
    }

private:
    struct RateEntry {
        std::chrono::steady_clock::time_point last_connect;
        int attempts = 0;
    };
    
    static constexpr int MAX_ATTEMPTS_PER_SECOND = 10;
    std::unordered_map<websocketpp::connection_hdl, RateEntry, 
                       std::hash<websocketpp::connection_hdl>> m_connections;
    std::mutex m_mutex;
};

// Usage in validate handler:
RateLimiter limiter;
server.set_validate_handler([&limiter](websocketpp::connection_hdl hdl) {
    return limiter.allow(hdl);
});
```
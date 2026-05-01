# Threading

**Thread Safety Guarantees**
WebSocket++ provides limited thread safety guarantees. The core event loop is not thread-safe by default, but the library supports multi-threaded operation with proper synchronization. Key points:

- **Endpoint methods**: NOT thread-safe (init_asio, listen, run)
- **Connection methods**: Thread-safe for send() when using single event loop
- **Handlers**: Called from event loop thread(s) only
- **Connection handles**: Safe to copy and store, but must be used with proper synchronization

**Single Event Loop with Worker Threads**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

websocketpp::server<websocketpp::config::asio> server;
std::queue<std::function<void()>> work_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;

// Worker thread pool
void worker_thread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, []() { return !work_queue.empty(); });
            task = std::move(work_queue.front());
            work_queue.pop();
        }
        task();
    }
}

// Message handler that offloads work
server.set_message_handler([](websocketpp::connection_hdl hdl, 
                               websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    work_queue.push([hdl, msg]() {
        // Heavy processing here
        std::string result = process_message(msg->get_payload());
        // Send result back through event loop
        server.send(hdl, result, websocketpp::frame::opcode::text);
    });
    queue_cv.notify_one();
});
```

**Multi-threaded Event Loop**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <vector>
#include <atomic>

websocketpp::server<websocketpp::config::asio> server;
std::atomic<bool> running(true);

void event_loop_thread() {
    while (running) {
        server.run_one(); // Process one event
    }
}

int main() {
    server.init_asio();
    server.set_reuse_addr(true);
    server.listen(9002);
    server.start_accept();
    
    // Start multiple event loop threads
    unsigned int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    
    for (unsigned int i = 0; i < thread_count; ++i) {
        threads.emplace_back(event_loop_thread);
    }
    
    // Wait for shutdown signal
    std::this_thread::sleep_for(std::chrono::hours(24));
    running = false;
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

**Thread-safe Connection Manager**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <shared_mutex>
#include <unordered_map>

class ThreadSafeConnectionManager {
public:
    using server = websocketpp::server<websocketpp::config::asio>;
    
    void add(websocketpp::connection_hdl hdl) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_connections.insert(hdl);
    }
    
    void remove(websocketpp::connection_hdl hdl) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_connections.erase(hdl);
    }
    
    void broadcast(server& srv, const std::string& message) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        for (auto& hdl : m_connections) {
            try {
                srv.send(hdl, message, websocketpp::frame::opcode::text);
            } catch (...) {
                // Handle send failure
            }
        }
    }
    
    size_t count() {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_connections.size();
    }

private:
    std::set<websocketpp::connection_hdl, 
             std::owner_less<websocketpp::connection_hdl>> m_connections;
    mutable std::shared_mutex m_mutex;
};
```

**Thread-safe Message Queue with Backpressure**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<typename T>
class ThreadSafeBoundedQueue {
public:
    explicit ThreadSafeBoundedQueue(size_t max_size) 
        : m_max_size(max_size), m_closed(false) {}
    
    bool push(T item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_closed || m_queue.size() >= m_max_size) {
            return false;
        }
        m_queue.push(std::move(item));
        m_not_empty.notify_one();
        return true;
    }
    
    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_not_empty.wait(lock, [this]() { 
            return !m_queue.empty() || m_closed; 
        });
        if (m_closed && m_queue.empty()) {
            throw std::runtime_error("Queue closed");
        }
        T item = std::move(m_queue.front());
        m_queue.pop();
        m_not_full.notify_one();
        return item;
    }
    
    void close() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_closed = true;
        m_not_empty.notify_all();
        m_not_full.notify_all();
    }

private:
    size_t m_max_size;
    bool m_closed;
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_not_empty;
    std::condition_variable m_not_full;
};

// Usage with WebSocket++
ThreadSafeBoundedQueue<std::pair<websocketpp::connection_hdl, std::string>> 
    message_queue(10000);

void producer_thread(websocketpp::server<websocketpp::config::asio>& srv) {
    srv.set_message_handler([](websocketpp::connection_hdl hdl, 
                                websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
        message_queue.push({hdl, msg->get_payload()});
    });
}

void consumer_thread(websocketpp::server<websocketpp::config::asio>& srv) {
    while (true) {
        try {
            auto [hdl, payload] = message_queue.pop();
            // Process message
            std::string response = process(payload);
            srv.send(hdl, response, websocketpp::frame::opcode::text);
        } catch (const std::runtime_error&) {
            break; // Queue closed
        }
    }
}
```
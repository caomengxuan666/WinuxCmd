# Performance

**Performance Characteristics**
WebSocket++ uses Boost.Asio for asynchronous I/O, providing excellent scalability for thousands of concurrent connections. The library is header-only, which eliminates compilation overhead but may increase build times. Key performance considerations include:

- **Memory per connection**: ~2-4KB for connection state, plus buffer allocations
- **Message overhead**: Each message frame adds 2-14 bytes of protocol overhead
- **Event loop**: Single-threaded by default, can scale with multiple threads
- **Buffer management**: Internal buffers grow dynamically; consider setting limits

**Allocation Patterns**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <vector>

websocketpp::server<websocketpp::config::asio> server;

// BAD: Frequent allocations in message handler
server.set_message_handler([](websocketpp::connection_hdl hdl, 
                               websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    std::string payload = msg->get_payload(); // Copy
    std::string response = "Echo: " + payload; // Another allocation
    server.send(hdl, response, websocketpp::frame::opcode::text);
});

// GOOD: Minimize allocations
server.set_message_handler([](websocketpp::connection_hdl hdl, 
                               websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    // Use string_view if available, or pass payload directly
    server.send(hdl, msg->get_payload(), msg->get_opcode());
});
```

**Buffer Management**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <vector>

// Configure buffer sizes for performance
struct PerformanceConfig : public websocketpp::config::asio {
    // Increase read buffer for large messages
    static const size_t connection_read_buffer_size = 65536; // 64KB
    
    // Set maximum message size
    static const size_t max_message_size = 1048576; // 1MB
    
    // Reduce logging overhead
    static const websocketpp::log::level con_access_level = 
        websocketpp::log::alevel::none;
    static const websocketpp::log::level con_error_level = 
        websocketpp::log::elevel::rerror;
};

websocketpp::server<PerformanceConfig> perf_server;

// Pre-allocate buffer pool
class BufferPool {
public:
    static constexpr size_t BUFFER_SIZE = 4096;
    
    std::vector<char> acquire() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pool.empty()) {
            return std::vector<char>(BUFFER_SIZE);
        }
        auto buf = std::move(m_pool.back());
        m_pool.pop_back();
        return buf;
    }
    
    void release(std::vector<char> buf) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pool.size() < MAX_POOL_SIZE) {
            buf.clear();
            m_pool.push_back(std::move(buf));
        }
    }

private:
    static constexpr size_t MAX_POOL_SIZE = 1000;
    std::vector<std::vector<char>> m_pool;
    std::mutex m_mutex;
};
```

**Optimization Tips**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <chrono>

websocketpp::server<websocketpp::config::asio> server;

// 1. Use binary frames for binary data
server.send(hdl, binary_data, websocketpp::frame::opcode::binary); // More efficient

// 2. Batch small messages
std::string batch;
for (int i = 0; i < 100; ++i) {
    batch += "message" + std::to_string(i) + "\n";
}
server.send(hdl, batch, websocketpp::frame::opcode::text); // Single frame

// 3. Use connection_hdl instead of connection_ptr
// connection_hdl is cheaper to copy and compare

// 4. Disable logging in production
struct ProductionConfig : public websocketpp::config::asio {
    static const websocketpp::log::level con_access_level = 
        websocketpp::log::alevel::none;
    static const websocketpp::log::level con_error_level = 
        websocketpp::log::elevel::none;
};

// 5. Set TCP_NODELAY for low latency
server.set_tcp_nodelay(true);

// 6. Use multiple acceptor threads
void run_multi_threaded(websocketpp::server<websocketpp::config::asio>& srv, 
                        unsigned int threads) {
    std::vector<std::thread> workers;
    for (unsigned int i = 0; i < threads; ++i) {
        workers.emplace_back([&srv]() {
            srv.run();
        });
    }
    for (auto& t : workers) {
        t.join();
    }
}
```

**Benchmarking Setup**
```cpp
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <chrono>
#include <atomic>

std::atomic<size_t> message_count(0);
std::atomic<size_t> byte_count(0);
auto start_time = std::chrono::steady_clock::now();

websocketpp::server<websocketpp::config::asio> bench_server;

bench_server.set_message_handler([](websocketpp::connection_hdl hdl, 
                                     websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
    message_count++;
    byte_count += msg->get_payload().size();
    
    // Report throughput every second
    static auto last_report = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (now - last_report > std::chrono::seconds(1)) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
        std::cout << "Messages: " << message_count.load() 
                  << ", Bytes: " << byte_count.load()
                  << ", Time: " << elapsed.count() << "s" << std::endl;
        last_report = now;
    }
    
    bench_server.send(hdl, msg->get_payload(), msg->get_opcode());
});
```
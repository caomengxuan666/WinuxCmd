# Performance

**Memory allocation patterns**

```cpp
// BAD: Frequent allocations in hot path
void handle_read(boost::asio::ip::tcp::socket& socket) {
    auto buffer = std::make_shared<std::vector<char>>(1024);
    socket.async_read_some(boost::asio::buffer(*buffer),
        [buffer](auto, auto) { /* use buffer */ });
}

// GOOD: Reuse buffers with memory pool
class Connection {
    static constexpr size_t BUFFER_SIZE = 65536;
    std::array<char, BUFFER_SIZE> read_buffer_;
    std::array<char, BUFFER_SIZE> write_buffer_;
    
    void do_read() {
        socket_.async_read_some(boost::asio::buffer(read_buffer_),
            [this](boost::system::error_code ec, size_t length) {
                if (!ec) process_read(length);
            });
    }
};
```

**Zero-copy operations with scatter/gather**

```cpp
// Use gather-write to avoid copying
std::vector<boost::asio::const_buffer> buffers;
buffers.push_back(boost::asio::buffer(header_));
buffers.push_back(boost::asio::buffer(payload_));
boost::asio::async_write(socket_, buffers, handler);
// No copying of data between buffers
```

**Performance tuning for high-throughput servers**

```cpp
class HighPerformanceServer {
    static constexpr size_t THREAD_COUNT = 4;
    static constexpr size_t BUFFER_SIZE = 65536;
    
    boost::asio::io_context io_;
    std::vector<std::thread> workers_;
    
    // Pre-allocate connection pool
    std::vector<std::unique_ptr<Connection>> connection_pool_;
    
public:
    HighPerformanceServer() {
        // Pre-allocate connections
        for (size_t i = 0; i < 1000; ++i) {
            connection_pool_.push_back(
                std::make_unique<Connection>(io_));
        }
        
        // Start thread pool
        for (size_t i = 0; i < THREAD_COUNT; ++i) {
            workers_.emplace_back([this]() { io_.run(); });
        }
    }
};
```

**Avoiding unnecessary copies in handlers**

```cpp
// BAD: Copies data into lambda
void send_data(const std::string& data) {
    socket_.async_write_some(boost::asio::buffer(data),
        [data](auto, auto) {}); // Copy of data
}

// GOOD: Move data into shared_ptr
void send_data(std::string data) {
    auto shared_data = std::make_shared<std::string>(std::move(data));
    socket_.async_write_some(boost::asio::buffer(*shared_data),
        [shared_data](auto, auto) {}); // No copy
}
```
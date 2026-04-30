# Pitfalls

**Pitfall 1: Using socket after move without reinitialization**

BAD:
```cpp
boost::asio::ip::tcp::socket sock(io);
boost::asio::ip::tcp::socket moved = std::move(sock);
sock.async_read_some(boost::asio::buffer(data), handler); // Undefined behavior!
```

GOOD:
```cpp
boost::asio::ip::tcp::socket sock(io);
boost::asio::ip::tcp::socket moved = std::move(sock);
// sock is now in moved-from state, must not be used
sock = boost::asio::ip::tcp::socket(io); // Reinitialize before use
sock.async_read_some(boost::asio::buffer(data), handler);
```

**Pitfall 2: Forgetting to keep io_context running**

BAD:
```cpp
boost::asio::io_context io;
boost::asio::steady_timer timer(io, std::chrono::seconds(1));
timer.async_wait([](auto) { std::cout << "Done"; });
// io.run() never called - timer never fires!
```

GOOD:
```cpp
boost::asio::io_context io;
boost::asio::steady_timer timer(io, std::chrono::seconds(1));
timer.async_wait([](auto) { std::cout << "Done"; });
io.run(); // Must call run() to process events
```

**Pitfall 3: Handler lifetime issues with shared_from_this**

BAD:
```cpp
class Session {
    boost::asio::ip::tcp::socket socket_;
public:
    void start() {
        socket_.async_read_some(boost::asio::buffer(data_),
            [this](auto, auto) { /* 'this' may be dangling */ });
    }
};
```

GOOD:
```cpp
class Session : public std::enable_shared_from_this<Session> {
    boost::asio::ip::tcp::socket socket_;
public:
    void start() {
        auto self = shared_from_this();
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](auto, auto) { /* Session kept alive */ });
    }
};
```

**Pitfall 4: Ignoring error codes in async operations**

BAD:
```cpp
socket_.async_read_some(boost::asio::buffer(data),
    [](boost::system::error_code, size_t) {
        // Ignoring error - may cause infinite loops
    });
```

GOOD:
```cpp
socket_.async_read_some(boost::asio::buffer(data),
    [this](boost::system::error_code ec, size_t length) {
        if (ec == boost::asio::error::eof) {
            // Connection closed cleanly
            return;
        }
        if (ec) {
            // Handle error appropriately
            return;
        }
        // Process data
    });
```

**Pitfall 5: Concurrent access to socket without strand**

BAD:
```cpp
boost::asio::ip::tcp::socket sock(io);
// Called from multiple threads
void write_data(const std::string& msg) {
    boost::asio::write(sock, boost::asio::buffer(msg)); // Race condition!
}
```

GOOD:
```cpp
boost::asio::ip::tcp::socket sock(io);
boost::asio::io_context::strand strand(io);

void write_data(const std::string& msg) {
    boost::asio::post(strand, [this, msg]() {
        boost::asio::write(sock, boost::asio::buffer(msg));
    });
}
```

**Pitfall 6: Using buffers after async operation starts**

BAD:
```cpp
std::string data = "Hello";
socket_.async_write_some(boost::asio::buffer(data), handler);
data = "World"; // Undefined behavior - buffer still in use!
```

GOOD:
```cpp
auto data = std::make_shared<std::string>("Hello");
socket_.async_write_some(boost::asio::buffer(*data), 
    [data](auto, auto) { /* data kept alive until completion */ });
```
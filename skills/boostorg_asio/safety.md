# Safety

**Red Line 1: Never use a moved-from socket without reinitialization**

BAD:
```cpp
boost::asio::ip::tcp::socket sock1(io);
auto sock2 = std::move(sock1);
sock1.async_read_some(boost::asio::buffer(data), handler); // CRASH
```

GOOD:
```cpp
boost::asio::ip::tcp::socket sock1(io);
auto sock2 = std::move(sock1);
sock1 = boost::asio::ip::tcp::socket(io); // Must reinitialize
sock1.async_read_some(boost::asio::buffer(data), handler);
```

**Red Line 2: Never destroy io_context while operations are pending**

BAD:
```cpp
auto io = std::make_unique<boost::asio::io_context>();
boost::asio::steady_timer timer(*io, std::chrono::seconds(5));
timer.async_wait([](auto) { std::cout << "Fired"; });
io.reset(); // Timer handler never called, potential crash
```

GOOD:
```cpp
auto io = std::make_unique<boost::asio::io_context>();
boost::asio::steady_timer timer(*io, std::chrono::seconds(5));
timer.async_wait([](auto) { std::cout << "Fired"; });
io->run(); // Process all handlers before destruction
io.reset(); // Safe now
```

**Red Line 3: Never access socket from multiple threads without strand**

BAD:
```cpp
boost::asio::ip::tcp::socket sock(io);
// Thread 1
std::thread t1([&]() { sock.async_read_some(boost::asio::buffer(d1), h1); });
// Thread 2 - concurrent access!
std::thread t2([&]() { sock.async_write_some(boost::asio::buffer(d2), h2); });
```

GOOD:
```cpp
boost::asio::ip::tcp::socket sock(io);
boost::asio::io_context::strand strand(io);
// All operations through strand
std::thread t1([&]() { 
    boost::asio::post(strand, [&]() { 
        sock.async_read_some(boost::asio::buffer(d1), h1); 
    });
});
std::thread t2([&]() { 
    boost::asio::post(strand, [&]() { 
        sock.async_write_some(boost::asio::buffer(d2), h2); 
    });
});
```

**Red Line 4: Never use stack-allocated buffers with async operations**

BAD:
```cpp
void send_message() {
    char buffer[1024];
    socket.async_write_some(boost::asio::buffer(buffer), 
        [](auto, auto) {}); // buffer destroyed when function returns!
}
```

GOOD:
```cpp
void send_message() {
    auto buffer = std::make_shared<std::array<char, 1024>>();
    socket.async_write_some(boost::asio::buffer(*buffer), 
        [buffer](auto, auto) {}); // buffer kept alive
}
```

**Red Line 5: Never ignore error codes in production code**

BAD:
```cpp
socket.async_read_some(boost::asio::buffer(data), 
    [](boost::system::error_code, size_t) {
        // Silent failure - dangerous!
    });
```

GOOD:
```cpp
socket.async_read_some(boost::asio::buffer(data), 
    [](boost::system::error_code ec, size_t length) {
        if (ec) {
            // Log error and handle appropriately
            std::cerr << "Read error: " << ec.message() << std::endl;
            return;
        }
        // Process data
    });
```
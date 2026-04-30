# Quickstart

```cpp
// Example 1: Synchronous TCP echo client
#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::asio::io_context io;
    boost::asio::ip::tcp::socket sock(io);
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string("127.0.0.1"), 8080);
    
    sock.connect(ep);
    
    std::string msg = "Hello, server!";
    boost::asio::write(sock, boost::asio::buffer(msg));
    
    char reply[1024];
    size_t len = sock.read_some(boost::asio::buffer(reply));
    std::cout << "Reply: " << std::string(reply, len) << std::endl;
    
    return 0;
}
```

```cpp
// Example 2: Asynchronous TCP echo server
#include <boost/asio.hpp>
#include <memory>

class Session : public std::enable_shared_from_this<Session> {
    boost::asio::ip::tcp::socket socket_;
    char data_[1024];
    
public:
    Session(boost::asio::ip::tcp::socket socket)
        : socket_(std::move(socket)) {}
    
    void start() { do_read(); }
    
private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](boost::system::error_code ec, size_t length) {
                if (!ec) do_write(length);
            });
    }
    
    void do_write(size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, size_t) {
                if (!ec) do_read();
            });
    }
};

int main() {
    boost::asio::io_context io;
    boost::asio::ip::tcp::acceptor acceptor(io, 
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    
    auto do_accept = [&]() {
        acceptor.async_accept(
            [&](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();
            });
    };
    do_accept();
    
    io.run();
    return 0;
}
```

```cpp
// Example 3: Timer with async_wait
#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::asio::io_context io;
    boost::asio::steady_timer timer(io, std::chrono::seconds(3));
    
    timer.async_wait([](boost::system::error_code ec) {
        if (!ec) std::cout << "Timer expired!" << std::endl;
    });
    
    io.run();
    return 0;
}
```

```cpp
// Example 4: UDP echo client
#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::asio::io_context io;
    boost::asio::ip::udp::socket sock(io, 
        boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
    
    boost::asio::ip::udp::resolver resolver(io);
    auto endpoint = *resolver.resolve("127.0.0.1", "8080").begin();
    
    std::string msg = "Hello, UDP!";
    sock.send_to(boost::asio::buffer(msg), endpoint);
    
    char reply[1024];
    boost::asio::ip::udp::endpoint sender;
    size_t len = sock.receive_from(boost::asio::buffer(reply), sender);
    std::cout << "Reply: " << std::string(reply, len) << std::endl;
    
    return 0;
}
```

```cpp
// Example 5: Strand for thread-safe async operations
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

int main() {
    boost::asio::io_context io;
    boost::asio::io_context::strand strand(io);
    
    // Post handlers through strand for serialized execution
    boost::asio::post(strand, []() {
        std::cout << "Handler 1 on thread " 
                  << std::this_thread::get_id() << std::endl;
    });
    
    boost::asio::post(strand, []() {
        std::cout << "Handler 2 on thread " 
                  << std::this_thread::get_id() << std::endl;
    });
    
    std::thread t1([&]() { io.run(); });
    std::thread t2([&]() { io.run(); });
    
    t1.join();
    t2.join();
    return 0;
}
```

```cpp
// Example 6: Coroutine with boost::asio::awaitable
#include <boost/asio.hpp>
#include <iostream>

boost::asio::awaitable<void> echo_session(
    boost::asio::ip::tcp::socket socket) {
    char data[1024];
    for (;;) {
        auto [ec, length] = co_await socket.async_read_some(
            boost::asio::buffer(data), boost::asio::as_tuple(boost::asio::use_awaitable));
        if (ec) break;
        co_await boost::asio::async_write(socket, 
            boost::asio::buffer(data, length), boost::asio::use_awaitable);
    }
}

int main() {
    boost::asio::io_context io;
    boost::asio::ip::tcp::acceptor acceptor(io, 
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    
    boost::asio::co_spawn(io, 
        [&]() -> boost::asio::awaitable<void> {
            for (;;) {
                auto socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
                boost::asio::co_spawn(io, echo_session(std::move(socket), 
                    boost::asio::detached));
            }
        }, boost::asio::detached);
    
    io.run();
    return 0;
}
```
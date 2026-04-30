# Lifecycle

**Construction and initialization**

```cpp
#include <boost/asio.hpp>

class NetworkService {
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    
public:
    NetworkService(uint16_t port)
        : acceptor_(io_, boost::asio::ip::tcp::endpoint(
              boost::asio::ip::tcp::v4(), port)),
          socket_(io_) {
        // All objects are fully constructed and ready
    }
    
    void start_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                // socket_ is now connected
                handle_connection();
            }
        });
    }
};
```

**Move semantics**

```cpp
class Connection {
    boost::asio::ip::tcp::socket socket_;
    
public:
    // Move constructor - socket is movable
    Connection(Connection&& other) noexcept
        : socket_(std::move(other.socket_)) {}
    
    // Move assignment
    Connection& operator=(Connection&& other) noexcept {
        if (this != &other) {
            socket_ = std::move(other.socket_);
        }
        return *this;
    }
    
    // After move, source is in valid but unspecified state
    void reset() {
        socket_ = boost::asio::ip::tcp::socket(
            socket_.get_executor());
    }
};
```

**Resource management with RAII**

```cpp
class Server {
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<class Session>> sessions_;
    
public:
    Server(uint16_t port) 
        : acceptor_(io_, boost::asio::ip::tcp::endpoint(
              boost::asio::ip::tcp::v4(), port)) {
        start_accept();
    }
    
    ~Server() {
        // RAII handles cleanup
        // acceptor_ closes automatically
        // io_context stops processing
        // All sessions are destroyed
    }
    
    void shutdown() {
        io_.stop(); // Graceful shutdown
        sessions_.clear();
    }
};
```

**Destruction order and cleanup**

```cpp
class SafeServer {
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<class Session>> sessions_;
    
public:
    ~SafeServer() {
        // 1. Stop accepting new connections
        boost::system::error_code ec;
        acceptor_.close(ec);
        
        // 2. Cancel all pending operations
        for (auto& session : sessions_) {
            session->cancel();
        }
        
        // 3. Run remaining handlers
        io_.poll();
        
        // 4. Clear sessions
        sessions_.clear();
    }
};
```
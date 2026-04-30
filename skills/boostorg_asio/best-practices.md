# Best Practices

**Pattern 1: Use shared_from_this for session lifetime management**

```cpp
class Session : public std::enable_shared_from_this<Session> {
    boost::asio::ip::tcp::socket socket_;
    std::array<char, 8192> buffer_;
    
public:
    Session(boost::asio::ip::tcp::socket socket) 
        : socket_(std::move(socket)) {}
    
    void start() {
        do_read();
    }
    
private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(boost::asio::buffer(buffer_),
            [this, self](boost::system::error_code ec, size_t length) {
                if (!ec) {
                    do_write(length);
                }
            });
    }
    
    void do_write(size_t length) {
        auto self = shared_from_this();
        boost::asio::async_write(socket_, 
            boost::asio::buffer(buffer_, length),
            [this, self](boost::system::error_code ec, size_t) {
                if (!ec) {
                    do_read();
                }
            });
    }
};
```

**Pattern 2: Strand-based thread pool for scalability**

```cpp
class ThreadPool {
    boost::asio::io_context io_;
    boost::asio::io_context::strand strand_;
    std::vector<std::thread> workers_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_;
    
public:
    ThreadPool(size_t num_threads) 
        : strand_(io_), work_(boost::asio::make_work_guard(io_)) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this]() { io_.run(); });
        }
    }
    
    template<typename F>
    void post(F&& f) {
        boost::asio::post(strand_, std::forward<F>(f));
    }
    
    ~ThreadPool() {
        work_.reset();
        for (auto& t : workers_) t.join();
    }
};
```

**Pattern 3: Error handling with error_code**

```cpp
class Connection {
    boost::asio::ip::tcp::socket socket_;
    
public:
    void async_read_with_error_handling() {
        auto buffer = std::make_shared<std::vector<char>>(1024);
        socket_.async_read_some(boost::asio::buffer(*buffer),
            [this, buffer](boost::system::error_code ec, size_t length) {
                if (ec == boost::asio::error::operation_aborted) {
                    return; // Operation cancelled intentionally
                }
                if (ec == boost::asio::error::eof) {
                    handle_disconnect();
                    return;
                }
                if (ec) {
                    log_error("Read failed: " + ec.message());
                    reconnect();
                    return;
                }
                process_data(buffer->data(), length);
            });
    }
};
```

**Pattern 4: Use steady_timer for timeouts**

```cpp
class TimeoutSession {
    boost::asio::ip::tcp::socket socket_;
    boost::asio::steady_timer timer_;
    std::chrono::seconds timeout_;
    
public:
    TimeoutSession(boost::asio::io_context& io, 
                   boost::asio::ip::tcp::socket socket,
                   std::chrono::seconds timeout)
        : socket_(std::move(socket)), timer_(io), timeout_(timeout) {}
    
    void start() {
        set_timeout();
        do_read();
    }
    
private:
    void set_timeout() {
        timer_.expires_after(timeout_);
        timer_.async_wait([this](boost::system::error_code ec) {
            if (!ec) {
                socket_.cancel(); // Timeout occurred
            }
        });
    }
    
    void do_read() {
        auto buffer = std::make_shared<std::array<char, 1024>>();
        socket_.async_read_some(boost::asio::buffer(*buffer),
            [this, buffer](boost::system::error_code ec, size_t length) {
                if (ec) return; // Handle error
                timer_.cancel(); // Cancel timeout
                process_data(buffer->data(), length);
            });
    }
};
```
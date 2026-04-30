# Threading

**Thread safety guarantees**

```cpp
// io_context::run() is thread-safe - multiple threads can call it
boost::asio::io_context io;

// Thread pool pattern
for (int i = 0; i < 4; ++i) {
    std::thread([&io]() { io.run(); }).detach();
}
```

**Strand usage for serialized execution**

```cpp
class ThreadSafeConnection {
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_context::strand strand_;
    
public:
    ThreadSafeConnection(boost::asio::io_context& io)
        : socket_(io), strand_(io) {}
    
    void async_write(const std::string& data) {
        // All write operations serialized through strand
        boost::asio::async_write(socket_, 
            boost::asio::buffer(data),
            boost::asio::bind_executor(strand_,
                [this](boost::system::error_code ec, size_t) {
                    if (!ec) handle_write_complete();
                }));
    }
    
    void async_read() {
        // All read operations serialized through strand
        socket_.async_read_some(boost::asio::buffer(buffer_),
            boost::asio::bind_executor(strand_,
                [this](boost::system::error_code ec, size_t length) {
                    if (!ec) handle_read(length);
                }));
    }
};
```

**Concurrent access patterns**

```cpp
class ConcurrentServer {
    boost::asio::io_context io_;
    std::map<int, std::shared_ptr<Session>> sessions_;
    std::mutex sessions_mutex_; // For non-Asio synchronization
    
public:
    void add_session(int id, std::shared_ptr<Session> session) {
        // External synchronization for shared state
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        sessions_[id] = session;
    }
    
    void broadcast(const std::string& message) {
        // Use strand for each session's operations
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        for (auto& [id, session] : sessions_) {
            session->async_write(message);
        }
    }
};
```

**Thread pool with work distribution**

```cpp
class WorkDistributor {
    boost::asio::io_context io_;
    boost::asio::io_context::strand strand_;
    std::vector<std::thread> workers_;
    
public:
    WorkDistributor(size_t num_threads) : strand_(io_) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this]() { io_.run(); });
        }
    }
    
    template<typename F>
    void post_work(F&& f) {
        // All work serialized through strand
        boost::asio::post(strand_, std::forward<F>(f));
    }
    
    template<typename F>
    void post_work_parallel(F&& f) {
        // Work can run in parallel (no strand)
        boost::asio::post(io_, std::forward<F>(f));
    }
    
    ~WorkDistributor() {
        io_.stop();
        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }
};
```
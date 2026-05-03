# Threading


### Thread safety guarantees
```cpp
// cppzmq sockets are NOT thread-safe
// Each socket must be used from a single thread

// BAD: Multiple threads accessing same socket
zmq::socket_t sock(ctx, zmq::socket_type::push);
std::thread t1([&]() { sock.send(zmq::str_buffer("a"), zmq::send_flags::none); });
std::thread t2([&]() { sock.send(zmq::str_buffer("b"), zmq::send_flags::none); });
t1.join(); t2.join(); // Race condition!

// GOOD: One socket per thread
zmq::context_t ctx;
std::thread t1([&]() {
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.send(zmq::str_buffer("a"), zmq::send_flags::none);
});
std::thread t2([&]() {
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.send(zmq::str_buffer("b"), zmq::send_flags::none);
});
t1.join(); t2.join();
```

### Context sharing across threads
```cpp
// Context is thread-safe, sockets are not
class ThreadPool {
    zmq::context_t ctx_;
    std::vector<std::thread> workers_;
    
public:
    void start_workers(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            workers_.emplace_back([this, i]() {
                // Each thread creates its own socket
                zmq::socket_t sock(ctx_, zmq::socket_type::rep);
                sock.bind("tcp://*:" + std::to_string(5555 + i));
                
                while (true) {
                    zmq::message_t msg;
                    sock.recv(msg, zmq::recv_flags::none);
                    sock.send(zmq::str_buffer("OK"), zmq::send_flags::none);
                }
            });
        }
    }
};
```

### Thread-safe message passing patterns
```cpp
// Thread-safe pattern: One socket per thread with inproc
class ThreadSafeWorker {
    zmq::context_t& ctx_;
    zmq::socket_t socket_;
    std::thread thread_;
    
public:
    ThreadSafeWorker(zmq::context_t& ctx, const std::string& endpoint)
        : ctx_(ctx)
        , socket_(ctx_, zmq::socket_type::pull)
    {
        socket_.connect(endpoint);
        thread_ = std::thread([this]() { run(); });
    }
    
    void run() {
        while (true) {
            zmq::message_t msg;
            socket_.recv(msg, zmq::recv_flags::none);
            // Process message
        }
    }
    
    ~ThreadSafeWorker() {
        thread_.detach(); // Or join with proper shutdown
    }
};
```

### Synchronization patterns
```cpp
// Using inproc for thread synchronization
class ThreadSync {
    zmq::context_t ctx_;
    zmq::socket_t sync_sock_;
    
public:
    ThreadSync() 
        : ctx_()
        , sync_sock_(ctx_, zmq::socket_type::pair)
    {
        sync_sock_.bind("inproc://sync");
    }
    
    void worker_thread() {
        zmq::socket_t sock(ctx_, zmq::socket_type::pair);
        sock.connect("inproc://sync");
        
        // Signal ready
        sock.send(zmq::str_buffer("READY"), zmq::send_flags::none);
        
        // Wait for start signal
        zmq::message_t msg;
        sock.recv(msg, zmq::recv_flags::none);
        
        // Start processing...
    }
    
    void start_workers() {
        std::thread worker([this]() { worker_thread(); });
        
        // Wait for worker to be ready
        zmq::message_t msg;
        sync_sock_.recv(msg, zmq::recv_flags::none);
        
        // Send start signal
        sync_sock_.send(zmq::str_buffer("START"), zmq::send_flags::none);
        
        worker.join();
    }
};
```

### Thread-safe context termination
```cpp
// Safe context termination with multiple threads
class SafeTermination {
    zmq::context_t ctx_;
    std::vector<std::thread> workers_;
    std::atomic<bool> running_{true};
    
public:
    void start() {
        for (int i = 0; i < 4; ++i) {
            workers_.emplace_back([this, i]() {
                zmq::socket_t sock(ctx_, zmq::socket_type::rep);
                sock.bind("tcp://*:" + std::to_string(5555 + i));
                
                while (running_) {
                    try {
                        zmq::message_t msg;
                        sock.recv(msg, zmq::recv_flags::none);
                        sock.send(zmq::str_buffer("OK"), zmq::send_flags::none);
                    } catch (const zmq::error_t& e) {
                        if (e.num() == ETERM) break; // Context terminated
                        throw;
                    }
                }
            });
        }
    }
    
    void shutdown() {
        running_ = false;
        ctx_.shutdown(); // Signal all sockets to terminate
        for (auto& t : workers_) {
            t.join();
        }
    }
};
```
# Best Practices


### Use RAII for automatic cleanup
```cpp
// Best practice: Let RAII handle cleanup
class MessageProcessor {
    zmq::context_t ctx_;
    zmq::socket_t socket_;
    
public:
    MessageProcessor() 
        : ctx_()
        , socket_(ctx_, zmq::socket_type::pull) 
    {
        socket_.bind("tcp://*:5555");
    }
    
    void process() {
        zmq::message_t msg;
        socket_.recv(msg, zmq::recv_flags::none);
        // Process message...
    }
    // Destructor automatically cleans up socket and context
};
```

### Implement proper error handling
```cpp
// Best practice: Consistent error handling
void safe_send(zmq::socket_t& sock, const std::string& data) {
    try {
        auto result = sock.send(zmq::str_buffer(data), zmq::send_flags::dontwait);
        if (!result) {
            throw std::runtime_error("Send failed");
        }
    } catch (const zmq::error_t& e) {
        if (e.num() == EAGAIN) {
            // Queue is full, apply backpressure
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            safe_send(sock, data); // Retry
        } else {
            throw; // Re-throw other errors
        }
    }
}
```

### Use connection management patterns
```cpp
// Best practice: Lazy connection with retry
class ReliableConnection {
    zmq::context_t& ctx_;
    zmq::socket_t socket_;
    std::string endpoint_;
    bool connected_ = false;
    
public:
    ReliableConnection(zmq::context_t& ctx, zmq::socket_type type)
        : ctx_(ctx), socket_(ctx_, type) {}
    
    void connect(const std::string& endpoint) {
        endpoint_ = endpoint;
        socket_.connect(endpoint);
        connected_ = true;
    }
    
    void reconnect() {
        if (connected_) {
            socket_.close();
            socket_ = zmq::socket_t(ctx_, socket_.get(zmq::sockopt::type));
            socket_.connect(endpoint_);
        }
    }
};
```

### Implement proper shutdown sequences
```cpp
// Best practice: Graceful shutdown
class GracefulServer {
    zmq::context_t ctx_;
    zmq::socket_t socket_;
    std::atomic<bool> running_{true};
    
public:
    GracefulServer() 
        : ctx_()
        , socket_(ctx_, zmq::socket_type::rep) 
    {
        socket_.bind("tcp://*:5555");
    }
    
    void run() {
        while (running_) {
            zmq::message_t msg;
            try {
                socket_.recv(msg, zmq::recv_flags::none);
                // Process message
                socket_.send(zmq::str_buffer("OK"), zmq::send_flags::none);
            } catch (const zmq::error_t& e) {
                if (e.num() == ETERM) break; // Context terminated
                throw;
            }
        }
    }
    
    void shutdown() {
        running_ = false;
        // Send a message to unblock recv
        zmq::socket_t shutdown_sock(ctx_, zmq::socket_type::push);
        shutdown_sock.connect("tcp://127.0.0.1:5555");
        shutdown_sock.send(zmq::str_buffer("SHUTDOWN"), zmq::send_flags::none);
    }
};
```

### Use message pooling for performance
```cpp
// Best practice: Reuse messages to reduce allocations
class MessagePool {
    std::vector<zmq::message_t> pool_;
    std::mutex mutex_;
    
public:
    zmq::message_t acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_.empty()) {
            return zmq::message_t();
        }
        auto msg = std::move(pool_.back());
        pool_.pop_back();
        return msg;
    }
    
    void release(zmq::message_t msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push_back(std::move(msg));
    }
};
```
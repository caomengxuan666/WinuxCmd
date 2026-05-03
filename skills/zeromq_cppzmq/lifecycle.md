# Lifecycle


### Construction and initialization
```cpp
// Context construction
zmq::context_t ctx;                    // Default: 1 I/O thread
zmq::context_t ctx2(4);                // 4 I/O threads
zmq::context_t ctx3(zmq::ctxopt::io_threads, 2); // Named parameter

// Socket construction
zmq::context_t ctx;
zmq::socket_t sock(ctx, zmq::socket_type::push); // Requires context
zmq::socket_t sock2(ctx, zmq::socket_type::pull);

// Message construction
zmq::message_t msg;                    // Empty message
zmq::message_t msg2(100);              // Pre-allocated 100 bytes
zmq::message_t msg3("data", 4);        // From buffer
```

### Move semantics
```cpp
// Move construction and assignment
zmq::context_t ctx1;
zmq::context_t ctx2 = std::move(ctx1); // Move context
// ctx1 is now in valid but unspecified state

zmq::socket_t sock1(ctx2, zmq::socket_type::push);
zmq::socket_t sock2 = std::move(sock1); // Move socket
// sock1 is now empty (handle == nullptr)

zmq::message_t msg1;
msg1.init_size(100);
zmq::message_t msg2 = std::move(msg1); // Move message
// msg1 is now empty
```

### Destruction and cleanup
```cpp
// Proper destruction order
{
    zmq::context_t ctx;
    {
        zmq::socket_t sock(ctx, zmq::socket_type::push);
        sock.bind("tcp://*:5555");
        // Use socket...
    } // sock destroyed first - unbinds and closes
} // ctx destroyed last - terminates I/O threads

// Explicit close
zmq::context_t ctx;
zmq::socket_t sock(ctx, zmq::socket_type::push);
sock.close(); // Explicit close, socket becomes invalid
// sock.handle() returns nullptr after close
```

### Resource management patterns
```cpp
// Managing socket lifetime
class SocketManager {
    zmq::context_t& ctx_;
    std::unique_ptr<zmq::socket_t> socket_;
    
public:
    SocketManager(zmq::context_t& ctx, zmq::socket_type type)
        : ctx_(ctx)
        , socket_(std::make_unique<zmq::socket_t>(ctx_, type)) 
    {}
    
    void reset(zmq::socket_type type) {
        socket_ = std::make_unique<zmq::socket_t>(ctx_, type);
    }
    
    zmq::socket_t& get() { return *socket_; }
};

// Context sharing
class ContextPool {
    std::shared_ptr<zmq::context_t> ctx_;
    
public:
    ContextPool() : ctx_(std::make_shared<zmq::context_t>()) {}
    
    std::shared_ptr<zmq::context_t> get_context() { return ctx_; }
};
```

### Message lifecycle
```cpp
// Message creation and destruction
void message_lifecycle() {
    zmq::message_t msg;
    
    // Initialize with data
    msg.init_size(1024);
    memcpy(msg.data(), "Hello", 5);
    
    // Rebuild message
    msg.rebuild(); // Clear and reinitialize
    msg.init_size(512);
    
    // Message automatically destroyed when going out of scope
    
    // Zero-copy from external buffer
    char buffer[100];
    zmq::message_t msg2(buffer, 100, [](void*, void*){});
}
```
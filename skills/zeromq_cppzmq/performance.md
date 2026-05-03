# Performance


### Zero-copy message handling
```cpp
// High performance: Use zero-copy where possible
void zero_copy_example() {
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    
    // Pre-allocate buffer
    std::vector<char> buffer(1024 * 1024); // 1MB
    
    // Zero-copy send (if supported)
    zmq::message_t msg(buffer.data(), buffer.size(), 
        [](void* data, void* hint) {
            // Custom cleanup if needed
        });
    sock.send(msg, zmq::send_flags::none);
}

// Avoid unnecessary copying
void efficient_send() {
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    
    // BAD: Creates temporary string
    sock.send(zmq::str_buffer(std::string("data")), zmq::send_flags::none);
    
    // GOOD: Direct buffer
    sock.send(zmq::str_buffer("data"), zmq::send_flags::none);
}
```

### Batch processing for throughput
```cpp
// High throughput: Batch messages
void batch_send(zmq::socket_t& sock, const std::vector<std::string>& messages) {
    const size_t batch_size = 100;
    
    for (size_t i = 0; i < messages.size(); i += batch_size) {
        size_t end = std::min(i + batch_size, messages.size());
        
        // Send batch without waiting
        for (size_t j = i; j < end; ++j) {
            sock.send(zmq::str_buffer(messages[j]), 
                     zmq::send_flags::dontwait);
        }
    }
}

// Batch receive
std::vector<zmq::message_t> batch_recv(zmq::socket_t& sock, size_t count) {
    std::vector<zmq::message_t> messages;
    messages.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        zmq::message_t msg;
        if (sock.recv(msg, zmq::recv_flags::dontwait)) {
            messages.push_back(std::move(msg));
        } else {
            break; // No more messages
        }
    }
    return messages;
}
```

### Memory allocation patterns
```cpp
// Optimize memory allocation
class OptimizedReceiver {
    zmq::socket_t& sock_;
    zmq::message_t msg_; // Reuse message
    
public:
    OptimizedReceiver(zmq::socket_t& sock) : sock_(sock) {}
    
    void process_messages() {
        while (true) {
            sock_.recv(msg_, zmq::recv_flags::none);
            // Process msg_ - no allocation needed
            process(msg_.data(), msg_.size());
            msg_.rebuild(); // Clear for reuse
        }
    }
};

// Pre-allocate message pool
class MessagePool {
    std::array<zmq::message_t, 1000> pool_;
    size_t index_ = 0;
    
public:
    zmq::message_t& acquire() {
        return pool_[index_++ % pool_.size()];
    }
};
```

### Socket options for performance
```cpp
// Tune socket options for performance
void configure_high_performance(zmq::socket_t& sock) {
    // Increase send buffer
    sock.set(zmq::sockopt::sndhwm, 100000);
    
    // Increase receive buffer
    sock.set(zmq::sockopt::rcvhwm, 100000);
    
    // Disable Nagle's algorithm for TCP
    sock.set(zmq::sockopt::tcp_nodelay, 1);
    
    // Set send timeout
    sock.set(zmq::sockopt::sndtimeo, 100); // 100ms
    
    // Set receive timeout
    sock.set(zmq::sockopt::rcvtimeo, 100); // 100ms
    
    // Increase message size limit
    sock.set(zmq::sockopt::maxmsgsize, 64 * 1024 * 1024); // 64MB
}
```

### Benchmarking patterns
```cpp
// Measure throughput
#include <chrono>

double benchmark_throughput(zmq::socket_t& sock, size_t message_count) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < message_count; ++i) {
        sock.send(zmq::str_buffer("benchmark"), zmq::send_flags::dontwait);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    return static_cast<double>(message_count) / (duration.count() / 1000000.0);
}
```
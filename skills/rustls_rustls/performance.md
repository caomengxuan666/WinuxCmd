# Performance

```cpp
// Performance: Characteristics, allocation patterns, optimization tips

// Performance characteristics:
// 1. Handshake overhead: ~1-2 round trips (TLS 1.3)
// 2. Memory per connection: ~4-8 KB
// 3. CPU cost: ~0.1-0.5 ms per handshake (modern CPU)
// 4. Throughput: ~1-5 Gbps per core (AES-NI enabled)

// Optimization 1: Reuse configurations
// BAD: Creating new config for each connection
void bad_config_reuse() {
    for (int i = 0; i < 1000; i++) {
        auto builder = rustls_client_config_builder_new();
        auto config = rustls_client_config_builder_build(builder);
        auto conn = rustls_client_connection_new(config, "host");
        // Use connection...
        rustls_connection_free(conn);
        rustls_client_config_free(config);
    }
}

// GOOD: Reuse config across connections
void good_config_reuse() {
    auto builder = rustls_client_config_builder_new();
    rustls_client_config_builder_load_roots_from_native_certs(builder);
    auto config = rustls_client_config_builder_build(builder);
    
    for (int i = 0; i < 1000; i++) {
        auto conn = rustls_client_connection_new(config, "host");
        // Use connection...
        rustls_connection_free(conn);
    }
    rustls_client_config_free(config);
}

// Optimization 2: Session resumption
// BAD: Full handshake every time
void bad_no_resumption() {
    for (int i = 0; i < 100; i++) {
        auto conn = rustls_client_connection_new(config, "host");
        // Full handshake each time - expensive
        rustls_connection_free(conn);
    }
}

// GOOD: Use session resumption
void good_with_resumption() {
    auto store = rustls_client_session_store_new();
    rustls_client_config_set_session_store(config, store);
    
    for (int i = 0; i < 100; i++) {
        auto conn = rustls_client_connection_new(config, "host");
        // First connection: full handshake
        // Subsequent connections: abbreviated handshake (1-RTT)
        rustls_connection_free(conn);
    }
    rustls_client_session_store_free(store);
}

// Optimization 3: Buffer management
// BAD: Small buffers cause many system calls
void bad_small_buffers() {
    char buffer[64];
    size_t total = 0;
    while (total < 65536) {
        size_t read = rustls_connection_read(conn, buffer, sizeof(buffer));
        total += read;
    }
    // 1024 system calls!
}

// GOOD: Large buffers reduce system calls
void good_large_buffers() {
    char buffer[65536];
    size_t total = 0;
    while (total < sizeof(buffer)) {
        size_t read = rustls_connection_read(conn, buffer + total, 
            sizeof(buffer) - total);
        if (read == 0) break;
        total += read;
    }
    // ~1 system call
}

// Optimization 4: Provider selection
// aws-lc-rs vs ring performance characteristics:
// - aws-lc-rs: Better performance, supports AES-NI, post-quantum
// - ring: Slightly slower, but easier to build

// Optimization 5: Connection pooling
class ConnectionPool {
    std::vector<rustls_connection*> idle_connections_;
    rustls_client_config* config_;
    
public:
    rustls_connection* acquire(const std::string& host) {
        if (!idle_connections_.empty()) {
            auto conn = idle_connections_.back();
            idle_connections_.pop_back();
            return conn;
        }
        return rustls_client_connection_new(config_, host.c_str());
    }
    
    void release(rustls_connection* conn) {
        if (idle_connections_.size() < 100) {
            idle_connections_.push_back(conn);
        } else {
            rustls_connection_free(conn);
        }
    }
};
```
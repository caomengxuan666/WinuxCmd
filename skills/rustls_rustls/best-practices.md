# Best Practices

```cpp
// Best Practices: Production-ready patterns

// Practice 1: RAII wrapper for resource management
class TlsConnection {
    rustls_connection* conn_;
public:
    TlsConnection(rustls_client_config* config, const std::string& host) {
        rustls_result result = rustls_client_connection_new_with_result(
            config, host.c_str(), &conn_);
        if (result != RUSTLS_RESULT_OK) {
            throw std::runtime_error(rustls_result_to_string(result));
        }
    }
    
    ~TlsConnection() {
        if (conn_) {
            rustls_connection_free(conn_);
            conn_ = nullptr;
        }
    }
    
    // Prevent copying
    TlsConnection(const TlsConnection&) = delete;
    TlsConnection& operator=(const TlsConnection&) = delete;
    
    // Allow moving
    TlsConnection(TlsConnection&& other) noexcept : conn_(other.conn_) {
        other.conn_ = nullptr;
    }
    
    size_t write(const void* data, size_t len) {
        return rustls_connection_write(conn_, data, len);
    }
    
    size_t read(void* buffer, size_t len) {
        return rustls_connection_read(conn_, buffer, len);
    }
};

// Practice 2: Connection pool with session resumption
class TlsConnectionPool {
    rustls_client_config* config_;
    rustls_client_session_store* session_store_;
    
public:
    TlsConnectionPool() {
        config_ = create_config();
        session_store_ = rustls_client_session_store_new();
        rustls_client_config_set_session_store(config_, session_store_);
    }
    
    std::unique_ptr<TlsConnection> get_connection(const std::string& host) {
        return std::make_unique<TlsConnection>(config_, host);
    }
    
private:
    rustls_client_config* create_config() {
        auto builder = rustls_client_config_builder_new();
        rustls_client_config_builder_load_roots_from_native_certs(builder);
        rustls_client_config_builder_set_alpn_protocols(builder, {"h2", "http/1.1"});
        return rustls_client_config_builder_build(builder);
    }
};

// Practice 3: Proper error handling with logging
class TlsLogger {
public:
    static void log_error(rustls_result result, const std::string& context) {
        std::cerr << "[TLS Error] " << context << ": " 
                  << rustls_result_to_string(result) << std::endl;
        // Send to monitoring system
    }
};

// Practice 4: Timeout handling
class TlsConnectionWithTimeout : public TlsConnection {
    std::chrono::seconds timeout_;
    
public:
    size_t read(void* buffer, size_t len) {
        auto start = std::chrono::steady_clock::now();
        size_t total = 0;
        
        while (total < len) {
            auto elapsed = std::chrono::steady_clock::now() - start;
            if (elapsed > timeout_) {
                throw std::runtime_error("TLS read timeout");
            }
            
            size_t read = rustls_connection_read(conn_, 
                static_cast<char*>(buffer) + total, len - total);
            if (read == 0) break;
            total += read;
        }
        return total;
    }
};

// Practice 5: Graceful shutdown
void graceful_shutdown(rustls_connection* conn) {
    // Send close_notify
    rustls_connection_send_close_notify(conn);
    
    // Wait for peer's close_notify
    char buffer[1];
    while (rustls_connection_read(conn, buffer, 1) > 0) {
        // Drain remaining data
    }
}
```
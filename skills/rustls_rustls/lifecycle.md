# Lifecycle

```cpp
// Lifecycle: Construction, destruction, move semantics, resource management

// Construction patterns
#include <rustls.h>
#include <memory>
#include <stdexcept>

// Pattern 1: Client connection construction
rustls_connection* create_client_connection(
    rustls_client_config* config, 
    const std::string& hostname) {
    
    rustls_connection* conn = nullptr;
    rustls_result result = rustls_client_connection_new_with_result(
        config, hostname.c_str(), &conn);
    
    if (result != RUSTLS_RESULT_OK) {
        throw std::runtime_error(
            "Failed to create connection: " + 
            std::string(rustls_result_to_string(result)));
    }
    return conn;
}

// Pattern 2: Server connection construction
rustls_connection* create_server_connection(rustls_server_config* config) {
    rustls_connection* conn = rustls_server_connection_new(config);
    if (conn == nullptr) {
        throw std::runtime_error("Failed to create server connection");
    }
    return conn;
}

// Pattern 3: Proper destruction with RAII
class ConnectionGuard {
    rustls_connection* conn_;
public:
    explicit ConnectionGuard(rustls_connection* conn) : conn_(conn) {}
    
    ~ConnectionGuard() {
        if (conn_) {
            // Send close_notify before freeing
            rustls_connection_send_close_notify(conn_);
            rustls_connection_free(conn_);
            conn_ = nullptr;
        }
    }
    
    // Move semantics
    ConnectionGuard(ConnectionGuard&& other) noexcept : conn_(other.conn_) {
        other.conn_ = nullptr;
    }
    
    ConnectionGuard& operator=(ConnectionGuard&& other) noexcept {
        if (this != &other) {
            if (conn_) rustls_connection_free(conn_);
            conn_ = other.conn_;
            other.conn_ = nullptr;
        }
        return *this;
    }
    
    // No copying
    ConnectionGuard(const ConnectionGuard&) = delete;
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;
    
    rustls_connection* get() const { return conn_; }
};

// Pattern 4: Config lifecycle management
class ConfigManager {
    rustls_client_config* client_config_;
    rustls_server_config* server_config_;
    
public:
    ConfigManager() {
        // Build client config
        auto client_builder = rustls_client_config_builder_new();
        rustls_client_config_builder_load_roots_from_native_certs(client_builder);
        client_config_ = rustls_client_config_builder_build(client_builder);
        
        // Build server config
        auto server_builder = rustls_server_config_builder_new();
        server_config_ = rustls_server_config_builder_build(server_builder);
    }
    
    ~ConfigManager() {
        if (client_config_) {
            rustls_client_config_free(client_config_);
            client_config_ = nullptr;
        }
        if (server_config_) {
            rustls_server_config_free(server_config_);
            server_config_ = nullptr;
        }
    }
    
    // Move semantics
    ConfigManager(ConfigManager&& other) noexcept 
        : client_config_(other.client_config_)
        , server_config_(other.server_config_) {
        other.client_config_ = nullptr;
        other.server_config_ = nullptr;
    }
    
    rustls_client_config* client_config() const { return client_config_; }
    rustls_server_config* server_config() const { return server_config_; }
};

// Pattern 5: Session store lifecycle
class SessionStoreManager {
    rustls_client_session_store* store_;
    
public:
    SessionStoreManager() {
        store_ = rustls_client_session_store_new();
        if (!store_) {
            throw std::runtime_error("Failed to create session store");
        }
    }
    
    ~SessionStoreManager() {
        if (store_) {
            rustls_client_session_store_free(store_);
            store_ = nullptr;
        }
    }
    
    // Prevent copying
    SessionStoreManager(const SessionStoreManager&) = delete;
    SessionStoreManager& operator=(const SessionStoreManager&) = delete;
    
    rustls_client_session_store* get() const { return store_; }
};
```
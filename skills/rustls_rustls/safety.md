# Safety

```cpp
// Safety: Conditions that must NEVER occur

// Condition 1: NEVER use a null config pointer
// BAD: Null config
void bad_null_config() {
    rustls_client_config* config = nullptr;
    rustls_connection* conn = rustls_client_connection_new(config, "host");
    // CRASH: null pointer dereference
}

// GOOD: Always validate config
void good_null_config() {
    rustls_client_config* config = rustls_client_config_builder_build(builder);
    if (config == nullptr) {
        throw std::runtime_error("Failed to create config");
    }
    rustls_connection* conn = rustls_client_connection_new(config, "host");
}

// Condition 2: NEVER use a connection after free
// BAD: Use after free
void bad_use_after_free() {
    rustls_connection_free(conn);
    size_t written = rustls_connection_write(conn, "data", 4);
    // Undefined behavior - may crash or corrupt memory
}

// GOOD: Track connection lifecycle
void good_connection_lifecycle() {
    rustls_connection_free(conn);
    conn = nullptr; // Prevent accidental reuse
}

// Condition 3: NEVER ignore TLS handshake errors
// BAD: Ignoring handshake failure
void bad_handshake() {
    rustls_connection* conn = rustls_client_connection_new(config, "host");
    // No handshake check - connection may be invalid
    rustls_connection_write(conn, "data", 4);
}

// GOOD: Complete handshake before I/O
void good_handshake() {
    rustls_connection* conn = nullptr;
    rustls_result result = rustls_client_connection_new_with_result(
        config, "host", &conn);
    if (result != RUSTLS_RESULT_OK) {
        // Handle error
        return;
    }
    // Now safe to use connection
}

// Condition 4: NEVER disable certificate verification in production
// BAD: Disabling verification
void bad_disable_verification() {
    rustls_client_config_builder_disable_verification(builder);
    // Man-in-the-middle attack possible
}

// GOOD: Proper verification
void good_verification() {
    rustls_client_config_builder_load_roots_from_native_certs(builder);
    // Or use custom CA
    rustls_client_config_builder_load_roots_from_pem_file(builder, "trusted_ca.pem");
}

// Condition 5: NEVER share mutable state across threads without synchronization
// BAD: Thread-unsafe sharing
void bad_thread_safety() {
    rustls_connection* shared_conn = /* ... */;
    std::thread t1([&]() { rustls_connection_write(shared_conn, "a", 1); });
    std::thread t2([&]() { rustls_connection_write(shared_conn, "b", 1); });
    // Data race - undefined behavior
}

// GOOD: Thread-local connections
void good_thread_safety() {
    // Each thread gets its own connection
    std::thread t1([config]() {
        rustls_connection* conn = rustls_client_connection_new(config, "host");
        rustls_connection_write(conn, "a", 1);
        rustls_connection_free(conn);
    });
    std::thread t2([config]() {
        rustls_connection* conn = rustls_client_connection_new(config, "host");
        rustls_connection_write(conn, "b", 1);
        rustls_connection_free(conn);
    });
}
```
# Pitfalls

```cpp
// Pitfalls: Common mistakes with BAD and GOOD examples

// Pitfall 1: Not checking connection result
// BAD: Ignoring return value
void bad_connect() {
    rustls_connection* conn = rustls_client_connection_new(config, "example.com");
    rustls_connection_write(conn, "data", 4); // May crash if connection failed
}

// GOOD: Always check result
void good_connect() {
    rustls_connection* conn = nullptr;
    rustls_result result = rustls_client_connection_new_with_result(
        config, "example.com", &conn);
    if (result != RUSTLS_RESULT_OK) {
        std::cerr << "Connection failed: " << rustls_result_to_string(result);
        return;
    }
    // Safe to use conn
}

// Pitfall 2: Forgetting to free resources
// BAD: Memory leak
void bad_resource_management() {
    rustls_client_config* config = rustls_client_config_builder_build(builder);
    // config never freed
}

// GOOD: Proper cleanup
void good_resource_management() {
    rustls_client_config* config = rustls_client_config_builder_build(builder);
    // Use config...
    rustls_client_config_free(config);
}

// Pitfall 3: Using freed connection
// BAD: Use after free
void bad_use_after_free() {
    rustls_connection_free(conn);
    rustls_connection_write(conn, "data", 4); // Undefined behavior
}

// GOOD: Set to nullptr after free
void good_use_after_free() {
    rustls_connection_free(conn);
    conn = nullptr;
}

// Pitfall 4: Not handling partial reads/writes
// BAD: Assuming complete I/O
void bad_io() {
    char buffer[4096];
    size_t read = rustls_connection_read(conn, buffer, sizeof(buffer));
    // May not have read all data
}

// GOOD: Loop until complete
void good_io() {
    char buffer[4096];
    size_t total_read = 0;
    while (total_read < sizeof(buffer)) {
        size_t read = rustls_connection_read(conn, 
            buffer + total_read, sizeof(buffer) - total_read);
        if (read == 0) break; // Connection closed
        total_read += read;
    }
}

// Pitfall 5: Ignoring certificate validation
// BAD: Disabling verification
void bad_cert_check() {
    rustls_client_config_builder_disable_verification(builder);
    // Dangerous - no security
}

// GOOD: Proper certificate handling
void good_cert_check() {
    rustls_client_config_builder_load_roots_from_native_certs(builder);
    // Or load specific CA certificates
    rustls_client_config_builder_load_roots_from_pem_file(builder, "ca.pem");
}

// Pitfall 6: Mixing providers incorrectly
// BAD: Inconsistent provider usage
void bad_provider() {
    rustls_crypto_provider* provider1 = rustls_crypto_provider_aws_lc_rs_new();
    rustls_crypto_provider* provider2 = rustls_crypto_provider_ring_new();
    // Using different providers for client/server can cause issues
}

// GOOD: Consistent provider
void good_provider() {
    rustls_crypto_provider* provider = rustls_crypto_provider_aws_lc_rs_new();
    rustls_client_config* client_config = 
        rustls_client_config_builder_new_with_provider(provider);
    rustls_server_config* server_config = 
        rustls_server_config_builder_new_with_provider(provider);
}
```
# Quickstart

```cpp
// Quickstart: Common rustls usage patterns
#include <rustls.h>
#include <iostream>
#include <string>
#include <vector>

// Pattern 1: Simple TLS client connection
void simple_client() {
    rustls_client_config_builder* config_builder = 
        rustls_client_config_builder_new();
    rustls_client_config_builder_load_roots_from_native_certs(config_builder);
    rustls_client_config* config = rustls_client_config_builder_build(config_builder);
    
    rustls_connection* conn = rustls_client_connection_new(config, "example.com");
    // Use connection...
    rustls_connection_free(conn);
    rustls_client_config_free(config);
}

// Pattern 2: TLS server with certificate
void simple_server() {
    rustls_server_config_builder* builder = 
        rustls_server_config_builder_new();
    
    // Load certificate chain and private key
    rustls_server_config_builder_load_certs_from_pem_file(builder, "cert.pem");
    rustls_server_config_builder_load_private_key_from_pem_file(builder, "key.pem");
    
    rustls_server_config* config = rustls_server_config_builder_build(builder);
    rustls_connection* conn = rustls_server_connection_new(config);
    // Use connection...
    rustls_connection_free(conn);
    rustls_server_config_free(config);
}

// Pattern 3: Reading/writing TLS data
void read_write_example(rustls_connection* conn) {
    const char* data = "Hello, TLS!";
    size_t written = rustls_connection_write(conn, data, strlen(data));
    
    char buffer[4096];
    size_t read = rustls_connection_read(conn, buffer, sizeof(buffer));
}

// Pattern 4: Error handling
void error_handling() {
    rustls_result result = rustls_client_connection_new_with_result(
        config, "example.com", &conn);
    
    if (result != RUSTLS_RESULT_OK) {
        const char* error_msg = rustls_result_to_string(result);
        std::cerr << "TLS error: " << error_msg << std::endl;
    }
}

// Pattern 5: Session resumption
void session_resumption() {
    rustls_client_session_store* store = rustls_client_session_store_new();
    rustls_client_config_builder_set_session_store(builder, store);
    // Subsequent connections can resume sessions
}

// Pattern 6: ALPN protocol negotiation
void alpn_example() {
    rustls_client_config_builder_set_alpn_protocols(builder, 
        {"h2", "http/1.1"});
}

// Pattern 7: Certificate verification callback
void cert_verification() {
    rustls_client_config_builder_set_verify_callback(builder, 
        [](void* userdata, rustls_verify_server_cert_params params) -> bool {
            // Custom verification logic
            return true;
        }, nullptr);
}
```
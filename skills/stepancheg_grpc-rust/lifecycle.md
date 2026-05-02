# Lifecycle

```markdown
# Lifecycle Management in grpc-rust

## Construction

### Server Construction

```rust
use grpc::ServerBuilder;

fn build_server() -> grpc::Server {
    let mut builder = ServerBuilder::new_plain();
    builder.http.set_port(50051);
    builder.add_service(greeter_grpc::GreeterServer::new_service_def(GreeterService));
    
    // Build consumes the builder
    builder.build().expect("Failed to build server")
}
```

### Client Construction

```rust
use grpc::ClientStub;

fn build_client() -> greeter_grpc::GreeterClient {
    greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .expect("Failed to create client")
}
```

## Resource Management

### Server Lifecycle

```rust
fn server_lifecycle() {
    // Construction
    let server = build_server();
    
    // Server is running in background threads
    // No explicit start() needed
    
    // When server is dropped, it shuts down
    // But this may not be graceful
    drop(server); // Implicit shutdown
}
```

### Client Lifecycle

```rust
fn client_lifecycle() {
    // Construction
    let client = build_client();
    
    // Client creates HTTP/2 connection
    // Connection is established lazily on first RPC
    
    // Client can be cloned (Arc internally)
    let client2 = client.clone();
    
    // When all clones are dropped, connection closes
    drop(client);
    drop(client2); // Connection closes here
}
```

## Move Semantics

### Moving Server

```rust
fn move_server() {
    let server = build_server();
    
    // Server can be moved to another thread
    std::thread::spawn(move || {
        // Server runs here
        loop { std::thread::park(); }
    });
}
```

### Moving Client

```rust
fn move_client() {
    let client = build_client();
    
    // Client can be moved to closures
    let handle = std::thread::spawn(move || {
        let mut req = greeter::HelloRequest::new();
        req.set_name("World".to_string());
        client.say_hello(RequestOptions::new(), req).wait()
    });
    
    let result = handle.join().unwrap();
}
```

## Resource Cleanup

### Proper Shutdown

```rust
fn graceful_shutdown() {
    let server = build_server();
    
    // Register cleanup handler
    ctrlc::set_handler(move || {
        println!("Shutting down...");
        // Server is dropped when this closure exits
        std::process::exit(0);
    }).expect("Error setting Ctrl-C handler");
    
    // Keep main thread alive
    loop { std::thread::park(); }
}
```

### Connection Management

```rust
fn connection_management() {
    let client = build_client();
    
    // Connection is established on first RPC
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    let _ = client.say_hello(RequestOptions::new(), req).wait();
    
    // Connection stays alive for subsequent RPCs
    let mut req2 = greeter::HelloRequest::new();
    req2.set_name("Again".to_string());
    let _ = client.say_hello(RequestOptions::new(), req2).wait();
    
    // Connection closes when client is dropped
    drop(client);
}
```

## Error Handling During Construction

```rust
fn safe_construction() -> Result<(), Box<dyn std::error::Error>> {
    let server = ServerBuilder::new_plain()
        .build()
        .map_err(|e| format!("Server construction failed: {:?}", e))?;
    
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .map_err(|e| format!("Client construction failed: {:?}", e))?;
    
    Ok(())
}
```

## Resource Leak Prevention

```rust
fn prevent_leaks() {
    // Use RAII pattern
    struct ServerGuard {
        server: Option<grpc::Server>,
    }
    
    impl Drop for ServerGuard {
        fn drop(&mut self) {
            if let Some(server) = self.server.take() {
                // Server is dropped here
                println!("Server shutdown");
            }
        }
    }
    
    let guard = ServerGuard {
        server: Some(build_server()),
    };
    
    // Guard ensures server is properly shut down
}
```

For more safety considerations, see [safety.md](safety.md). For threading details, see [threading.md](threading.md).
```
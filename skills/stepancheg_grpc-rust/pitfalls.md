# Pitfalls

```markdown
# Common Pitfalls in grpc-rust

## 1. Forgetting to Call `.wait()` on Futures

**BAD**: The future is never executed
```rust
fn bad_example(client: &greeter_grpc::GreeterClient) {
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    // Future is created but never executed!
    let _future = client.say_hello(RequestOptions::new(), req);
    // Program continues without waiting for response
}
```

**GOOD**: Always call `.wait()` to execute the future
```rust
fn good_example(client: &greeter_grpc::GreeterClient) -> Result<(), grpc::Error> {
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    let reply = client.say_hello(RequestOptions::new(), req).wait()?;
    println!("Response: {}", reply.get_message());
    Ok(())
}
```

## 2. Ignoring Error Handling

**BAD**: Using `.unwrap()` on gRPC calls
```rust
fn bad_example() {
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .unwrap(); // Panics if connection fails
    
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    let reply = client.say_hello(RequestOptions::new(), req)
        .wait()
        .unwrap(); // Panics on RPC failure
}
```

**GOOD**: Proper error propagation
```rust
fn good_example() -> Result<(), Box<dyn std::error::Error>> {
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())?;
    
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    match client.say_hello(RequestOptions::new(), req).wait() {
        Ok(reply) => println!("Success: {}", reply.get_message()),
        Err(e) => eprintln!("RPC failed: {:?}", e),
    }
    Ok(())
}
```

## 3. Not Setting Required Protobuf Fields

**BAD**: Missing required fields
```rust
fn bad_example() {
    let mut req = greeter::HelloRequest::new();
    // name field is not set!
    // This may cause serialization errors or unexpected behavior
    let reply = client.say_hello(RequestOptions::new(), req).wait();
}
```

**GOOD**: Always set required fields
```rust
fn good_example() {
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string()); // Required field
    let reply = client.say_hello(RequestOptions::new(), req).wait();
}
```

## 4. Blocking the Event Loop

**BAD**: Calling `.wait()` inside a streaming handler
```rust
fn bad_stream_handler() {
    // Inside a streaming RPC handler
    let client = create_client();
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    // This blocks the streaming handler thread!
    let reply = client.say_hello(RequestOptions::new(), req).wait();
}
```

**GOOD**: Use separate threads for blocking calls
```rust
fn good_stream_handler() {
    let client = create_client();
    std::thread::spawn(move || {
        let mut req = greeter::HelloRequest::new();
        req.set_name("World".to_string());
        let reply = client.say_hello(RequestOptions::new(), req).wait();
    });
}
```

## 5. Incorrect Port Binding

**BAD**: Binding to privileged port without permissions
```rust
fn bad_example() {
    let mut server = ServerBuilder::new_plain();
    server.http.set_port(80); // Requires root privileges
    let server = server.build().expect("Failed to start server");
}
```

**GOOD**: Use non-privileged ports
```rust
fn good_example() {
    let mut server = ServerBuilder::new_plain();
    server.http.set_port(50051); // Non-privileged port
    let server = server.build().expect("Failed to start server");
}
```

## 6. Memory Leaks with Large Messages

**BAD**: Holding references to large protobuf messages
```rust
fn bad_example() {
    let mut large_msg = greeter::LargeMessage::new();
    // Fill with lots of data...
    let data = large_msg.get_data().to_vec(); // Clone large data
    // Original message still in memory
}
```

**GOOD**: Use `take_*` methods to avoid cloning
```rust
fn good_example() {
    let mut large_msg = greeter::LargeMessage::new();
    // Fill with lots of data...
    let data = large_msg.take_data(); // Moves data out, message is cleared
    // Original message is now empty
}
```

## 7. Ignoring Server Shutdown

**BAD**: Abrupt program termination
```rust
fn bad_example() {
    let server = ServerBuilder::new_plain()
        .build()
        .expect("Failed to start server");
    // Program exits immediately, dropping server without cleanup
}
```

**GOOD**: Proper server lifecycle management
```rust
fn good_example() {
    let server = ServerBuilder::new_plain()
        .build()
        .expect("Failed to start server");
    
    // Keep server running
    ctrlc::set_handler(move || {
        println!("Shutting down gracefully...");
        std::process::exit(0);
    }).expect("Error setting Ctrl-C handler");
    
    loop { std::thread::park(); }
}
```

For more safety considerations, see [safety.md](safety.md). For best practices, see [best-practices.md](best-practices.md).
```
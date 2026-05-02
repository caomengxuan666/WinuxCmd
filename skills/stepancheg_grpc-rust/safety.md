# Safety

```markdown
# Safety Guidelines for grpc-rust

## Red-Line Conditions: These MUST NEVER Occur

### 1. NEVER Use Uninitialized Protobuf Messages

**BAD**: Creating messages without setting required fields
```rust
// This can cause undefined behavior or panics
fn bad_example() {
    let mut req = greeter::HelloRequest::new();
    // name field is required but not set
    let reply = client.say_hello(RequestOptions::new(), req).wait();
    // May panic or send corrupted data
}
```

**GOOD**: Always initialize all required fields
```rust
fn good_example() {
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string()); // Required field
    req.set_greeting("Hello".to_string()); // Optional field with default
    
    let reply = client.say_hello(RequestOptions::new(), req)
        .wait()
        .expect("RPC failed");
}
```

### 2. NEVER Ignore gRPC Status Codes

**BAD**: Silently ignoring errors
```rust
fn bad_example() {
    let reply = client.say_hello(RequestOptions::new(), req).wait();
    // Ignoring potential errors
    println!("Reply: {}", reply.unwrap().get_message());
}
```

**GOOD**: Always handle all possible error conditions
```rust
fn good_example() -> Result<(), grpc::Error> {
    let reply = client.say_hello(RequestOptions::new(), req).wait()?;
    
    // Check for specific gRPC status codes
    match reply.get_status() {
        grpc::GrpcStatus::Ok => println!("Success: {}", reply.get_message()),
        grpc::GrpcStatus::Unavailable => {
            eprintln!("Service unavailable, retrying...");
            // Implement retry logic
        }
        grpc::GrpcStatus::DeadlineExceeded => {
            eprintln!("Request timed out");
            return Err(grpc::Error::Other("Timeout".to_string()));
        }
        _ => {
            eprintln!("Unexpected status: {:?}", reply.get_status());
            return Err(grpc::Error::Other("Unexpected status".to_string()));
        }
    }
    Ok(())
}
```

### 3. NEVER Share Mutable State Across Threads Without Synchronization

**BAD**: Unsafe shared state
```rust
use std::sync::Mutex;

fn bad_example() {
    let counter = std::cell::RefCell::new(0u32); // Not thread-safe!
    
    // Multiple threads accessing RefCell causes panic
    let handles: Vec<_> = (0..10).map(|_| {
        std::thread::spawn(|| {
            *counter.borrow_mut() += 1; // Panics if multiple threads access
        })
    }).collect();
}
```

**GOOD**: Use proper synchronization
```rust
fn good_example() {
    let counter = std::sync::Arc::new(std::sync::atomic::AtomicU32::new(0));
    
    let handles: Vec<_> = (0..10).map(|_| {
        let counter = counter.clone();
        std::thread::spawn(move || {
            counter.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        })
    }).collect();
    
    for handle in handles {
        handle.join().unwrap();
    }
}
```

### 4. NEVER Use `.unwrap()` on gRPC Results in Production

**BAD**: Panicking on errors
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

**GOOD**: Proper error handling
```rust
fn good_example() -> Result<(), Box<dyn std::error::Error>> {
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .map_err(|e| format!("Failed to create client: {:?}", e))?;
    
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    let reply = client.say_hello(RequestOptions::new(), req)
        .wait()
        .map_err(|e| format!("RPC failed: {:?}", e))?;
    
    println!("Success: {}", reply.get_message());
    Ok(())
}
```

### 5. NEVER Use Deprecated or Removed APIs

**BAD**: Using old API patterns
```rust
// This API may be removed or changed
fn bad_example() {
    let client = greeter_grpc::GreeterClient::new("localhost", 50051, false) // Deprecated
        .unwrap();
}
```

**GOOD**: Use current API
```rust
fn good_example() {
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .expect("Failed to create client");
}
```

For more common mistakes, see [pitfalls.md](pitfalls.md). For lifecycle management, see [lifecycle.md](lifecycle.md).
```
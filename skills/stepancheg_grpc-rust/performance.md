# Performance

```rust
# Performance Characteristics of grpc-rust

## Known Limitations

The README explicitly states performance is a TODO item. This library is not optimized for high-throughput scenarios.

## Allocation Patterns

### Message Allocation

**BAD**: Repeated allocation of large messages
```rust
fn bad_allocation() {
    for i in 0..1000 {
        let mut msg = greeter::HelloRequest::new();
        msg.set_name(format!("User {}", i));
        // Each iteration allocates new message
        let _ = client.say_hello(RequestOptions::new(), msg).wait();
    }
}
```

**GOOD**: Reuse message buffers
```rust
fn good_allocation() {
    let mut msg = greeter::HelloRequest::new();
    for i in 0..1000 {
        msg.set_name(format!("User {}", i));
        // Reuse same message
        let _ = client.say_hello(RequestOptions::new(), msg.clone()).wait();
        msg.clear_name(); // Clear for next iteration
    }
}
```

## Serialization Overhead

### Minimize Serialization

```rust
fn minimize_serialization() {
    // BAD: Serialize multiple times
    let mut msg = greeter::HelloRequest::new();
    msg.set_name("World".to_string());
    let bytes = msg.write_to_bytes().unwrap(); // Serialize once
    
    // GOOD: Use protobuf directly
    let mut msg = greeter::HelloRequest::new();
    msg.set_name("World".to_string());
    // Let gRPC handle serialization
}
```

## Connection Pooling

### Single Connection vs Pool

```rust
fn connection_pooling() {
    // BAD: Create new connection for each RPC
    for i in 0..100 {
        let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
            .unwrap();
        let mut req = greeter::HelloRequest::new();
        req.set_name(format!("User {}", i));
        let _ = client.say_hello(RequestOptions::new(), req).wait();
    }
    
    // GOOD: Reuse connection
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .unwrap();
    for i in 0..100 {
        let mut req = greeter::HelloRequest::new();
        req.set_name(format!("User {}", i));
        let _ = client.say_hello(RequestOptions::new(), req).wait();
    }
}
```

## Batch Operations

### Batching Requests

```rust
fn batch_operations() {
    // BAD: Sequential blocking calls
    for i in 0..10 {
        let mut req = greeter::HelloRequest::new();
        req.set_name(format!("User {}", i));
        let reply = client.say_hello(RequestOptions::new(), req).wait().unwrap();
        println!("Reply: {}", reply.get_message());
    }
    
    // GOOD: Use threads for parallel execution
    let handles: Vec<_> = (0..10).map(|i| {
        let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
            .unwrap();
        std::thread::spawn(move || {
            let mut req = greeter::HelloRequest::new();
            req.set_name(format!("User {}", i));
            client.say_hello(RequestOptions::new(), req).wait()
        })
    }).collect();
    
    for handle in handles {
        let reply = handle.join().unwrap().unwrap();
        println!("Reply: {}", reply.get_message());
    }
}
```

## Memory Management

### Large Messages

```rust
fn large_message_handling() {
    // BAD: Keep large messages in memory
    let mut large_msg = greeter::LargeMessage::new();
    large_msg.set_data(vec![0u8; 1024 * 1024]); // 1MB
    let data = large_msg.get_data().to_vec(); // Clone 1MB
    // Original still in memory
    
    // GOOD: Use take to move data
    let mut large_msg = greeter::LargeMessage::new();
    large_msg.set_data(vec![0u8; 1024 * 1024]); // 1MB
    let data = large_msg.take_data(); // Move data out
    // Original is now empty
}
```

## Threading Overhead

### Thread Management

```rust
fn thread_management() {
    // BAD: Create threads for each RPC
    for i in 0..100 {
        std::thread::spawn(move || {
            let client = build_client();
            let mut req = greeter::HelloRequest::new();
            req.set_name(format!("User {}", i));
            let _ = client.say_hello(RequestOptions::new(), req).wait();
        });
    }
    
    // GOOD: Use thread pool
    use std::sync::mpsc;
    let (tx, rx) = mpsc::channel();
    
    let worker = std::thread::spawn(move || {
        let client = build_client();
        for name in rx {
            let mut req = greeter::HelloRequest::new();
            req.set_name(name);
            let _ = client.say_hello(RequestOptions::new(), req).wait();
        }
    });
    
    for i in 0..100 {
        tx.send(format!("User {}", i)).unwrap();
    }
}
```

## Profiling Tips

```rust
fn profile_performance() {
    let start = std::time::Instant::now();
    
    let client = build_client();
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    let _ = client.say_hello(RequestOptions::new(), req).wait();
    
    let duration = start.elapsed();
    println!("RPC took {:?}", duration);
    
    // Monitor memory usage
    let usage = std::mem::size_of::<greeter::HelloRequest>();
    println!("Message size: {} bytes", usage);
}
```

For threading considerations, see [threading.md](threading.md). For lifecycle management, see [lifecycle.md](lifecycle.md).
```
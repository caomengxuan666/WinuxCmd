# Threading

```rust
# Thread Safety in grpc-rust

## Thread Safety Guarantees

### Server Thread Safety

The server runs in its own thread pool. Each incoming request is handled in a separate thread.

```rust
fn server_threading() {
    let mut server = ServerBuilder::new_plain();
    server.http.set_port(50051);
    server.add_service(greeter_grpc::GreeterServer::new_service_def(GreeterService));
    let server = server.build().expect("Failed to build server");
    
    // Server spawns internal threads
    // Each RPC handler runs in its own thread
    // Handlers must be thread-safe
}
```

### Client Thread Safety

Clients are `Send` and `Sync`, but not all operations are thread-safe.

```rust
fn client_threading() {
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .unwrap();
    
    // Client can be shared across threads
    let client2 = client.clone();
    
    std::thread::spawn(move || {
        let mut req = greeter::HelloRequest::new();
        req.set_name("Thread 1".to_string());
        let _ = client.say_hello(RequestOptions::new(), req).wait();
    });
    
    std::thread::spawn(move || {
        let mut req = greeter::HelloRequest::new();
        req.set_name("Thread 2".to_string());
        let _ = client2.say_hello(RequestOptions::new(), req).wait();
    });
}
```

## Concurrent Access Patterns

### Safe Concurrent Access

```rust
use std::sync::Arc;

fn safe_concurrent_access() {
    let client = Arc::new(
        greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
            .unwrap()
    );
    
    let mut handles = vec![];
    for i in 0..10 {
        let client = client.clone();
        handles.push(std::thread::spawn(move || {
            let mut req = greeter::HelloRequest::new();
            req.set_name(format!("User {}", i));
            client.say_hello(RequestOptions::new(), req).wait()
        }));
    }
    
    for handle in handles {
        let result = handle.join().unwrap();
        println!("Result: {:?}", result);
    }
}
```

### Unsafe Shared State

```rust
// BAD: Shared mutable state without synchronization
fn unsafe_shared_state() {
    let counter = std::cell::RefCell::new(0u32);
    
    let handles: Vec<_> = (0..10).map(|_| {
        std::thread::spawn(|| {
            // RefCell is not thread-safe!
            *counter.borrow_mut() += 1; // This will panic
        })
    }).collect();
}

// GOOD: Use atomic operations
fn safe_shared_state() {
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

## Thread Pool Configuration

### Custom Thread Pool

```rust
fn custom_thread_pool() {
    use std::thread;
    
    let num_threads = 4;
    let mut handles = vec![];
    
    for i in 0..num_threads {
        handles.push(thread::spawn(move || {
            let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
                .unwrap();
            
            loop {
                let mut req = greeter::HelloRequest::new();
                req.set_name(format!("Worker {}", i));
                match client.say_hello(RequestOptions::new(), req).wait() {
                    Ok(reply) => println!("Worker {}: {}", i, reply.get_message()),
                    Err(e) => eprintln!("Worker {} error: {:?}", i, e),
                }
                thread::sleep(std::time::Duration::from_secs(1));
            }
        }));
    }
    
    for handle in handles {
        handle.join().unwrap();
    }
}
```

## Synchronization Primitives

### Using Mutex for Shared Resources

```rust
use std::sync::{Arc, Mutex};

struct SharedState {
    counter: u32,
    last_request: String,
}

fn synchronized_access() {
    let state = Arc::new(Mutex::new(SharedState {
        counter: 0,
        last_request: String::new(),
    }));
    
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .unwrap();
    
    let handles: Vec<_> = (0..5).map(|i| {
        let state = state.clone();
        let client = client.clone();
        
        std::thread::spawn(move || {
            let mut req = greeter::HelloRequest::new();
            req.set_name(format!("User {}", i));
            
            let reply = client.say_hello(RequestOptions::new(), req).wait().unwrap();
            
            let mut state = state.lock().unwrap();
            state.counter += 1;
            state.last_request = reply.get_message().to_string();
        })
    }).collect();
    
    for handle in handles {
        handle.join().unwrap();
    }
}
```

## Deadlock Prevention

### Avoiding Deadlocks

```rust
fn avoid_deadlocks() {
    let lock1 = Arc::new(Mutex::new(1u32));
    let lock2 = Arc::new(Mutex::new(2u32));
    
    // BAD: Potential deadlock
    let handle1 = {
        let lock1 = lock1.clone();
        let lock2 = lock2.clone();
        std::thread::spawn(move || {
            let _g1 = lock1.lock().unwrap();
            std::thread::sleep(std::time::Duration::from_millis(100));
            let _g2 = lock2.lock().unwrap();
        })
    };
    
    let handle2 = {
        let lock1 = lock1.clone();
        let lock2 = lock2.clone();
        std::thread::spawn(move || {
            let _g2 = lock2.lock().unwrap();
            std::thread::sleep(std::time::Duration::from_millis(100));
            let _g1 = lock1.lock().unwrap();
        })
    };
    
    // GOOD: Always acquire locks in same order
    let handle3 = {
        let lock1 = lock1.clone();
        let lock2 = lock2.clone();
        std::thread::spawn(move || {
            let _g1 = lock1.lock().unwrap();
            let _g2 = lock2.lock().unwrap();
        })
    };
}
```

## Channel-Based Communication

### Using Channels for Thread Communication

```rust
fn channel_communication() {
    use std::sync::mpsc;
    
    let (tx, rx) = mpsc::channel();
    
    // Worker thread
    let worker = std::thread::spawn(move || {
        let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
            .unwrap();
        
        for name in rx {
            let mut req = greeter::HelloRequest::new();
            req.set_name(name);
            let reply = client.say_hello(RequestOptions::new(), req).wait().unwrap();
            println!("Processed: {}", reply.get_message());
        }
    });
    
    // Main thread sends work
    for i in 0..10 {
        tx.send(format!("User {}", i)).unwrap();
    }
    
    drop(tx); // Signal worker to stop
    worker.join().unwrap();
}
```

For performance considerations, see [performance.md](performance.md). For lifecycle management, see [lifecycle.md](lifecycle.md).
```
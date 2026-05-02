# Best Practices

```rust
# Best Practices for grpc-rust

## 1. Use Builder Pattern for Configuration

```rust
use grpc::{ServerBuilder, ClientStub};

fn create_production_server() -> grpc::Server {
    let mut server = ServerBuilder::new_plain();
    server.http.set_port(50051);
    server.http.set_max_concurrent_streams(100);
    server.http.set_keepalive_timeout_ms(30000);
    
    // Add services
    server.add_service(greeter_grpc::GreeterServer::new_service_def(GreeterService));
    
    server.build().expect("Failed to build server")
}
```

## 2. Implement Retry Logic

```rust
fn call_with_retry(client: &greeter_grpc::GreeterClient, name: &str, max_retries: u32) -> Result<String, grpc::Error> {
    let mut retries = 0;
    loop {
        let mut req = greeter::HelloRequest::new();
        req.set_name(name.to_string());
        
        match client.say_hello(RequestOptions::new(), req).wait() {
            Ok(reply) => return Ok(reply.get_message().to_string()),
            Err(e) => {
                retries += 1;
                if retries >= max_retries {
                    return Err(e);
                }
                eprintln!("Retry {} after error: {:?}", retries, e);
                std::thread::sleep(std::time::Duration::from_millis(100 * retries));
            }
        }
    }
}
```

## 3. Use Timeouts

```rust
fn call_with_timeout(client: &greeter_grpc::GreeterClient, name: &str, timeout_ms: u64) -> Result<String, grpc::Error> {
    let mut req = greeter::HelloRequest::new();
    req.set_name(name.to_string());
    
    let mut options = RequestOptions::new();
    options.set_timeout_ms(timeout_ms);
    
    client.say_hello(options, req).wait()
        .map(|reply| reply.get_message().to_string())
}
```

## 4. Implement Health Checks

```rust
fn health_check(client: &greeter_grpc::GreeterClient) -> bool {
    let mut req = greeter::HelloRequest::new();
    req.set_name("health".to_string());
    
    match client.say_hello(RequestOptions::new(), req).wait() {
        Ok(_) => true,
        Err(_) => false
    }
}

fn monitor_health(client: greeter_grpc::GreeterClient) {
    std::thread::spawn(move || {
        loop {
            if !health_check(&client) {
                eprintln!("Service unhealthy!");
                // Trigger recovery
            }
            std::thread::sleep(std::time::Duration::from_secs(30));
        }
    });
}
```

## 5. Use Connection Pooling

```rust
use std::sync::Arc;

struct ConnectionPool {
    clients: Vec<greeter_grpc::GreeterClient>,
    current: std::sync::atomic::AtomicUsize,
}

impl ConnectionPool {
    fn new(size: usize, host: &str, port: u16) -> Self {
        let clients: Vec<_> = (0..size)
            .map(|_| greeter_grpc::GreeterClient::new_plain(host, port, Default::default())
                .expect("Failed to create client"))
            .collect();
        
        ConnectionPool {
            clients,
            current: std::sync::atomic::AtomicUsize::new(0),
        }
    }
    
    fn get_client(&self) -> &greeter_grpc::GreeterClient {
        let idx = self.current.fetch_add(1, std::sync::atomic::Ordering::Relaxed) % self.clients.len();
        &self.clients[idx]
    }
}
```

## 6. Logging and Monitoring

```rust
fn instrumented_call(client: &greeter_grpc::GreeterClient, name: &str) -> Result<String, grpc::Error> {
    let start = std::time::Instant::now();
    
    let mut req = greeter::HelloRequest::new();
    req.set_name(name.to_string());
    
    let result = client.say_hello(RequestOptions::new(), req).wait();
    
    let duration = start.elapsed();
    println!("RPC call to say_hello took {:?}", duration);
    
    match &result {
        Ok(reply) => println!("Success: {}", reply.get_message()),
        Err(e) => eprintln!("Failed: {:?}", e),
    }
    
    result.map(|reply| reply.get_message().to_string())
}
```

## 7. Graceful Degradation

```rust
fn fallback_call(client: &greeter_grpc::GreeterClient, fallback_client: &greeter_grpc::GreeterClient, name: &str) -> String {
    let mut req = greeter::HelloRequest::new();
    req.set_name(name.to_string());
    
    match client.say_hello(RequestOptions::new(), req).wait() {
        Ok(reply) => reply.get_message().to_string(),
        Err(_) => {
            eprintln!("Primary failed, using fallback");
            let mut req = greeter::HelloRequest::new();
            req.set_name(name.to_string());
            fallback_client.say_hello(RequestOptions::new(), req)
                .wait()
                .map(|reply| reply.get_message().to_string())
                .unwrap_or_else(|_| "Fallback failed".to_string())
        }
    }
}
```

For thread safety considerations, see [threading.md](threading.md). For performance optimization, see [performance.md](performance.md).
```
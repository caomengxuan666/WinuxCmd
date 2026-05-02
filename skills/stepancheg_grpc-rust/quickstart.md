# Quickstart

```markdown
# Quickstart Guide for grpc-rust

This guide provides copy-pasteable code examples for common gRPC patterns using the `stepancheg/grpc-rust` library.

## Prerequisites

Add to your `Cargo.toml`:
```toml
[dependencies]
grpc = "~0.8"
protobuf = "2.23"
futures = "~0.3"

[build-dependencies]
protoc-rust-grpc = "~0.8"
```

## 1. Basic Server Setup

```rust
// src/main.rs
use grpc::{ServerBuilder, RequestOptions, SingleResponse, Error};
use protobuf::RepeatedField;
use futures::Future;

// Generated proto code
mod greeter;
mod greeter_grpc;

struct GreeterService;

impl greeter_grpc::Greeter for GreeterService {
    fn say_hello(&self, _o: RequestOptions, req: greeter::HelloRequest) -> SingleResponse<greeter::HelloReply> {
        let mut reply = greeter::HelloReply::new();
        reply.set_message(format!("Hello {}", req.get_name()));
        SingleResponse::completed(reply)
    }
}

fn main() {
    let mut server = ServerBuilder::new_plain();
    server.http.set_port(50051);
    server.add_service(greeter_grpc::GreeterServer::new_service_def(GreeterService));
    let _server = server.build().expect("Failed to start server");
    println!("Server listening on port 50051");
    loop { std::thread::park(); }
}
```

## 2. Basic Client Setup

```rust
// src/client.rs
use grpc::{ClientStub, ClientStubExt, RequestOptions};
use futures::Future;

fn main() {
    let client = greeter_grpc::GreeterClient::new_plain("localhost", 50051, Default::default())
        .expect("Failed to create client");
    
    let mut req = greeter::HelloRequest::new();
    req.set_name("World".to_string());
    
    let resp = client.say_hello(RequestOptions::new(), req)
        .wait()
        .expect("RPC failed");
    
    println!("Response: {}", resp.get_message());
}
```

## 3. Unary RPC with Error Handling

```rust
fn call_with_timeout(client: &greeter_grpc::GreeterClient) -> Result<String, String> {
    let mut req = greeter::HelloRequest::new();
    req.set_name("Alice".to_string());
    
    match client.say_hello(RequestOptions::new(), req).wait() {
        Ok(reply) => Ok(reply.get_message().to_string()),
        Err(e) => Err(format!("gRPC error: {:?}", e))
    }
}
```

## 4. Server Streaming

```rust
// Server side
impl greeter_grpc::Greeter for GreeterService {
    fn list_features(&self, _o: RequestOptions, req: greeter::Rectangle) -> grpc::ServerStreamingResponse<greeter::Feature> {
        let f = grpc::ServerStreamingResponse::new(vec![
            create_feature("Feature 1", 1.0, 2.0),
            create_feature("Feature 2", 3.0, 4.0),
        ]);
        grpc::ServerStreamingResponse::new(f)
    }
}

// Client side
fn stream_features(client: &greeter_grpc::GreeterClient) {
    let mut rect = greeter::Rectangle::new();
    // Set rectangle coordinates...
    
    let stream = client.list_features(RequestOptions::new(), rect)
        .wait()
        .expect("Stream failed");
    
    for feature in stream {
        println!("Feature: {}", feature.get_name());
    }
}
```

## 5. Client Streaming

```rust
// Server side
impl greeter_grpc::Greeter for GreeterService {
    fn record_route(&self, _o: RequestOptions, req: grpc::ClientStreamingRequest<greeter::Point>) -> SingleResponse<greeter::RouteSummary> {
        let mut summary = greeter::RouteSummary::new();
        summary.set_point_count(req.len() as i32);
        SingleResponse::completed(summary)
    }
}

// Client side
fn send_points(client: &greeter_grpc::GreeterClient) {
    let points = vec![create_point(1.0, 2.0), create_point(3.0, 4.0)];
    let summary = client.record_route(RequestOptions::new(), points)
        .wait()
        .expect("RPC failed");
    println!("Sent {} points", summary.get_point_count());
}
```

## 6. Bidirectional Streaming

```rust
// Server side
impl greeter_grpc::Greeter for GreeterService {
    fn route_chat(&self, _o: RequestOptions, req: grpc::ClientStreamingRequest<greeter::RouteNote>) -> grpc::ServerStreamingResponse<greeter::RouteNote> {
        let responses: Vec<greeter::RouteNote> = req.into_iter()
            .map(|note| {
                let mut reply = greeter::RouteNote::new();
                reply.set_message(format!("Echo: {}", note.get_message()));
                reply
            })
            .collect();
        grpc::ServerStreamingResponse::new(responses)
    }
}
```

## 7. Using TLS

```rust
use grpc::tls::TlsOptions;

fn create_tls_server() {
    let mut server = ServerBuilder::new_tls();
    server.http.set_port(50052);
    server.tls = Some(TlsOptions {
        cert: include_str!("server.crt").to_string(),
        key: include_str!("server.key").to_string(),
        ..Default::default()
    });
    // Add services...
}

fn create_tls_client() {
    let client = greeter_grpc::GreeterClient::new_tls("localhost", 50052, Default::default())
        .expect("Failed to create TLS client");
}
```

## 8. Graceful Shutdown

```rust
fn main() {
    let mut server = ServerBuilder::new_plain();
    server.http.set_port(50051);
    server.add_service(greeter_grpc::GreeterServer::new_service_def(GreeterService));
    let server = server.build().expect("Failed to start server");
    
    ctrlc::set_handler(move || {
        println!("Shutting down...");
        std::process::exit(0);
    }).expect("Error setting Ctrl-C handler");
    
    loop { std::thread::park(); }
}
```

For more details on thread safety, see [threading.md](threading.md). For performance considerations, see [performance.md](performance.md).
```
# Overview

```markdown
# Overview of grpc-rust

## What is grpc-rust?

`stepancheg/grpc-rust` is a pure Rust implementation of the [gRPC](http://www.grpc.io/) protocol. It provides a framework for building gRPC servers and clients using Rust, with code generation from Protocol Buffers (protobuf) definitions.

## Key Features

- **Pure Rust implementation**: No C++ dependencies, built on top of `rust-http2` for HTTP/2 support
- **Protobuf integration**: Uses `rust-protobuf` for message serialization
- **Code generation**: Supports both `protoc-rust-grpc` crate and `protoc-gen-rust-grpc` plugin
- **Basic gRPC patterns**: Unary, server streaming, client streaming, and bidirectional streaming
- **TLS support**: Secure connections via TLS options

## When to Use

- **Learning gRPC concepts**: Good for understanding gRPC internals
- **Simple internal services**: Suitable for non-critical, low-traffic services
- **Rust-only environments**: When you want to avoid C++ dependencies
- **Prototyping**: Quick experimentation with gRPC in Rust

## When NOT to Use

- **Production systems**: The README explicitly states "not suitable for production use"
- **High-performance requirements**: Performance is listed as a TODO item
- **Complex gRPC features**: Missing advanced features like load balancing, retries, deadlines
- **Long-term projects**: The project is now dead; gRPC team is developing a new implementation

## Key Design Decisions

1. **Futures-based API**: Uses `futures` crate for async operations
2. **Blocking by default**: Most examples use `.wait()` to block on futures
3. **Simple threading model**: Each connection runs in its own thread
4. **Protobuf v2**: Uses `protobuf` crate version 2.23

## Architecture

```
┌─────────────────┐
│  gRPC Layer     │  (grpc crate)
├─────────────────┤
│  HTTP/2 Layer   │  (httpbis/rust-http2)
├─────────────────┤
│  TCP Transport  │  (std::net)
└─────────────────┘
```

## Code Generation

Two approaches for generating Rust code from `.proto` files:

### Method 1: Build Script (Recommended)
```rust
// build.rs
fn main() {
    protoc_rust_grpc::Codegen::new()
        .out_dir("src/protos")
        .inputs(&["proto/helloworld.proto"])
        .include("proto")
        .run()
        .expect("protoc-rust-grpc failed");
}
```

### Method 2: Command Line
```bash
protoc --rust_out=src --rust-grpc_out=src proto/helloworld.proto
```

## Current Status

- **Version**: v0.8.3
- **Status**: Dead project (no longer maintained)
- **Known issues**: Performance, incomplete tests, TODO items in source code

For practical usage patterns, see [quickstart.md](quickstart.md). For common mistakes, see [pitfalls.md](pitfalls.md).
```
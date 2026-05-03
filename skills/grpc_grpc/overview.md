# Overview

gRPC is a high-performance, open-source remote procedure call (RPC) framework developed by Google. It enables client and server applications to communicate transparently using Protocol Buffers as the interface definition language and serialization format. gRPC supports multiple programming languages and runs on top of HTTP/2, providing features like bidirectional streaming, flow control, header compression, and multiplexing.

Use gRPC when you need efficient, strongly-typed communication between microservices, especially in polyglot environments where different services use different languages. It excels in scenarios requiring streaming data, real-time communication, or low-latency RPCs. Avoid gRPC for browser-based applications (use gRPC-Web instead), simple request-response patterns where REST would suffice, or when you need human-readable wire formats.

Key design principles include contract-first development using Protocol Buffers, support for four service types (unary, server streaming, client streaming, bidirectional streaming), built-in authentication and encryption, and pluggable load balancing and tracing. The framework separates interface definition from implementation, allowing services to evolve independently through versioned protobuf schemas.

```cpp
// Defining a service in .proto file
service Greeter {
  rpc SayHello (HelloRequest) returns (HelloReply);
  rpc SayHelloServerStream (HelloRequest) returns (stream HelloReply);
  rpc SayHelloClientStream (stream HelloRequest) returns (HelloReply);
  rpc SayHelloBidiStream (stream HelloRequest) returns (stream HelloReply);
}
```
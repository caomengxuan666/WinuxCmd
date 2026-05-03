# Safety

**Red Line 1: NEVER use insecure credentials in production**

BAD: Insecure production deployment
```cpp
grpc::ServerBuilder builder;
builder.AddListeningPort("0.0.0.0:50051",
                         grpc::InsecureServerCredentials()); // UNSAFE
```

GOOD: Always use SSL/TLS in production
```cpp
grpc::SslServerCredentialsOptions ssl_opts;
ssl_opts.pem_key_cert_pairs.push_back({server_key, server_cert});
builder.AddListeningPort("0.0.0.0:50051",
                         grpc::SslServerCredentials(ssl_opts));
```

**Red Line 2: NEVER ignore gRPC status codes in production code**

BAD: Silently ignoring errors
```cpp
void HandleRpc() {
    grpc::Status status = stub_->DoSomething(&context, request, &reply);
    // No error handling - data corruption risk
}
```

GOOD: Always check and handle status
```cpp
void HandleRpc() {
    grpc::Status status = stub_->DoSomething(&context, request, &reply);
    if (!status.ok()) {
        throw std::runtime_error("RPC failed: " +
                                 status.error_message());
    }
}
```

**Red Line 3: NEVER use the same ClientContext for multiple RPCs**

BAD: Reusing context
```cpp
grpc::ClientContext context;
stub_->Call1(&context, req1, &rep1);
stub_->Call2(&context, req2, &rep2); // UNDEFINED BEHAVIOR
```

GOOD: Create new context for each RPC
```cpp
{
    grpc::ClientContext context1;
    stub_->Call1(&context1, req1, &rep1);
}
{
    grpc::ClientContext context2;
    stub_->Call2(&context2, req2, &rep2);
}
```

**Red Line 4: NEVER call Finish() on a stream before reading all messages**

BAD: Premature finish
```cpp
auto reader = stub_->ServerStreamCall(&context, request);
HelloReply reply;
reader->Read(&reply);
reader->Finish(); // Missing remaining messages
```

GOOD: Read all messages before finishing
```cpp
auto reader = stub_->ServerStreamCall(&context, request);
HelloReply reply;
while (reader->Read(&reply)) {
    ProcessMessage(reply);
}
grpc::Status status = reader->Finish();
```

**Red Line 5: NEVER destroy a stub while RPCs are in progress**

BAD: Destroying stub during active RPC
```cpp
auto stub = Greeter::NewStub(channel);
auto reader = stub->ServerStreamCall(&context, request);
stub.reset(); // UB - RPC still in progress
reader->Read(&reply);
```

GOOD: Ensure all RPCs complete before destroying stub
```cpp
auto stub = Greeter::NewStub(channel);
{
    auto reader = stub->ServerStreamCall(&context, request);
    while (reader->Read(&reply)) {}
    reader->Finish();
}
stub.reset(); // Safe now
```
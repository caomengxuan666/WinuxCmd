# Pitfalls

**Pitfall 1: Not handling gRPC status codes properly**

BAD: Ignoring all error statuses
```cpp
grpc::Status status = stub_->SayHello(&context, request, &reply);
// No error checking - silently continues
```

GOOD: Proper status code handling
```cpp
grpc::Status status = stub_->SayHello(&context, request, &reply);
if (!status.ok()) {
    switch (status.error_code()) {
        case grpc::StatusCode::DEADLINE_EXCEEDED:
            // Retry with backoff
            break;
        case grpc::StatusCode::UNAVAILABLE:
            // Reconnect or failover
            break;
        default:
            LOG(ERROR) << "RPC failed: " << status.error_message();
    }
}
```

**Pitfall 2: Forgetting to call WritesDone on client streams**

BAD: Not signaling end of stream
```cpp
auto writer = stub_->ClientStreamCall(&context, &reply);
for (const auto& item : items) {
    writer->Write(item);
}
// Missing WritesDone - server may hang waiting for more data
```

GOOD: Properly closing client stream
```cpp
auto writer = stub_->ClientStreamCall(&context, &reply);
for (const auto& item : items) {
    if (!writer->Write(item)) {
        break; // Stream broken
    }
}
writer->WritesDone();
grpc::Status status = writer->Finish();
```

**Pitfall 3: Using blocking calls in UI threads**

BAD: Blocking main thread
```cpp
void OnButtonClick() {
    HelloReply reply;
    stub_->SayHello(&context, request, &reply); // Blocks UI
    UpdateUI(reply.message());
}
```

GOOD: Using async API
```cpp
void OnButtonClick() {
    auto* cq = new grpc::CompletionQueue();
    auto* rpc = new AsyncRpc(stub_, cq);
    rpc->Start();
    std::thread([cq, rpc]() {
        void* tag;
        bool ok;
        cq->Next(&tag, &ok);
        auto* completed = static_cast<AsyncRpc*>(tag);
        completed->HandleResponse();
    }).detach();
}
```

**Pitfall 4: Not setting deadlines for RPCs**

BAD: Infinite wait
```cpp
grpc::ClientContext context;
// No deadline set - could hang forever
stub_->SayHello(&context, request, &reply);
```

GOOD: Setting appropriate deadlines
```cpp
grpc::ClientContext context;
context.set_deadline(std::chrono::system_clock::now() +
                     std::chrono::milliseconds(500));
stub_->SayHello(&context, request, &reply);
```

**Pitfall 5: Sharing gRPC channels across threads without synchronization**

BAD: Unsafe channel sharing
```cpp
auto channel = grpc::CreateChannel("localhost:50051",
                                   grpc::InsecureChannelCredentials());
// Multiple threads using same channel without sync
std::thread t1([&]() { GreeterClient c1(channel); });
std::thread t2([&]() { GreeterClient c2(channel); });
```

GOOD: Channels are thread-safe, but stubs are not
```cpp
auto channel = grpc::CreateChannel("localhost:50051",
                                   grpc::InsecureChannelCredentials());
// Each thread creates its own stub from shared channel
std::thread t1([&]() {
    auto stub = Greeter::NewStub(channel);
    GreeterClient c1(std::move(stub));
});
std::thread t2([&]() {
    auto stub = Greeter::NewStub(channel);
    GreeterClient c2(std::move(stub));
});
```

**Pitfall 6: Not handling server shutdown gracefully**

BAD: Abrupt shutdown
```cpp
std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
// ... later
server.reset(); // Forcefully kills all connections
```

GOOD: Graceful shutdown
```cpp
std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
// ... later
server->Shutdown(); // Stop accepting new connections
server->Wait(); // Wait for existing calls to complete
server.reset(); // Safe to destroy now
```
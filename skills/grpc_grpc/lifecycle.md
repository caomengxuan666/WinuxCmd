# Lifecycle

**Construction: Creating channels and stubs**

Channels represent connections to gRPC servers and are created using `grpc::CreateChannel`. Stubs are lightweight objects created from channels and are not thread-safe.

```cpp
// Channel construction - expensive, should be reused
auto channel = grpc::CreateChannel(
    "localhost:50051",
    grpc::InsecureChannelCredentials()
);

// Stub construction - cheap, create per thread or per RPC
auto stub = Greeter::NewStub(channel);
```

**Destruction: Proper cleanup sequence**

Always destroy stubs before channels, and ensure all RPCs complete before destruction.

```cpp
class ClientManager {
public:
    ~ClientManager() {
        // 1. Destroy stubs first
        stub_.reset();
        // 2. Then destroy channel
        channel_.reset();
    }
private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<Greeter::Stub> stub_;
};
```

**Move semantics: Channels and stubs are movable**

```cpp
// Channels are shared_ptr, so move is cheap
auto channel1 = grpc::CreateChannel("localhost:50051",
                                    grpc::InsecureChannelCredentials());
auto channel2 = std::move(channel1); // channel1 is now null

// Stubs are unique_ptr, support move
auto stub1 = Greeter::NewStub(channel2);
auto stub2 = std::move(stub1); // stub1 is now null
```

**Resource management: Server lifecycle**

```cpp
class ServerManager {
public:
    void Start() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort("0.0.0.0:50051",
                                 grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);
        server_ = builder.BuildAndStart();
    }
    
    void Stop() {
        if (server_) {
            server_->Shutdown();  // Stop accepting new connections
            server_->Wait();      // Wait for active calls to complete
            server_.reset();      // Release resources
        }
    }
    
    ~ServerManager() {
        Stop();  // Ensure cleanup
    }
private:
    GreeterServiceImpl service_;
    std::unique_ptr<grpc::Server> server_;
};
```

**Completion queue lifecycle**

```cpp
class AsyncClient {
public:
    AsyncClient(std::shared_ptr<grpc::Channel> channel)
        : channel_(channel),
          stub_(Greeter::NewStub(channel_)),
          cq_(new grpc::CompletionQueue()) {}
    
    ~AsyncClient() {
        cq_->Shutdown();  // Signal completion queue to stop
        void* tag;
        bool ok;
        while (cq_->Next(&tag, &ok)) {
            // Drain remaining events
            delete static_cast<AsyncCall*>(tag);
        }
    }
private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<Greeter::Stub> stub_;
    std::unique_ptr<grpc::CompletionQueue> cq_;
};
```
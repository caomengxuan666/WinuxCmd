# Threading

**Thread safety guarantees**

gRPC channels are thread-safe and can be shared across threads. Stubs are NOT thread-safe and should be used from a single thread or protected by synchronization. Completion queues are thread-safe for adding events but not for reading.

```cpp
// Thread-safe: Channels can be shared
auto channel = grpc::CreateChannel("localhost:50051",
                                   grpc::InsecureChannelCredentials());

// NOT thread-safe: Each thread needs its own stub
std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back([channel]() {
        auto stub = Greeter::NewStub(channel);  // Per-thread stub
        grpc::ClientContext context;
        HelloRequest request;
        HelloReply reply;
        stub->SayHello(&context, request, &reply);
    });
}
```

**Concurrent access patterns: Using completion queues**

```cpp
class ThreadPoolAsyncClient {
public:
    void Start() {
        for (int i = 0; i < num_threads_; ++i) {
            workers_.emplace_back([this]() { WorkerLoop(); });
        }
    }
    
    void EnqueueRpc(const std::string& name) {
        auto* call = new AsyncCall(stub_.get());
        call->Start(name);
    }
    
private:
    void WorkerLoop() {
        void* tag;
        bool ok;
        while (cq_->Next(&tag, &ok)) {
            auto* call = static_cast<AsyncCall*>(tag);
            if (ok) {
                call->HandleResponse();
            }
            delete call;
        }
    }
    
    std::unique_ptr<grpc::CompletionQueue> cq_;
    std::unique_ptr<Greeter::Stub> stub_;
    std::vector<std::thread> workers_;
    int num_threads_ = 4;
};
```

**Synchronization for shared state**

```cpp
class ThreadSafeGreeterClient {
public:
    std::string SayHello(const std::string& name) {
        auto stub = GetStub();
        grpc::ClientContext context;
        HelloRequest request;
        request.set_name(name);
        HelloReply reply;
        grpc::Status status = stub->SayHello(&context, request, &reply);
        if (status.ok()) return reply.message();
        throw std::runtime_error(status.error_message());
    }
    
private:
    Greeter::Stub* GetStub() {
        // Thread-local stubs for performance
        thread_local auto stub = Greeter::NewStub(channel_);
        return stub.get();
    }
    
    std::shared_ptr<grpc::Channel> channel_;
};
```

**Server-side threading model**

```cpp
class ThreadPoolServer {
public:
    void Start() {
        grpc::ServerBuilder builder;
        builder.AddListeningPort("0.0.0.0:50051",
                                 grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);
        
        // Configure thread pool
        builder.SetSyncServerOption(
            grpc::ServerBuilder::SyncServerOption::NUM_CQS, 4);
        builder.SetSyncServerOption(
            grpc::ServerBuilder::SyncServerOption::MIN_POLLERS, 2);
        builder.SetSyncServerOption(
            grpc::ServerBuilder::SyncServerOption::MAX_POLLERS, 8);
        
        server_ = builder.BuildAndStart();
        server_->Wait();
    }
    
private:
    GreeterServiceImpl service_;
    std::unique_ptr<grpc::Server> server_;
};
```

**Atomic operations for shared counters**

```cpp
class MetricsCollector {
public:
    void RecordRpc() {
        total_rpcs_.fetch_add(1, std::memory_order_relaxed);
    }
    
    void RecordLatency(std::chrono::microseconds latency) {
        auto current = total_latency_.load(std::memory_order_relaxed);
        while (!total_latency_.compare_exchange_weak(
            current, current + latency.count(),
            std::memory_order_relaxed)) {}
    }
    
    double AverageLatency() {
        auto total = total_rpcs_.load(std::memory_order_acquire);
        if (total == 0) return 0.0;
        return static_cast<double>(
            total_latency_.load(std::memory_order_acquire)) / total;
    }
    
private:
    std::atomic<uint64_t> total_rpcs_{0};
    std::atomic<uint64_t> total_latency_{0};
};
```
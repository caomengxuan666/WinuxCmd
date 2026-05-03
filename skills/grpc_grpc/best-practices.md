# Best Practices

**Practice 1: Use connection pooling with channel reuse**

```cpp
class GrpcConnectionPool {
public:
    std::shared_ptr<grpc::Channel> GetChannel(const std::string& target) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = channels_.find(target);
        if (it == channels_.end()) {
            auto channel = grpc::CreateChannel(target,
                grpc::InsecureChannelCredentials());
            channels_[target] = channel;
            return channel;
        }
        return it->second;
    }
private:
    std::mutex mutex_;
    std::unordered_map<std::string,
        std::shared_ptr<grpc::Channel>> channels_;
};
```

**Practice 2: Implement retry with exponential backoff**

```cpp
class RetryingClient {
public:
    std::string CallWithRetry(const std::string& name, int max_retries = 3) {
        for (int i = 0; i < max_retries; ++i) {
            grpc::ClientContext context;
            context.set_deadline(std::chrono::system_clock::now() +
                                 std::chrono::milliseconds(100 * (1 << i)));
            HelloRequest request;
            request.set_name(name);
            HelloReply reply;
            grpc::Status status = stub_->SayHello(&context, request, &reply);
            if (status.ok()) return reply.message();
            if (status.error_code() == grpc::StatusCode::UNAVAILABLE) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100 * (1 << i)));
                continue;
            }
            throw std::runtime_error(status.error_message());
        }
        throw std::runtime_error("Max retries exceeded");
    }
};
```

**Practice 3: Use async API for high-throughput servers**

```cpp
class AsyncGreeterServer {
public:
    void Start() {
        builder_.AddListeningPort("0.0.0.0:50051",
                                  grpc::InsecureServerCredentials());
        builder_.RegisterService(&service_);
        cq_ = builder_.AddCompletionQueue();
        server_ = builder_.BuildAndStart();
        HandleRpcs();
    }
private:
    void HandleRpcs() {
        new CallData(&service_, cq_.get());
        void* tag;
        bool ok;
        while (cq_->Next(&tag, &ok)) {
            static_cast<CallData*>(tag)->Proceed(ok);
        }
    }
    grpc::ServerBuilder builder_;
    Greeter::AsyncService service_;
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;
};
```

**Practice 4: Implement health checks**

```cpp
#include <grpcpp/health_check_service_interface.h>

void SetupHealthCheck() {
    grpc::EnableDefaultHealthCheckService(true);
    auto health_service = grpc::DefaultHealthCheckService();
    health_service->SetServingStatus("", true);
}

// Client-side health check
bool IsServerHealthy(const std::string& target) {
    auto channel = grpc::CreateChannel(target,
        grpc::InsecureChannelCredentials());
    auto health_stub = grpc::health::v1::Health::NewStub(channel);
    grpc::ClientContext context;
    grpc::health::v1::HealthCheckRequest request;
    grpc::health::v1::HealthCheckResponse response;
    request.set_service("");
    grpc::Status status = health_stub->Check(&context, request, &response);
    return status.ok() &&
           response.status() ==
               grpc::health::v1::HealthCheckResponse::SERVING;
}
```

**Practice 5: Use interceptors for cross-cutting concerns**

```cpp
class LoggingInterceptor : public grpc::experimental::Interceptor {
public:
    void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override {
        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
            auto* metadata = methods->GetSendInitialMetadata();
            metadata->insert({"request_id", GenerateUUID()});
        }
        methods->Proceed();
    }
};

class LoggingInterceptorFactory
    : public grpc::experimental::ClientInterceptorFactory {
public:
    grpc::experimental::Interceptor* CreateClientInterceptor() override {
        return new LoggingInterceptor();
    }
};
```
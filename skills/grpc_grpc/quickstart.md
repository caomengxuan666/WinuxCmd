# Quickstart

```cpp
#include <grpcpp/grpcpp.h>
#include "helloworld.grpc.pb.h"

// Pattern 1: Creating a simple synchronous client
class GreeterClient {
public:
    GreeterClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}
    
    std::string SayHello(const std::string& user) {
        HelloRequest request;
        request.set_name(user);
        HelloReply reply;
        grpc::ClientContext context;
        grpc::Status status = stub_->SayHello(&context, request, &reply);
        if (status.ok()) return reply.message();
        return "RPC failed";
    }
private:
    std::unique_ptr<Greeter::Stub> stub_;
};

// Pattern 2: Creating a simple synchronous server
class GreeterServiceImpl final : public Greeter::Service {
    grpc::Status SayHello(grpc::ServerContext* context,
                          const HelloRequest* request,
                          HelloReply* reply) override {
        reply->set_message("Hello " + request->name());
        return grpc::Status::OK;
    }
};

// Pattern 3: Starting the server
void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();
}

// Pattern 4: Client-side streaming
class ClientStreamClient {
public:
    explicit ClientStreamClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}
    
    void SendStream(const std::vector<std::string>& names) {
        grpc::ClientContext context;
        HelloReply reply;
        auto writer = stub_->SayHelloClientStream(&context, &reply);
        for (const auto& name : names) {
            HelloRequest request;
            request.set_name(name);
            writer->Write(request);
        }
        writer->WritesDone();
        grpc::Status status = writer->Finish();
    }
private:
    std::unique_ptr<Greeter::Stub> stub_;
};

// Pattern 5: Server-side streaming
class ServerStreamClient {
public:
    explicit ServerStreamClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}
    
    void ReceiveStream(const std::string& name) {
        HelloRequest request;
        request.set_name(name);
        grpc::ClientContext context;
        auto reader = stub_->SayHelloServerStream(&context, request);
        HelloReply reply;
        while (reader->Read(&reply)) {
            std::cout << reply.message() << std::endl;
        }
        grpc::Status status = reader->Finish();
    }
private:
    std::unique_ptr<Greeter::Stub> stub_;
};

// Pattern 6: Bidirectional streaming
class BidiStreamClient {
public:
    explicit BidiStreamClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}
    
    void BidirectionalStream(const std::vector<std::string>& names) {
        grpc::ClientContext context;
        auto stream = stub_->SayHelloBidiStream(&context);
        std::thread writer([&stream, &names]() {
            for (const auto& name : names) {
                HelloRequest request;
                request.set_name(name);
                stream->Write(request);
            }
            stream->WritesDone();
        });
        HelloReply reply;
        while (stream->Read(&reply)) {
            std::cout << reply.message() << std::endl;
        }
        writer.join();
        grpc::Status status = stream->Finish();
    }
private:
    std::unique_ptr<Greeter::Stub> stub_;
};

// Pattern 7: Using SSL/TLS credentials
std::shared_ptr<grpc::Channel> CreateSecureChannel() {
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = LoadRootCerts();
    auto channel = grpc::CreateChannel("localhost:50051",
                                       grpc::SslCredentials(ssl_opts));
    return channel;
}

// Pattern 8: Setting deadlines
void CallWithDeadline(Greeter::Stub* stub) {
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() +
                         std::chrono::seconds(5));
    HelloRequest request;
    request.set_name("world");
    HelloReply reply;
    grpc::Status status = stub->SayHello(&context, request, &reply);
}
```
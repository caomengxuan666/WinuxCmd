# Performance

**Performance characteristics**

gRPC uses HTTP/2 multiplexing, allowing multiple concurrent RPCs over a single TCP connection. Channels maintain connection pools and automatically handle load balancing. Protocol Buffers provide efficient binary serialization, typically 3-10x faster than JSON.

```cpp
// Measure serialization performance
void BenchmarkSerialization() {
    HelloRequest request;
    request.set_name(std::string(1024, 'a'));
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        std::string data;
        request.SerializeToString(&data);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();
    std::cout << "Serialization time: " << duration << "ms" << std::endl;
}
```

**Allocation patterns: Minimize allocations in hot paths**

```cpp
// BAD: Allocating new messages for each RPC
void ProcessManyRequests(const std::vector<std::string>& names) {
    for (const auto& name : names) {
        HelloRequest request;  // Allocation on each iteration
        request.set_name(name);
        // Process request
    }
}

// GOOD: Reuse messages
void ProcessManyRequests(const std::vector<std::string>& names) {
    HelloRequest request;
    for (const auto& name : names) {
        request.Clear();  // Reuse allocated memory
        request.set_name(name);
        // Process request
    }
}
```

**Optimization tips: Use streaming for large data**

```cpp
// BAD: Sending large data in a single message
void SendLargeData(const std::vector<char>& data) {
    DataRequest request;
    request.set_data(data.data(), data.size());  // Large allocation
    stub_->SendData(&context, request, &reply);
}

// GOOD: Stream data in chunks
void SendLargeData(const std::vector<char>& data) {
    auto writer = stub_->SendDataStream(&context, &reply);
    const size_t chunk_size = 64 * 1024;  // 64KB chunks
    for (size_t i = 0; i < data.size(); i += chunk_size) {
        DataChunk chunk;
        size_t size = std::min(chunk_size, data.size() - i);
        chunk.set_data(data.data() + i, size);
        if (!writer->Write(chunk)) {
            break;  // Stream broken
        }
    }
    writer->WritesDone();
    writer->Finish();
}
```

**Connection pooling and reuse**

```cpp
// Configure channel for optimal performance
grpc::ChannelArguments args;
args.SetInt(GRPC_ARG_MAX_RECONNECT_BACKOFF_MS, 1000);
args.SetInt(GRPC_ARG_MIN_RECONNECT_BACKOFF_MS, 100);
args.SetInt(GRPC_ARG_INITIAL_RECONNECT_BACKOFF_MS, 100);
args.SetInt(GRPC_ARG_MAX_CONCURRENT_STREAMS, 100);
args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);
args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);

auto channel = grpc::CreateCustomChannel(
    "localhost:50051",
    grpc::InsecureChannelCredentials(),
    args
);
```

**Buffer management for streaming**

```cpp
// Configure flow control for high-throughput streaming
grpc::ChannelArguments args;
args.SetInt(GRPC_ARG_HTTP2_BDP_PROBE, 1);
args.SetInt(GRPC_ARG_HTTP2_MIN_RECV_BYTES_PER_CHUNK, 65536);
args.SetInt(GRPC_ARG_HTTP2_MIN_SEND_BYTES_PER_CHUNK, 65536);
args.SetInt(GRPC_ARG_HTTP2_MAX_FRAME_SIZE, 16777215);  // 16MB max frame
```
# Performance

### Allocation patterns and optimization
```cpp
// BAD: Frequent allocations in loops
for (int i = 0; i < 100000; ++i) {
    std::string data;
    message.SerializeToString(&data);  // Allocates each iteration
    Process(data);
}

// GOOD: Reuse buffer
std::string data;
for (int i = 0; i < 100000; ++i) {
    data.clear();
    message.SerializeToString(&data);
    Process(data);
}
```

### Arena allocation for batch processing
```cpp
// BAD: Individual allocations
for (int i = 0; i < 1000; ++i) {
    auto msg = std::make_unique<MyMessage>();
    msg->set_value(i);
    Process(msg.get());
}

// GOOD: Arena allocation
google::protobuf::Arena arena;
std::vector<const MyMessage*> messages;
for (int i = 0; i < 1000; ++i) {
    auto* msg = google::protobuf::Arena::CreateMessage<MyMessage>(&arena);
    msg->set_value(i);
    messages.push_back(msg);
}
// Single deallocation when arena goes out of scope
```

### Pre-sizing for repeated fields
```cpp
// BAD: Growing repeated field incrementally
for (int i = 0; i < 10000; ++i) {
    msg.add_values(i);  // May reallocate multiple times
}

// GOOD: Reserve capacity
msg.mutable_values()->Reserve(10000);
for (int i = 0; i < 10000; ++i) {
    msg.add_values(i);  // No reallocation
}
```

### Zero-copy deserialization
```cpp
// BAD: Copying data for deserialization
std::vector<char> buffer = ReadData();
MyMessage msg;
msg.ParseFromArray(buffer.data(), buffer.size());  // Copies internally

// GOOD: Use string for zero-copy (C++11+)
std::string data = ReadDataAsString();
MyMessage msg;
msg.ParseFromString(data);  // May avoid copy with SSO
```

### Batch serialization
```cpp
// BAD: Serializing each message separately
for (const auto& msg : messages) {
    std::string data;
    msg.SerializeToString(&data);
    WriteToNetwork(data);
}

// GOOD: Batch serialize
std::string batch;
for (const auto& msg : messages) {
    msg.AppendToString(&batch);  // Append to existing string
}
WriteToNetwork(batch);
```

### Using reflection for dynamic access
```cpp
// BAD: Reflection for every access (slow)
const Reflection* reflection = msg.GetReflection();
const FieldDescriptor* field = descriptor->FindFieldByName("name");
for (int i = 0; i < 1000; ++i) {
    std::string name = reflection->GetString(msg, field);
}

// GOOD: Cache reflection results
const Reflection* reflection = msg.GetReflection();
const FieldDescriptor* field = descriptor->FindFieldByName("name");
std::string cached_name = reflection->GetString(msg, field);
for (int i = 0; i < 1000; ++i) {
    Process(cached_name);  // Use cached value
}
```
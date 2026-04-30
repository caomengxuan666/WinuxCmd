# Pitfalls

```cpp
// PITFALL 1: Forgetting to include MSGPACK_DEFINE for custom types
class Point {
public:
    int x, y;
    // BAD: Missing MSGPACK_DEFINE
};

void bad_serialize() {
    Point p{1, 2};
    std::stringstream buffer;
    // msgpack::pack(buffer, p); // COMPILE ERROR: no serialization defined
}

class GoodPoint {
public:
    int x, y;
    MSGPACK_DEFINE(x, y); // GOOD: Define serialization
};

void good_serialize() {
    GoodPoint p{1, 2};
    std::stringstream buffer;
    msgpack::pack(buffer, p); // Works correctly
}

// PITFALL 2: Using unpacked object after buffer is destroyed
void dangling_reference() {
    std::stringstream buffer;
    msgpack::pack(buffer, std::string("hello"));
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    
    // BAD: buffer.str() creates temporary, data() pointer becomes invalid
    // msgpack::unpack(msg, buffer.str().data(), buffer.str().size()); // Dangling pointer!
    
    // GOOD: Keep buffer alive
    std::string data = buffer.str();
    msgpack::unpack(msg, data.data(), data.size());
    std::string result = msg.get().as<std::string>(); // Safe
}

// PITFALL 3: Type mismatch during deserialization
void type_mismatch() {
    std::stringstream buffer;
    msgpack::pack(buffer, 42);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    
    // BAD: Will throw msgpack::type_error
    // std::string result = msg.get().as<std::string>();
    
    // GOOD: Check type first
    if (msg.get().type == msgpack::type::POSITIVE_INTEGER) {
        int result = msg.get().as<int>();
    }
}

// PITFALL 4: Not handling incomplete data in streaming
void incomplete_data() {
    std::stringstream buffer;
    msgpack::pack(buffer, 42);
    std::string partial = buffer.str().substr(0, 2); // Only first 2 bytes
    
    msgpack::unpacker pac;
    // BAD: Will fail silently or corrupt
    pac.reserve_buffer(partial.size());
    memcpy(pac.buffer(), partial.data(), partial.size());
    pac.buffer_consumed(partial.size());
    
    msgpack::unpacked result;
    // GOOD: Check if unpack was successful
    while (pac.next(result)) {
        // Process complete message
    }
}

// PITFALL 5: Serializing large binary data without optimization
void large_binary() {
    std::vector<char> large_data(1024 * 1024); // 1MB
    
    // BAD: Copies data multiple times
    std::stringstream buffer;
    msgpack::pack(buffer, large_data);
    
    // GOOD: Use raw_ref for zero-copy
    msgpack::type::raw_ref ref(large_data.data(), large_data.size());
    msgpack::pack(buffer, ref);
}

// PITFALL 6: Not handling nested object memory correctly
void nested_memory() {
    std::stringstream buffer;
    std::vector<std::vector<int>> nested = {{1, 2}, {3, 4}};
    msgpack::pack(buffer, nested);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    
    // BAD: Object references buffer, which may be destroyed
    msgpack::object obj = msg.get();
    // buffer.str() creates temporary!
    
    // GOOD: Convert to owned data
    msgpack::zone z;
    msgpack::object owned_obj(obj, z);
    auto result = owned_obj.as<std::vector<std::vector<int>>>();
}
```
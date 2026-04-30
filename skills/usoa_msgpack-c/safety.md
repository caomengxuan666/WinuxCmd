# Safety

```cpp
// RED LINE 1: NEVER use unpacked object after source buffer is destroyed
void unsafe_buffer_lifetime() {
    msgpack::unpacked msg;
    {
        std::string buffer;
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, 42);
        buffer.assign(sbuf.data(), sbuf.size());
        
        // BAD: msg.get() references buffer's memory
        msgpack::unpack(msg, buffer.data(), buffer.size());
        // buffer destroyed here!
    }
    // msg.get().as<int>(); // UNDEFINED BEHAVIOR - buffer is gone
    
    // GOOD: Copy data immediately
    int value = msg.get().as<int>(); // Safe if done before buffer destruction
}

// RED LINE 2: NEVER pass null or invalid pointers to unpack functions
void invalid_pointer() {
    // BAD: Passing null pointer
    // msgpack::unpack(msg, nullptr, 0); // Undefined behavior
    
    // GOOD: Always check data validity
    const char* data = get_data_safely();
    size_t size = get_size_safely();
    if (data != nullptr && size > 0) {
        msgpack::unpack(msg, data, size);
    }
}

// RED LINE 3: NEVER ignore msgpack::type_error exceptions
void ignore_type_error() {
    std::stringstream buffer;
    msgpack::pack(buffer, 42.5); // Double
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    
    // BAD: Will throw unhandled exception
    // int value = msg.get().as<int>(); // Throws msgpack::type_error
    
    // GOOD: Handle type mismatch
    try {
        int value = msg.get().as<int>();
    } catch (const msgpack::type_error& e) {
        // Handle type mismatch gracefully
        double value = msg.get().as<double>();
    }
}

// RED LINE 4: NEVER use MSGPACK_DEFINE with non-copyable types
class NonCopyable {
public:
    std::unique_ptr<int> data;
    // BAD: MSGPACK_DEFINE(data); // Won't compile with unique_ptr
};

// GOOD: Implement custom serialization
class CustomSerializable {
public:
    std::unique_ptr<int> data;
    template <typename Packer>
    void msgpack_pack(Packer& pk) const {
        pk.pack_array(1);
        pk.pack(*data);
    }
    void msgpack_unpack(msgpack::object o) {
        if (o.type != msgpack::type::ARRAY || o.via.array.size != 1) {
            throw msgpack::type_error();
        }
        data = std::make_unique<int>(o.via.array.ptr[0].as<int>());
    }
};

// RED LINE 5: NEVER assume msgpack::unpack returns valid data without checking
void unchecked_unpack() {
    std::string corrupted_data = "\x01\x02\x03"; // Invalid msgpack
    
    msgpack::unpacked msg;
    // BAD: No error checking
    // msgpack::unpack(msg, corrupted_data.data(), corrupted_data.size());
    
    // GOOD: Use try-catch for malformed data
    try {
        msgpack::unpack(msg, corrupted_data.data(), corrupted_data.size());
    } catch (const msgpack::unpack_error& e) {
        // Handle malformed data
        std::cerr << "Failed to unpack: " << e.what() << std::endl;
    }
}
```
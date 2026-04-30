# Best Practices

```cpp
// BEST PRACTICE 1: Use sbuffer for efficient serialization
#include <msgpack.hpp>
#include <iostream>

void efficient_serialization() {
    // GOOD: sbuffer avoids stringstream overhead
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, 42);
    msgpack::pack(buffer, std::string("hello"));
    
    // Direct access to contiguous memory
    const char* data = buffer.data();
    size_t size = buffer.size();
}

// BEST PRACTICE 2: Implement custom serialization for complex types
class User {
    std::string username;
    int age;
    std::vector<std::string> roles;
    
public:
    // Manual serialization for better control
    template <typename Packer>
    void msgpack_pack(Packer& pk) const {
        pk.pack_array(3);
        pk.pack(username);
        pk.pack(age);
        pk.pack(roles);
    }
    
    void msgpack_unpack(msgpack::object o) {
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
        if (o.via.array.size != 3) throw msgpack::type_error();
        
        username = o.via.array.ptr[0].as<std::string>();
        age = o.via.array.ptr[1].as<int>();
        roles = o.via.array.ptr[2].as<std::vector<std::string>>();
    }
    
    void msgpack_object(msgpack::object* o, msgpack::zone& z) const {
        // For zero-copy object creation
        o->type = msgpack::type::ARRAY;
        o->via.array.size = 3;
        o->via.array.ptr = static_cast<msgpack::object*>(
            z.allocate_align(sizeof(msgpack::object) * 3));
        
        o->via.array.ptr[0] = msgpack::object(username, z);
        o->via.array.ptr[1] = msgpack::object(age, z);
        o->via.array.ptr[2] = msgpack::object(roles, z);
    }
};

// BEST PRACTICE 3: Use object_with_zone for owned deserialization
void owned_deserialization() {
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, std::vector<int>{1, 2, 3});
    
    // Create owned object that doesn't depend on buffer
    msgpack::object_handle oh = msgpack::unpack(buffer.data(), buffer.size());
    msgpack::object obj = oh.get();
    // oh owns the data, safe to use obj even after buffer is destroyed
}

// BEST PRACTICE 4: Version your serialized data
class VersionedData {
    static constexpr uint32_t CURRENT_VERSION = 2;
    uint32_t version = CURRENT_VERSION;
    std::string payload;
    
public:
    MSGPACK_DEFINE(version, payload);
    
    void upgrade() {
        if (version < 2) {
            // Handle migration logic
            version = CURRENT_VERSION;
        }
    }
};

// BEST PRACTICE 5: Use streaming for large messages
void streaming_large_data() {
    msgpack::unpacker pac;
    msgpack::sbuffer buffer;
    
    // Pack multiple messages
    for (int i = 0; i < 1000; ++i) {
        msgpack::pack(buffer, i);
    }
    
    // Feed data incrementally
    const char* data = buffer.data();
    size_t remaining = buffer.size();
    
    while (remaining > 0) {
        size_t chunk_size = std::min(remaining, size_t(1024));
        pac.reserve_buffer(chunk_size);
        memcpy(pac.buffer(), data, chunk_size);
        pac.buffer_consumed(chunk_size);
        
        msgpack::unpacked result;
        while (pac.next(result)) {
            // Process each complete message
            int value = result.get().as<int>();
        }
        
        data += chunk_size;
        remaining -= chunk_size;
    }
}

// BEST PRACTICE 6: Use type aliases for complex types
using UserMap = std::map<std::string, User>;
using UserList = std::vector<User>;

void type_alias_serialization() {
    UserMap users;
    users["alice"] = User();
    
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, users);
    
    auto result = msgpack::unpack(buffer.data(), buffer.size())
        .get().as<UserMap>();
}
```
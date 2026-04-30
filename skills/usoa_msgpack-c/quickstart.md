# Quickstart

```cpp
// Basic serialization and deserialization with msgpack-c
#include <msgpack.hpp>
#include <vector>
#include <string>
#include <iostream>

// Pattern 1: Serialize primitive types
void serialize_primitives() {
    std::stringstream buffer;
    int value = 42;
    msgpack::pack(buffer, value);
    
    // Deserialize
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    int result = msg.get().as<int>();
    std::cout << "Deserialized: " << result << std::endl; // 42
}

// Pattern 2: Serialize containers
void serialize_containers() {
    std::stringstream buffer;
    std::vector<int> vec = {1, 2, 3, 4, 5};
    msgpack::pack(buffer, vec);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    std::vector<int> result = msg.get().as<std::vector<int>>();
    for (int v : result) std::cout << v << " "; // 1 2 3 4 5
}

// Pattern 3: Serialize custom objects with macro
class Person {
public:
    std::string name;
    int age;
    MSGPACK_DEFINE(name, age); // Macro for automatic serialization
};

void serialize_custom_object() {
    Person p{"Alice", 30};
    std::stringstream buffer;
    msgpack::pack(buffer, p);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    Person result = msg.get().as<Person>();
    std::cout << result.name << " " << result.age; // Alice 30
}

// Pattern 4: Serialize to/from buffer directly
void buffer_serialization() {
    std::vector<char> buffer(1024);
    int value = 42;
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, value);
    
    // Copy to your buffer
    buffer.assign(sbuf.data(), sbuf.data() + sbuf.size());
}

// Pattern 5: Deserialize with type checking
void type_safe_deserialization() {
    std::stringstream buffer;
    msgpack::pack(buffer, 42);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    
    msgpack::object obj = msg.get();
    if (obj.type == msgpack::type::POSITIVE_INTEGER) {
        int value = obj.as<int>();
        std::cout << "Integer: " << value;
    }
}

// Pattern 6: Serialize map/dictionary
void serialize_map() {
    std::map<std::string, int> dict = {{"one", 1}, {"two", 2}};
    std::stringstream buffer;
    msgpack::pack(buffer, dict);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    auto result = msg.get().as<std::map<std::string, int>>();
    for (auto& [k, v] : result) std::cout << k << ":" << v << " "; // one:1 two:2
}

// Pattern 7: Serialize with object zone (for zero-copy)
void zone_serialization() {
    std::stringstream buffer;
    std::string data = "hello";
    msgpack::pack(buffer, data);
    
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer.str().data(), buffer.str().size());
    
    // Use zone to manage memory
    msgpack::zone z;
    msgpack::object copied_obj(msg.get(), z);
    // copied_obj is now independent of original buffer
}
```
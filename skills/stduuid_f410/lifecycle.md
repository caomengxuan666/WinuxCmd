# Lifecycle

```cpp
// Construction: Default, from bytes, from string, from generator
#include <uuid.h>
#include <iostream>

int main() {
    // Default construction (nil UUID)
    uuids::uuid nil_id;
    std::cout << "Nil UUID: " << uuids::to_string(nil_id) << std::endl;
    
    // From byte array
    std::array<uint8_t, 16> bytes = {0};
    uuids::uuid from_bytes(bytes);
    
    // From string
    auto from_str = uuids::uuid::from_string("550e8400-e29b-41d4-a716-446655440000");
    
    // From generator
    uuids::uuid_system_generator gen;
    uuids::uuid generated = gen();
    
    return 0;
}
```

```cpp
// Move semantics and copying
#include <uuid.h>
#include <vector>

int main() {
    std::vector<uuids::uuid> ids;
    
    // Copy construction
    uuids::uuid id1 = uuids::uuid_system_generator{}();
    uuids::uuid id2 = id1; // Copy - both are valid
    
    // Move construction
    uuids::uuid id3 = std::move(id1); // id1 is now in valid but unspecified state
    
    // Copy assignment
    id2 = id3;
    
    // Move assignment
    id1 = std::move(id3); // id3 is now in valid but unspecified state
    
    ids.push_back(uuids::uuid_system_generator{}()); // Efficient move into vector
    
    return 0;
}
```

```cpp
// Resource management - UUIDs are value types
#include <uuid.h>
#include <memory>

class ResourceWithUuid {
    uuids::uuid id_;
    std::unique_ptr<int> data_;
public:
    ResourceWithUuid() 
        : id_(uuids::uuid_system_generator{}())
        , data_(std::make_unique<int>(42)) {}
    
    // Default copy is fine for UUID
    ResourceWithUuid(const ResourceWithUuid& other)
        : id_(other.id_)
        , data_(std::make_unique<int>(*other.data_)) {}
    
    const uuids::uuid& get_id() const { return id_; }
};
```

```cpp
// Destruction - no special cleanup needed
#include <uuid.h>
#include <iostream>

class Logger {
    uuids::uuid session_id_;
public:
    Logger() : session_id_(uuids::uuid_system_generator{}()) {
        std::cout << "Session started: " << uuids::to_string(session_id_) << std::endl;
    }
    
    ~Logger() {
        std::cout << "Session ended: " << uuids::to_string(session_id_) << std::endl;
        // UUID destructor is trivial - no cleanup needed
    }
};
```
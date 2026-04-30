# Lifecycle

```cpp
// Construction and destruction patterns
#include <msgpack.hpp>
#include <memory>

// Pattern 1: Stack allocation (preferred)
void stack_lifecycle() {
    // Construction on stack
    msgpack::sbuffer buffer;  // Default constructed, empty
    msgpack::pack(buffer, 42);
    
    // Automatically destroyed when out of scope
    // No explicit cleanup needed
}

// Pattern 2: Heap allocation with unique_ptr
void heap_lifecycle() {
    auto buffer = std::make_unique<msgpack::sbuffer>();
    msgpack::pack(*buffer, 42);
    
    // Move semantics
    auto buffer2 = std::move(buffer);
    // buffer is now null, buffer2 owns the data
    
    // Explicit reset
    buffer2.reset(); // Destroys the buffer
}

// Pattern 3: Object handle lifecycle
void object_handle_lifecycle() {
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, std::vector<int>{1, 2, 3});
    
    // unpack returns object_handle which owns the data
    msgpack::object_handle oh = msgpack::unpack(buffer.data(), buffer.size());
    
    // Move handle to extend lifetime
    msgpack::object_handle oh2 = std::move(oh);
    // oh is now empty, oh2 owns the data
    
    // Access object through handle
    msgpack::object obj = oh2.get();
    std::vector<int> vec = obj.as<std::vector<int>>();
    
    // When oh2 goes out of scope, data is freed
}

// Pattern 4: Zone-based memory management
void zone_memory_management() {
    msgpack::zone zone;  // Memory pool
    
    // Create objects in zone
    msgpack::object obj1(42, zone);
    msgpack::object obj2(std::string("hello"), zone);
    
    // Zone owns all allocated memory
    // Objects remain valid as long as zone exists
    
    // Copy zone (expensive, avoid)
    // msgpack::zone zone2 = zone; // Copies all data
    
    // Move zone (efficient)
    msgpack::zone zone3 = std::move(zone);
    // zone is now empty, zone3 owns the memory
    
    // Zone automatically frees all memory on destruction
}

// Pattern 5: Resource management with RAII
class MessagePackWrapper {
    msgpack::sbuffer buffer_;
    msgpack::object_handle handle_;
    
public:
    MessagePackWrapper() = default;
    
    // Move constructor
    MessagePackWrapper(MessagePackWrapper&& other) noexcept
        : buffer_(std::move(other.buffer_))
        , handle_(std::move(other.handle_)) {}
    
    // Move assignment
    MessagePackWrapper& operator=(MessagePackWrapper&& other) noexcept {
        if (this != &other) {
            buffer_ = std::move(other.buffer_);
            handle_ = std::move(other.handle_);
        }
        return *this;
    }
    
    // Deleted copy constructor/assignment
    MessagePackWrapper(const MessagePackWrapper&) = delete;
    MessagePackWrapper& operator=(const MessagePackWrapper&) = delete;
    
    void pack(const auto& value) {
        msgpack::pack(buffer_, value);
    }
    
    void unpack() {
        handle_ = msgpack::unpack(buffer_.data(), buffer_.size());
    }
    
    // Destructor is implicit, handles cleanup
};

// Pattern 6: Serialization of move-only types
class MoveOnlyResource {
    std::unique_ptr<int> data_;
    
public:
    MoveOnlyResource() : data_(std::make_unique<int>(42)) {}
    
    // Move constructor
    MoveOnlyResource(MoveOnlyResource&& other) noexcept
        : data_(std::move(other.data_)) {}
    
    // Custom serialization (can't use MSGPACK_DEFINE with unique_ptr)
    template <typename Packer>
    void msgpack_pack(Packer& pk) const {
        pk.pack(*data_);
    }
    
    void msgpack_unpack(msgpack::object o) {
        data_ = std::make_unique<int>(o.as<int>());
    }
};
```
# Lifecycle

```cpp
// Construction: Snappy is stateless - no objects to construct
// Just include the header and use functions directly
#include <snappy.h>

// Resource management: Snappy manages no resources internally
// All memory management is the caller's responsibility

// Example of proper resource management
class CompressedBuffer {
    std::vector<char> buffer;
    size_t compressed_size;
public:
    CompressedBuffer(const std::string& input) {
        size_t max_size = snappy::MaxCompressedLength(input.size());
        buffer.resize(max_size);
        snappy::RawCompress(input.data(), input.size(), buffer.data(), &compressed_size);
    }
    
    // Move semantics
    CompressedBuffer(CompressedBuffer&& other) noexcept 
        : buffer(std::move(other.buffer)), compressed_size(other.compressed_size) {}
    
    CompressedBuffer& operator=(CompressedBuffer&& other) noexcept {
        if (this != &other) {
            buffer = std::move(other.buffer);
            compressed_size = other.compressed_size;
        }
        return *this;
    }
    
    // No custom destructor needed - vector handles cleanup
    const char* data() const { return buffer.data(); }
    size_t size() const { return compressed_size; }
};
```

```cpp
// Destruction: No special cleanup needed
{
    std::string compressed;
    snappy::Compress("data", 4, &compressed);
    // compressed will be cleaned up when it goes out of scope
} // No snappy-specific cleanup needed
```
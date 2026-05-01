# Best Practices

```cpp
// Best Practice 1: Create a compression wrapper class
class SnappyCompressor {
public:
    static std::string compress(const std::string& input) {
        if (input.empty()) return input;
        std::string output;
        snappy::Compress(input.data(), input.size(), &output);
        return output.size() < input.size() ? output : input;
    }
    
    static std::string decompress(const std::string& input) {
        if (input.empty()) return input;
        if (!snappy::IsValidCompressedBuffer(input.data(), input.size())) {
            throw std::invalid_argument("Invalid compressed data");
        }
        std::string output;
        if (!snappy::Uncompress(input.data(), input.size(), &output)) {
            throw std::runtime_error("Decompression failed");
        }
        return output;
    }
};
```

```cpp
// Best Practice 2: Use streaming compression for large data
class StreamingSnappy {
    std::string buffer;
public:
    void feed(const char* data, size_t size) {
        buffer.append(data, size);
    }
    
    std::string flush() {
        std::string compressed;
        snappy::Compress(buffer.data(), buffer.size(), &compressed);
        buffer.clear();
        return compressed;
    }
};
```

```cpp
// Best Practice 3: Always validate before decompression in production
std::string safe_decompress(const std::string& compressed) {
    if (!snappy::IsValidCompressedBuffer(compressed.data(), compressed.size())) {
        throw std::runtime_error("Invalid snappy data");
    }
    
    size_t uncompressed_length;
    if (!snappy::GetUncompressedLength(compressed.data(), compressed.size(), &uncompressed_length)) {
        throw std::runtime_error("Failed to get uncompressed length");
    }
    
    // Prevent memory exhaustion from malicious input
    if (uncompressed_length > MAX_UNCOMPRESSED_SIZE) {
        throw std::runtime_error("Uncompressed data too large");
    }
    
    std::string uncompressed;
    if (!snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed)) {
        throw std::runtime_error("Decompression failed");
    }
    return uncompressed;
}
```
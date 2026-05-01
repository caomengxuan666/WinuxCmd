# Performance

```cpp
// Performance tip 1: Pre-allocate output buffer for compression
std::string input = get_large_data();
std::string compressed;
compressed.resize(snappy::MaxCompressedLength(input.size())); // Pre-allocate
size_t compressed_size;
snappy::RawCompress(input.data(), input.size(), &compressed[0], &compressed_size);
compressed.resize(compressed_size); // Trim to actual size
```

```cpp
// Performance tip 2: Reuse buffers to avoid reallocation
class ReusableCompressor {
    std::string compressed;
    std::string uncompressed;
public:
    std::string_view compress(const std::string_view input) {
        compressed.clear();
        snappy::Compress(input.data(), input.size(), &compressed);
        return compressed;
    }
    
    std::string_view decompress(const std::string_view input) {
        uncompressed.clear();
        snappy::Uncompress(input.data(), input.size(), &uncompressed);
        return uncompressed;
    }
};
```

```cpp
// Performance tip 3: Batch small inputs for better compression ratio
std::string batch_compress(const std::vector<std::string>& inputs) {
    std::string combined;
    for (const auto& input : inputs) {
        combined += input;
    }
    std::string compressed;
    snappy::Compress(combined.data(), combined.size(), &compressed);
    return compressed;
}
```

```cpp
// Performance characteristics:
// - Compression speed: ~250-500 MB/s (single-threaded)
// - Decompression speed: ~500-1000 MB/s (single-threaded)
// - Compression ratio: 1.5x-3x for text, 1x-1.5x for binary
// - Memory overhead: ~32KB internal buffer during compression
// - Best for: data > 1KB, real-time applications
```
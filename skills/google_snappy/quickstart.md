# Quickstart

```cpp
#include <snappy.h>
#include <string>
#include <cassert>
#include <vector>

// Pattern 1: Compress a string
void compress_string() {
    std::string input = "Hello, World! This is a test string for snappy compression.";
    std::string compressed;
    snappy::Compress(input.data(), input.size(), &compressed);
    // compressed now holds the compressed data
}

// Pattern 2: Uncompress a string
void uncompress_string() {
    std::string compressed = /* previously compressed data */;
    std::string uncompressed;
    bool success = snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed);
    assert(success);
    // uncompressed now holds the original data
}

// Pattern 3: Check if data is snappy compressed
void check_compressed() {
    std::string data = /* some data */;
    bool is_compressed = snappy::IsValidCompressedBuffer(data.data(), data.size());
    if (is_compressed) {
        // Safe to uncompress
    }
}

// Pattern 4: Get uncompressed length without decompressing
void get_uncompressed_length() {
    std::string compressed = /* compressed data */;
    size_t uncompressed_length;
    bool success = snappy::GetUncompressedLength(compressed.data(), compressed.size(), &uncompressed_length);
    assert(success);
    // uncompressed_length now holds the size of the original data
}

// Pattern 5: Compress with raw output (no string)
void compress_raw() {
    const char* input = "Some data to compress";
    size_t input_length = strlen(input);
    size_t max_compressed_length = snappy::MaxCompressedLength(input_length);
    std::vector<char> compressed(max_compressed_length);
    size_t compressed_length;
    snappy::RawCompress(input, input_length, compressed.data(), &compressed_length);
    compressed.resize(compressed_length);
}

// Pattern 6: Uncompress to raw buffer
void uncompress_raw() {
    std::vector<char> compressed = /* compressed data */;
    size_t uncompressed_length;
    snappy::GetUncompressedLength(compressed.data(), compressed.size(), &uncompressed_length);
    std::vector<char> uncompressed(uncompressed_length);
    bool success = snappy::RawUncompress(compressed.data(), compressed.size(), uncompressed.data());
    assert(success);
}
```
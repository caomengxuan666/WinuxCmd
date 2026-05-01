# Pitfalls

```cpp
// BAD: Not checking return value of Uncompress
std::string compressed = get_compressed_data();
std::string uncompressed;
snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed);
// If compressed data is corrupted, this silently fails

// GOOD: Always check return value
std::string compressed = get_compressed_data();
std::string uncompressed;
if (!snappy::Uncompress(compressed.data(), compressed.size(), &uncompressed)) {
    throw std::runtime_error("Failed to decompress data");
}
```

```cpp
// BAD: Using wrong buffer size for decompression
std::string compressed = get_compressed_data();
std::vector<char> output(1024); // Arbitrary size
snappy::RawUncompress(compressed.data(), compressed.size(), output.data());
// Buffer overflow if uncompressed data > 1024 bytes

// GOOD: Get exact uncompressed length first
std::string compressed = get_compressed_data();
size_t uncompressed_length;
snappy::GetUncompressedLength(compressed.data(), compressed.size(), &uncompressed_length);
std::vector<char> output(uncompressed_length);
snappy::RawUncompress(compressed.data(), compressed.size(), output.data());
```

```cpp
// BAD: Assuming compressed data is always smaller
std::string input = "a";
std::string compressed;
snappy::Compress(input.data(), input.size(), &compressed);
// compressed may be larger than input for small data

// GOOD: Check if compression was beneficial
std::string input = get_data();
std::string compressed;
snappy::Compress(input.data(), input.size(), &compressed);
if (compressed.size() >= input.size()) {
    // Store original instead
}
```

```cpp
// BAD: Passing null pointers to compression functions
snappy::Compress(nullptr, 0, &output); // Undefined behavior

// GOOD: Always pass valid pointers
std::string input = "test";
std::string output;
snappy::Compress(input.data(), input.size(), &output);
```
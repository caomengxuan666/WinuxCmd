# Safety

```cpp
// RED LINE 1: NEVER pass corrupted data to Uncompress without validation
// BAD:
std::string corrupted_data = get_untrusted_data();
std::string result;
snappy::Uncompress(corrupted_data.data(), corrupted_data.size(), &result); // Crash risk

// GOOD:
std::string data = get_untrusted_data();
if (!snappy::IsValidCompressedBuffer(data.data(), data.size())) {
    throw std::runtime_error("Invalid compressed data");
}
std::string result;
snappy::Uncompress(data.data(), data.size(), &result);
```

```cpp
// RED LINE 2: NEVER assume compressed output fits in a fixed-size buffer
// BAD:
char buffer[1024];
snappy::RawCompress(input.data(), input.size(), buffer, &compressed_length); // Overflow

// GOOD:
size_t max_size = snappy::MaxCompressedLength(input.size());
std::vector<char> buffer(max_size);
snappy::RawCompress(input.data(), input.size(), buffer.data(), &compressed_length);
```

```cpp
// RED LINE 3: NEVER use Uncompress on data that wasn't compressed by Snappy
// BAD:
std::string random_data = get_random_bytes();
std::string result;
snappy::Uncompress(random_data.data(), random_data.size(), &result); // Undefined behavior

// GOOD:
if (snappy::IsValidCompressedBuffer(random_data.data(), random_data.size())) {
    snappy::Uncompress(random_data.data(), random_data.size(), &result);
}
```

```cpp
// RED LINE 4: NEVER ignore the return value of GetUncompressedLength
// BAD:
size_t length;
snappy::GetUncompressedLength(data.data(), data.size(), &length); // May fail silently

// GOOD:
size_t length;
if (!snappy::GetUncompressedLength(data.data(), data.size(), &length)) {
    throw std::runtime_error("Invalid compressed data");
}
```

```cpp
// RED LINE 5: NEVER compress data with zero length without proper handling
// BAD:
std::string empty;
std::string compressed;
snappy::Compress(empty.data(), 0, &compressed); // May produce unexpected results

// GOOD:
if (input.empty()) {
    return input; // No compression needed
}
std::string compressed;
snappy::Compress(input.data(), input.size(), &compressed);
```
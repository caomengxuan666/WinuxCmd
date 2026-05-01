# Threading

```cpp
// Thread safety: Snappy functions are thread-safe for independent operations
// No shared state between calls

// Thread-safe usage example
#include <thread>
#include <vector>

void compress_chunk(const std::string& input, std::string& output) {
    snappy::Compress(input.data(), input.size(), &output);
}

int main() {
    std::vector<std::string> chunks = {"data1", "data2", "data3"};
    std::vector<std::string> results(chunks.size());
    std::vector<std::thread> threads;
    
    for (size_t i = 0; i < chunks.size(); ++i) {
        threads.emplace_back(compress_chunk, std::ref(chunks[i]), std::ref(results[i]));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
```

```cpp
// NOT thread-safe: Sharing output buffer between threads
std::string shared_output;
void unsafe_compress(const std::string& input) {
    snappy::Compress(input.data(), input.size(), &shared_output); // Race condition!
}

// Thread-safe: Each thread has its own output buffer
void safe_compress(const std::string& input, std::string& output) {
    snappy::Compress(input.data(), input.size(), &output); // Safe
}
```

```cpp
// Thread-safe wrapper for concurrent access
#include <mutex>

class ThreadSafeCompressor {
    std::mutex mtx;
public:
    std::string compress(const std::string& input) {
        std::lock_guard<std::mutex> lock(mtx);
        std::string output;
        snappy::Compress(input.data(), input.size(), &output);
        return output;
    }
    
    std::string decompress(const std::string& input) {
        std::lock_guard<std::mutex> lock(mtx);
        std::string output;
        snappy::Uncompress(input.data(), input.size(), &output);
        return output;
    }
};
```
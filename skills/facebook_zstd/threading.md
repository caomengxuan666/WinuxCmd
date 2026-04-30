# Threading

### Thread safety guarantees
```cpp
// ZSTD contexts are NOT thread-safe
// Each context must be used by only one thread at a time

// GOOD: Thread-local contexts
void process_data(const std::vector<uint8_t>& data) {
    thread_local ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) cctx = ZSTD_createCCtx();
    
    ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);
    // Safe to use cctx here
}
```

### Multi-threaded compression (internal parallelism)
```cpp
// ZSTD supports internal multi-threading for compression
ZSTD_CCtx* cctx = ZSTD_createCCtx();
ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 4); // Internal parallelism
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);

// This is safe to call from a single thread
ZSTD_compress2(cctx, dst, dstSize, src, srcSize);
// Internal threads are managed by ZSTD
```

### Thread-safe context pool
```cpp
class ThreadSafeContextPool {
    std::mutex mtx;
    std::vector<ZSTD_CCtx*> available;
    
public:
    ZSTD_CCtx* acquire() {
        std::lock_guard<std::mutex> lock(mtx);
        if (available.empty()) {
            return ZSTD_createCCtx();
        }
        auto ctx = available.back();
        available.pop_back();
        return ctx;
    }
    
    void release(ZSTD_CCtx* ctx) {
        ZSTD_CCtx_reset(ctx, ZSTD_reset_session_and_parameters);
        std::lock_guard<std::mutex> lock(mtx);
        available.push_back(ctx);
    }
};

// Usage in multi-threaded environment
ThreadSafeContextPool pool;
std::vector<std::future<std::vector<uint8_t>>> futures;

for (const auto& chunk : chunks) {
    futures.push_back(std::async(std::launch::async, [&pool, &chunk]() {
        ZSTD_CCtx* cctx = pool.acquire();
        std::vector<uint8_t> result;
        // Compress with cctx...
        pool.release(cctx);
        return result;
    }));
}
```

### Thread-safe decompression
```cpp
// Decompression contexts are also NOT thread-safe
// Each thread needs its own context

void parallel_decompress(const std::vector<std::vector<uint8_t>>& compressed_chunks,
                         std::vector<std::vector<uint8_t>>& results) {
    std::vector<std::thread> threads;
    
    for (size_t i = 0; i < compressed_chunks.size(); ++i) {
        threads.emplace_back([&, i]() {
            ZSTD_DCtx* dctx = ZSTD_createDCtx();
            if (!dctx) return;
            
            // Safe to use dctx in this thread
            unsigned long long size = ZSTD_getFrameContentSize(
                compressed_chunks[i].data(), compressed_chunks[i].size());
            results[i].resize(size);
            
            ZSTD_decompressDCtx(dctx, results[i].data(), results[i].size(),
                               compressed_chunks[i].data(), compressed_chunks[i].size());
            
            ZSTD_freeDCtx(dctx);
        });
    }
    
    for (auto& t : threads) t.join();
}
```

### Atomic operations for shared state
```cpp
// Shared state between threads needs synchronization
struct SharedCompressionStats {
    std::atomic<size_t> total_compressed{0};
    std::atomic<size_t> total_original{0};
    std::mutex error_mutex;
    std::vector<std::string> errors;
    
    void record(size_t original, size_t compressed) {
        total_original += original;
        total_compressed += compressed;
    }
    
    void add_error(const std::string& err) {
        std::lock_guard<std::mutex> lock(error_mutex);
        errors.push_back(err);
    }
};

// Each thread updates shared stats safely
void worker_thread(const std::vector<uint8_t>& data, 
                   SharedCompressionStats& stats) {
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    // ... compress ...
    stats.record(data.size(), compressed_size);
    ZSTD_freeCCtx(cctx);
}
```
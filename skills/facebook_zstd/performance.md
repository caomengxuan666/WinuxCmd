# Performance

### Compression level tradeoffs
```cpp
// Level -5 (fastest): ~545 MB/s compression, ~1850 MB/s decompression
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, -5);

// Level 3 (default): ~510 MB/s compression, ~1550 MB/s decompression
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);

// Level 19 (high): ~20 MB/s compression, ~1500 MB/s decompression
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 19);

// Level 22 (max): ~5 MB/s compression, ~1500 MB/s decompression
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 22);
```

### Buffer size optimization
```cpp
// GOOD: Use recommended buffer sizes
const size_t COMPRESS_BUFFER = ZSTD_CStreamOutSize();  // ~128KB
const size_t DECOMPRESS_BUFFER = ZSTD_DStreamOutSize(); // ~128KB

// BAD: Too small buffers cause many iterations
const size_t TINY_BUFFER = 1024; // Will cause many calls

// GOOD: Larger buffers reduce overhead
const size_t LARGE_BUFFER = 1024 * 1024; // 1MB for high-throughput
```

### Multi-threaded compression
```cpp
// Enable multi-threading for large data
ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 4); // 4 threads
ZSTD_CCtx_setParameter(cctx, ZSTD_c_jobSize, 1 << 20); // 1MB per job

// For best performance, match workers to CPU cores
int num_workers = std::thread::hardware_concurrency();
ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, num_workers);
```

### Memory usage considerations
```cpp
// Control memory usage with window log
ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 20); // ~1MB window
ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 23); // ~8MB window
ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 27); // ~128MB window

// For memory-constrained environments
ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 18); // ~256KB window
ZSTD_CCtx_setParameter(cctx, ZSTD_c_hashLog, 17);   // Smaller hash table
ZSTD_CCtx_setParameter(cctx, ZSTD_c_chainLog, 16);  // Smaller chain table
```

### Pre-allocating buffers
```cpp
// GOOD: Pre-allocate buffers to avoid reallocation
class FastCompressor {
    std::vector<uint8_t> buffer;
    ZSTD_CCtx* cctx;
public:
    FastCompressor(size_t max_input_size) 
        : buffer(ZSTD_compressBound(max_input_size))
        , cctx(ZSTD_createCCtx()) {}
    
    std::span<uint8_t> compress(std::span<const uint8_t> input) {
        size_t result = ZSTD_compress2(cctx, buffer.data(), buffer.size(),
                                       input.data(), input.size());
        if (ZSTD_isError(result)) throw std::runtime_error(ZSTD_getErrorName(result));
        return {buffer.data(), result};
    }
};
```

### Dictionary performance boost
```cpp
// Training dictionary improves small data compression by 10-50%
std::vector<uint8_t> dict = train_dictionary(samples);

// Load dictionary once, reuse many times
ZSTD_CCtx_loadDictionary(cctx, dict.data(), dict.size());
// Subsequent compressions benefit from dictionary
```
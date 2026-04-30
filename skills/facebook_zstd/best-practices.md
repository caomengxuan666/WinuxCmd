# Best Practices

### Use RAII wrappers for context management
```cpp
class ZstdCompressor {
    ZSTD_CCtx* cctx;
public:
    ZstdCompressor(int level = 3) : cctx(ZSTD_createCCtx()) {
        if (!cctx) throw std::bad_alloc();
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);
    }
    ~ZstdCompressor() { if (cctx) ZSTD_freeCCtx(cctx); }
    ZstdCompressor(const ZstdCompressor&) = delete;
    ZstdCompressor& operator=(const ZstdCompressor&) = delete;
    
    std::vector<uint8_t> compress(const std::vector<uint8_t>& input) {
        size_t bound = ZSTD_compressBound(input.size());
        std::vector<uint8_t> output(bound);
        size_t result = ZSTD_compress2(cctx, output.data(), output.size(),
                                       input.data(), input.size());
        if (ZSTD_isError(result)) throw std::runtime_error(ZSTD_getErrorName(result));
        output.resize(result);
        return output;
    }
};
```

### Prefer ZSTD_compress2 over deprecated functions
```cpp
// BAD: Using deprecated API
ZSTD_CCtx* cctx = ZSTD_createCCtx();
ZSTD_compressBegin(cctx, 3);
ZSTD_compressContinue(cctx, dst, dstSize, src, srcSize);
ZSTD_compressEnd(cctx, dst, dstSize, nullptr, 0);

// GOOD: Using modern API
ZSTD_CCtx* cctx = ZSTD_createCCtx();
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);
ZSTD_compress2(cctx, dst, dstSize, src, srcSize);
```

### Use appropriate buffer sizes for streaming
```cpp
// GOOD: Using recommended buffer sizes
const size_t BUFFER_SIZE = ZSTD_CStreamOutSize(); // ~128KB
std::vector<uint8_t> buffer(BUFFER_SIZE);

// For decompression
const size_t DECOMP_BUFFER = ZSTD_DStreamOutSize(); // ~128KB
```

### Implement proper error handling strategy
```cpp
class ZstdException : public std::runtime_error {
public:
    explicit ZstdException(const std::string& msg) : std::runtime_error(msg) {}
};

void safe_compress(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx) throw ZstdException("Failed to create compression context");
    
    auto cleanup = std::unique_ptr<ZSTD_CCtx, decltype(&ZSTD_freeCCtx)>(cctx, ZSTD_freeCCtx);
    
    size_t bound = ZSTD_compressBound(input.size());
    output.resize(bound);
    
    size_t result = ZSTD_compress2(cctx, output.data(), output.size(),
                                   input.data(), input.size());
    if (ZSTD_isError(result)) {
        throw ZstdException(ZSTD_getErrorName(result));
    }
    output.resize(result);
}
```

### Cache contexts for repeated use
```cpp
class ContextPool {
    std::vector<ZSTD_CCtx*> pool;
    std::mutex mtx;
public:
    ZSTD_CCtx* acquire() {
        std::lock_guard<std::mutex> lock(mtx);
        if (pool.empty()) {
            return ZSTD_createCCtx();
        }
        auto ctx = pool.back();
        pool.pop_back();
        return ctx;
    }
    
    void release(ZSTD_CCtx* ctx) {
        ZSTD_CCtx_reset(ctx, ZSTD_reset_session_and_parameters);
        std::lock_guard<std::mutex> lock(mtx);
        pool.push_back(ctx);
    }
};
```
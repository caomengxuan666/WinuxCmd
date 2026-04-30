# Lifecycle

### Construction and initialization
```cpp
// Creating contexts
ZSTD_CCtx* cctx = ZSTD_createCCtx();  // Compression context
ZSTD_DCtx* dctx = ZSTD_createDCtx();  // Decompression context
ZSTD_CStream* zcs = ZSTD_createCStream();  // Streaming compression
ZSTD_DStream* zds = ZSTD_createDStream();  // Streaming decompression

// Always check for NULL
if (!cctx) {
    // Handle allocation failure
}
```

### Destruction
```cpp
// Proper cleanup
ZSTD_freeCCtx(cctx);   // Free compression context
ZSTD_freeDCtx(dctx);   // Free decompression context
ZSTD_freeCStream(zcs); // Free streaming compression
ZSTD_freeDStream(zds); // Free streaming decompression

// Setting pointers to NULL after free is good practice
cctx = nullptr;
```

### Reset and reuse
```cpp
// Reset context for reuse
ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);     // Reset session, keep parameters
ZSTD_CCtx_reset(cctx, ZSTD_reset_parameters);        // Reset parameters to defaults
ZSTD_CCtx_reset(cctx, ZSTD_reset_session_and_parameters); // Reset everything

// Example of context reuse
void compress_multiple_buffers(ZSTD_CCtx* cctx, const std::vector<std::vector<uint8_t>>& buffers) {
    for (const auto& buf : buffers) {
        ZSTD_CCtx_reset(cctx, ZSTD_reset_session_only);
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);
        
        size_t bound = ZSTD_compressBound(buf.size());
        std::vector<uint8_t> output(bound);
        ZSTD_compress2(cctx, output.data(), output.size(), buf.data(), buf.size());
    }
}
```

### Move semantics (C++ wrapper)
```cpp
class ZstdContext {
    ZSTD_CCtx* ctx;
public:
    ZstdContext() : ctx(ZSTD_createCCtx()) {}
    ~ZstdContext() { if (ctx) ZSTD_freeCCtx(ctx); }
    
    // Move constructor
    ZstdContext(ZstdContext&& other) noexcept : ctx(other.ctx) {
        other.ctx = nullptr;
    }
    
    // Move assignment
    ZstdContext& operator=(ZstdContext&& other) noexcept {
        if (this != &other) {
            if (ctx) ZSTD_freeCCtx(ctx);
            ctx = other.ctx;
            other.ctx = nullptr;
        }
        return *this;
    }
    
    // No copy
    ZstdContext(const ZstdContext&) = delete;
    ZstdContext& operator=(const ZstdContext&) = delete;
};
```

### Resource management with unique_ptr
```cpp
// Custom deleter for unique_ptr
struct ZstdCCtxDeleter {
    void operator()(ZSTD_CCtx* ctx) { ZSTD_freeCCtx(ctx); }
};

struct ZstdDCtxDeleter {
    void operator()(ZSTD_DCtx* ctx) { ZSTD_freeDCtx(ctx); }
};

using UniqueZstdCCtx = std::unique_ptr<ZSTD_CCtx, ZstdCCtxDeleter>;
using UniqueZstdDCtx = std::unique_ptr<ZSTD_DCtx, ZstdDCtxDeleter>;

// Usage
UniqueZstdCCtx cctx(ZSTD_createCCtx());
if (!cctx) throw std::bad_alloc();
// Automatically freed when cctx goes out of scope
```
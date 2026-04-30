# Quickstart

```cpp
// Example 1: Simple one-shot compression/decompression
#include <zstd.h>
#include <vector>
#include <cstdint>

std::vector<uint8_t> compress_data(const std::vector<uint8_t>& input) {
    size_t bound = ZSTD_compressBound(input.size());
    std::vector<uint8_t> compressed(bound);
    size_t result = ZSTD_compress(compressed.data(), compressed.size(),
                                  input.data(), input.size(), 3);
    if (ZSTD_isError(result)) throw std::runtime_error(ZSTD_getErrorName(result));
    compressed.resize(result);
    return compressed;
}

std::vector<uint8_t> decompress_data(const std::vector<uint8_t>& compressed) {
    unsigned long long size = ZSTD_getFrameContentSize(compressed.data(), compressed.size());
    if (size == ZSTD_CONTENTSIZE_ERROR) throw std::runtime_error("Invalid frame");
    std::vector<uint8_t> decompressed(size);
    size_t result = ZSTD_decompress(decompressed.data(), decompressed.size(),
                                    compressed.data(), compressed.size());
    if (ZSTD_isError(result)) throw std::runtime_error(ZSTD_getErrorName(result));
    return decompressed;
}

// Example 2: Streaming compression
#include <zstd.h>

struct StreamCompressor {
    ZSTD_CCtx* cctx;
    StreamCompressor(int level) : cctx(ZSTD_createCCtx()) {
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);
    }
    ~StreamCompressor() { ZSTD_freeCCtx(cctx); }
    
    std::vector<uint8_t> compress(const std::vector<uint8_t>& input) {
        ZSTD_inBuffer in = {input.data(), input.size(), 0};
        std::vector<uint8_t> output(ZSTD_CStreamOutSize());
        ZSTD_outBuffer out = {output.data(), output.size(), 0};
        
        ZSTD_compressStream2(cctx, &out, &in, ZSTD_e_continue);
        output.resize(out.pos);
        return output;
    }
    
    std::vector<uint8_t> flush() {
        ZSTD_inBuffer in = {nullptr, 0, 0};
        std::vector<uint8_t> output(ZSTD_CStreamOutSize());
        ZSTD_outBuffer out = {output.data(), output.size(), 0};
        ZSTD_compressStream2(cctx, &out, &in, ZSTD_e_end);
        output.resize(out.pos);
        return output;
    }
};

// Example 3: Using compression levels
void compression_levels_example() {
    std::vector<uint8_t> data(1024 * 1024, 0x42); // 1MB test data
    
    // Fast mode (negative levels)
    size_t fast_bound = ZSTD_compressBound(data.size());
    std::vector<uint8_t> fast_result(fast_bound);
    size_t fast_size = ZSTD_compress(fast_result.data(), fast_result.size(),
                                     data.data(), data.size(), -5);
                                     
    // Default level 3
    std::vector<uint8_t> default_result(fast_bound);
    size_t default_size = ZSTD_compress(default_result.data(), default_result.size(),
                                        data.data(), data.size(), 3);
                                        
    // Max compression level 22
    std::vector<uint8_t> max_result(fast_bound);
    size_t max_size = ZSTD_compress(max_result.data(), max_result.size(),
                                    data.data(), data.size(), 22);
}

// Example 4: Dictionary compression
#include <zstd.h>

std::vector<uint8_t> train_dictionary(const std::vector<std::vector<uint8_t>>& samples) {
    std::vector<size_t> sizes;
    std::vector<const void*> buffers;
    for (const auto& s : samples) {
        buffers.push_back(s.data());
        sizes.push_back(s.size());
    }
    
    std::vector<uint8_t> dict(ZSTD_dictionarySizeLimit());
    size_t dict_size = ZSTD_trainFromBuffer(dict.data(), dict.size(),
                                            buffers.data(), sizes.data(), samples.size());
    dict.resize(dict_size);
    return dict;
}

// Example 5: Content size detection
void content_size_example(const std::vector<uint8_t>& compressed) {
    unsigned long long size = ZSTD_getFrameContentSize(compressed.data(), compressed.size());
    if (size == ZSTD_CONTENTSIZE_UNKNOWN) {
        // Streaming decompression needed
    } else if (size == ZSTD_CONTENTSIZE_ERROR) {
        // Invalid data
    } else {
        // Known size, can allocate exact buffer
        std::vector<uint8_t> output(size);
    }
}

// Example 6: Multi-threaded compression
void multithreaded_compress(const std::vector<uint8_t>& input) {
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 4); // 4 threads
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);
    
    size_t bound = ZSTD_compressBound(input.size());
    std::vector<uint8_t> output(bound);
    size_t result = ZSTD_compress2(cctx, output.data(), output.size(),
                                   input.data(), input.size());
    ZSTD_freeCCtx(cctx);
}
```
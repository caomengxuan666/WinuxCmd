# Pitfalls

### Pitfall 1: Not checking for errors
```cpp
// BAD: Ignoring error returns
size_t result = ZSTD_compress(dst, dstSize, src, srcSize, 3);
// result might be an error code, not compressed size

// GOOD: Always check for errors
size_t result = ZSTD_compress(dst, dstSize, src, srcSize, 3);
if (ZSTD_isError(result)) {
    fprintf(stderr, "Compression failed: %s\n", ZSTD_getErrorName(result));
    return -1;
}
```

### Pitfall 2: Using wrong buffer sizes
```cpp
// BAD: Assuming exact output size
char output[1024]; // Might be too small

// GOOD: Use ZSTD_compressBound to get safe size
size_t bound = ZSTD_compressBound(inputSize);
char* output = malloc(bound);
```

### Pitfall 3: Mixing streaming and one-shot APIs
```cpp
// BAD: Using streaming context with one-shot functions
ZSTD_CStream* zcs = ZSTD_createCStream();
ZSTD_compress(zcs, dst, dstSize, src, srcSize, 3); // Wrong!

// GOOD: Use appropriate API
ZSTD_CCtx* cctx = ZSTD_createCCtx();
ZSTD_compress2(cctx, dst, dstSize, src, srcSize);
```

### Pitfall 4: Ignoring ZSTD_CONTENTSIZE_UNKNOWN
```cpp
// BAD: Assuming content size is always known
unsigned long long size = ZSTD_getFrameContentSize(data, size);
char* output = malloc(size); // Crash if size == ZSTD_CONTENTSIZE_UNKNOWN

// GOOD: Handle unknown sizes
unsigned long long size = ZSTD_getFrameContentSize(data, size);
if (size == ZSTD_CONTENTSIZE_UNKNOWN) {
    // Use streaming decompression
} else if (size == ZSTD_CONTENTSIZE_ERROR) {
    // Handle error
} else {
    char* output = malloc(size);
}
```

### Pitfall 5: Not freeing resources
```cpp
// BAD: Memory leak
ZSTD_CCtx* cctx = ZSTD_createCCtx();
// Use cctx...
// Never freed!

// GOOD: Always free
ZSTD_CCtx* cctx = ZSTD_createCCtx();
// Use cctx...
ZSTD_freeCCtx(cctx);
```

### Pitfall 6: Incorrect dictionary usage
```cpp
// BAD: Using different dictionaries for compress/decompress
ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 3);
ZSTD_CCtx_loadDictionary(cctx, dict1, dict1Size);
// Compress...
ZSTD_DCtx_loadDictionary(dctx, dict2, dict2Size); // Wrong dictionary!

// GOOD: Use same dictionary
ZSTD_CCtx_loadDictionary(cctx, dict, dictSize);
// Compress...
ZSTD_DCtx_loadDictionary(dctx, dict, dictSize); // Same dictionary
```

### Pitfall 7: Thread safety violations
```cpp
// BAD: Sharing context between threads
ZSTD_CCtx* cctx = ZSTD_createCCtx();
// Thread 1: ZSTD_compress2(cctx, ...)
// Thread 2: ZSTD_compress2(cctx, ...) // Race condition!

// GOOD: Each thread gets its own context
void thread_func(void* data) {
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    ZSTD_compress2(cctx, ...);
    ZSTD_freeCCtx(cctx);
}
```

### Pitfall 8: Not handling incomplete frames
```cpp
// BAD: Assuming single call decompresses everything
ZSTD_outBuffer out = {output, outputSize, 0};
ZSTD_inBuffer in = {input, inputSize, 0};
ZSTD_decompressStream(dctx, &out, &in);
// Might not have consumed all input!

// GOOD: Loop until done
while (in.pos < in.size) {
    size_t ret = ZSTD_decompressStream(dctx, &out, &in);
    if (ZSTD_isError(ret)) break;
    if (ret == 0) break; // Frame done
}
```
# Safety

### Red Line 1: NEVER use uninitialized contexts
```cpp
// BAD: Using context before initialization
ZSTD_CCtx* cctx; // Uninitialized!
ZSTD_compress2(cctx, dst, dstSize, src, srcSize); // Undefined behavior

// GOOD: Always create and initialize properly
ZSTD_CCtx* cctx = ZSTD_createCCtx();
if (!cctx) { /* handle error */ }
ZSTD_compress2(cctx, dst, dstSize, src, srcSize);
```

### Red Line 2: NEVER ignore ZSTD_isError returns
```cpp
// BAD: Ignoring error codes
size_t result = ZSTD_compress(dst, dstSize, src, srcSize, 3);
// result might be error, not compressed size

// GOOD: Always validate
size_t result = ZSTD_compress(dst, dstSize, src, srcSize, 3);
if (ZSTD_isError(result)) {
    // Handle error, never use result as size
    abort();
}
```

### Red Line 3: NEVER use the same context concurrently
```cpp
// BAD: Concurrent access to same context
ZSTD_CCtx* cctx = ZSTD_createCCtx();
#pragma omp parallel for
for (int i = 0; i < 100; i++) {
    ZSTD_compress2(cctx, ...); // Data race!
}

// GOOD: Each thread gets its own context
#pragma omp parallel for
for (int i = 0; i < 100; i++) {
    ZSTD_CCtx* local_cctx = ZSTD_createCCtx();
    ZSTD_compress2(local_cctx, ...);
    ZSTD_freeCCtx(local_cctx);
}
```

### Red Line 4: NEVER pass NULL pointers to compression functions
```cpp
// BAD: NULL source or destination
ZSTD_compress(NULL, 0, src, srcSize, 3); // Crash
ZSTD_compress(dst, dstSize, NULL, 0, 3); // Crash

// GOOD: Always validate pointers
if (!dst || !src) return -1;
size_t result = ZSTD_compress(dst, dstSize, src, srcSize, 3);
```

### Red Line 5: NEVER use ZSTD_getFrameContentSize on untrusted data without validation
```cpp
// BAD: Using content size from untrusted source
unsigned long long size = ZSTD_getFrameContentSize(data, dataSize);
char* buffer = malloc(size); // Could be huge or ZSTD_CONTENTSIZE_ERROR

// GOOD: Validate before use
unsigned long long size = ZSTD_getFrameContentSize(data, dataSize);
if (size == ZSTD_CONTENTSIZE_ERROR || size == ZSTD_CONTENTSIZE_UNKNOWN) {
    // Handle error or use streaming
    return -1;
}
if (size > MAX_ALLOWED_SIZE) {
    // Reject oversized content
    return -1;
}
char* buffer = malloc(size);
```
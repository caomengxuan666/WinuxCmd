# Performance

**Event loop performance characteristics**
```cpp
// UV_RUN_NOWAIT for non-blocking iteration
while (!done) {
    uv_run(loop, UV_RUN_NOWAIT); // Returns immediately if no events
    // Do other work
}

// UV_RUN_ONCE for blocking with timeout
uv_run(loop, UV_RUN_ONCE); // Blocks until at least one callback fires
```

**Buffer allocation patterns**
```cpp
// BAD: Allocating in read callback
void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
    buf->base = (char*)malloc(size); // Frequent allocations
    buf->len = size;
}

// GOOD: Use a buffer pool
typedef struct {
    char* buffer;
    size_t size;
    int in_use;
} buffer_t;

#define POOL_SIZE 10
buffer_t buffer_pool[POOL_SIZE];

void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!buffer_pool[i].in_use && buffer_pool[i].size >= size) {
            buffer_pool[i].in_use = 1;
            buf->base = buffer_pool[i].buffer;
            buf->len = buffer_pool[i].size;
            return;
        }
    }
    buf->base = (char*)malloc(size);
    buf->len = size;
}
```

**Optimizing file I/O**
```cpp
// Use UV_FS_O_DIRECT for large sequential reads
uv_fs_t req;
int flags = UV_FS_O_RDONLY | UV_FS_O_DIRECT;
uv_fs_open(loop, &req, "largefile.bin", flags, 0644, NULL);

// Batch file operations
void batch_file_ops() {
    uv_fs_t reqs[10];
    for (int i = 0; i < 10; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "file_%d.txt", i);
        uv_fs_open(loop, &reqs[i], filename, O_RDONLY, 0, NULL);
    }
}
```

**Thread pool tuning**
```cpp
// Default thread pool size is 4
// Adjust for I/O bound vs CPU bound workloads
#include <uv.h>

void set_thread_pool_size() {
    // For I/O bound: more threads
    uv_os_setenv("UV_THREADPOOL_SIZE", "8");
    
    // For CPU bound: match core count
    uv_cpu_info_t* info;
    int count;
    uv_cpu_info(&info, &count);
    char size[4];
    snprintf(size, sizeof(size), "%d", count);
    uv_os_setenv("UV_THREADPOOL_SIZE", size);
    uv_free_cpu_info(info, count);
}
```
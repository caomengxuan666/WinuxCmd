# Pitfalls

**Pitfall 1: Forgetting to close handles**
```cpp
// BAD: Handle leak
uv_tcp_t server;
uv_tcp_init(uv_default_loop(), &server);
// ... use server
// Never closed!

// GOOD: Always close handles
uv_tcp_t server;
uv_tcp_init(uv_default_loop(), &server);
// ... use server
uv_close((uv_handle_t*)&server, NULL);
```

**Pitfall 2: Using stack-allocated handles after scope exit**
```cpp
// BAD: Handle goes out of scope
void setup_timer() {
    uv_timer_t timer;
    uv_timer_init(uv_default_loop(), &timer);
    uv_timer_start(&timer, timer_cb, 1000, 0);
} // timer destroyed, but loop still references it!

// GOOD: Heap-allocate or ensure lifetime
void setup_timer() {
    uv_timer_t* timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
    uv_timer_init(uv_default_loop(), timer);
    uv_timer_start(timer, timer_cb, 1000, 0);
}
```

**Pitfall 3: Not checking return values**
```cpp
// BAD: Ignoring errors
uv_listen((uv_stream_t*)&server, 128, on_new_connection);

// GOOD: Always check return values
int r = uv_listen((uv_stream_t*)&server, 128, on_new_connection);
if (r) {
    fprintf(stderr, "Listen error: %s\n", uv_strerror(r));
    uv_close((uv_handle_t*)&server, NULL);
}
```

**Pitfall 4: Calling uv_stop from outside the event loop**
```cpp
// BAD: uv_stop in callback from thread pool
void work_cb(uv_work_t* req) {
    uv_stop(uv_default_loop()); // Undefined behavior!
}

// GOOD: Use async handle to signal stop
uv_async_t async;
void async_cb(uv_async_t* handle) {
    uv_stop(uv_default_loop());
}
void work_cb(uv_work_t* req) {
    uv_async_send(&async); // Safe way to signal
}
```

**Pitfall 5: Modifying shared data without synchronization**
```cpp
// BAD: Race condition on shared counter
int counter = 0;
void timer_cb(uv_timer_t* handle) {
    counter++; // Not thread-safe if multiple timers
}

// GOOD: Use atomic operations or mutex
#include <stdatomic.h>
atomic_int counter = 0;
void timer_cb(uv_timer_t* handle) {
    atomic_fetch_add(&counter, 1);
}
```

**Pitfall 6: Using uv_default_loop() in library code**
```cpp
// BAD: Library pollutes user's default loop
void my_library_init() {
    uv_timer_init(uv_default_loop(), &my_timer);
}

// GOOD: Accept loop parameter
void my_library_init(uv_loop_t* loop) {
    uv_timer_init(loop, &my_timer);
}
```

**Pitfall 7: Not cleaning up request objects**
```cpp
// BAD: Memory leak from request
uv_fs_t req;
uv_fs_open(uv_default_loop(), &req, "file.txt", O_RDONLY, 0, NULL);
// req not cleaned up

// GOOD: Always cleanup
uv_fs_t req;
uv_fs_open(uv_default_loop(), &req, "file.txt", O_RDONLY, 0, NULL);
uv_fs_req_cleanup(&req);
```

**Pitfall 8: Assuming uv_run returns immediately**
```cpp
// BAD: Blocking the event loop
void process_data() {
    // Some long computation
    sleep(5); // Blocks event loop!
}

// GOOD: Use work queue for blocking operations
void work_cb(uv_work_t* req) {
    // Long computation here
}
void after_work_cb(uv_work_t* req, int status) {
    // Called on event loop thread
}
```
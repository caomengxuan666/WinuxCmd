# Safety

**Condition 1: NEVER call uv_close on a handle that's already closing**
```cpp
// BAD: Double close
uv_close((uv_handle_t*)&timer, NULL);
uv_close((uv_handle_t*)&timer, NULL); // Undefined behavior!

// GOOD: Check state or use flag
int closing = 0;
void close_timer() {
    if (!closing) {
        closing = 1;
        uv_close((uv_handle_t*)&timer, NULL);
    }
}
```

**Condition 2: NEVER access handle after uv_close callback without reinitializing**
```cpp
// BAD: Use after close
void on_close(uv_handle_t* handle) {
    uv_timer_start((uv_timer_t*)handle, timer_cb, 1000, 0); // Handle is invalid!
}

// GOOD: Reinitialize if needed
void on_close(uv_handle_t* handle) {
    uv_timer_init(uv_default_loop(), (uv_timer_t*)handle);
    uv_timer_start((uv_timer_t*)handle, timer_cb, 1000, 0);
}
```

**Condition 3: NEVER call uv_run recursively**
```cpp
// BAD: Recursive event loop
void callback(uv_timer_t* handle) {
    uv_run(uv_default_loop(), UV_RUN_NOWAIT); // Re-entrancy issues!
}

// GOOD: Use uv_stop or async handles
void callback(uv_timer_t* handle) {
    uv_stop(uv_default_loop()); // Safe way to exit
}
```

**Condition 4: NEVER pass uninitialized handles to libuv functions**
```cpp
// BAD: Uninitialized handle
uv_tcp_t client;
uv_accept(server, (uv_stream_t*)&client); // client not initialized!

// GOOD: Always initialize first
uv_tcp_t client;
uv_tcp_init(uv_default_loop(), &client);
uv_accept(server, (uv_stream_t*)&client);
```

**Condition 5: NEVER modify uv_buf_t data after starting a read operation**
```cpp
// BAD: Buffer modification during read
uv_buf_t buf = uv_buf_init(data, size);
uv_read_start(stream, alloc_cb, read_cb);
strcpy(data, "modified"); // Race condition!

// GOOD: Use separate buffers
char* read_data = (char*)malloc(size);
uv_buf_t buf = uv_buf_init(read_data, size);
uv_read_start(stream, alloc_cb, read_cb);
// Don't touch read_data until callback
```
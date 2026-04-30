# Lifecycle

**Construction and initialization**
```cpp
// All handles must be initialized before use
uv_loop_t loop;
uv_loop_init(&loop); // Initialize custom loop

uv_tcp_t server;
uv_tcp_init(&loop, &server); // Initialize TCP handle

// Default loop for simple cases
uv_tcp_t client;
uv_tcp_init(uv_default_loop(), &client);
```

**Resource management with close callbacks**
```cpp
typedef struct {
    uv_tcp_t handle;
    char* buffer;
    int is_closing;
} my_connection_t;

void on_close(uv_handle_t* handle) {
    my_connection_t* conn = (my_connection_t*)handle->data;
    free(conn->buffer);
    free(conn);
}

void close_connection(my_connection_t* conn) {
    if (!conn->is_closing) {
        conn->is_closing = 1;
        uv_close((uv_handle_t*)&conn->handle, on_close);
    }
}
```

**Move semantics (libuv doesn't support moving handles)**
```cpp
// BAD: Attempting to move a handle
uv_tcp_t handle1;
uv_tcp_init(loop, &handle1);
uv_tcp_t handle2 = handle1; // Shallow copy - dangerous!

// GOOD: Use pointers and reinitialize
uv_tcp_t* create_tcp(uv_loop_t* loop) {
    uv_tcp_t* handle = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, handle);
    return handle;
}

void move_tcp(uv_loop_t* new_loop, uv_tcp_t** handle) {
    uv_close((uv_handle_t*)*handle, NULL);
    *handle = create_tcp(new_loop);
}
```

**Loop lifecycle management**
```cpp
int main() {
    uv_loop_t* loop = uv_default_loop();
    
    // Initialize handles
    uv_timer_t timer;
    uv_timer_init(loop, &timer);
    
    // Start operations
    uv_timer_start(&timer, timer_cb, 1000, 0);
    
    // Run event loop
    uv_run(loop, UV_RUN_DEFAULT);
    
    // Cleanup
    uv_close((uv_handle_t*)&timer, NULL);
    uv_run(loop, UV_RUN_NOWAIT); // Process close callbacks
    uv_loop_close(loop); // Only after all handles closed
    
    return 0;
}
```
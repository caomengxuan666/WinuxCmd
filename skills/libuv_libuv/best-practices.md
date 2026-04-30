# Best Practices

**Pattern 1: Centralized error handling**
```cpp
#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(r, msg) do { \
    if (r < 0) { \
        fprintf(stderr, "%s: %s\n", msg, uv_strerror(r)); \
        exit(1); \
    } \
} while(0)

int main() {
    uv_loop_t* loop = uv_default_loop();
    uv_tcp_t server;
    CHECK(uv_tcp_init(loop, &server), "Failed to init server");
    struct sockaddr_in addr;
    CHECK(uv_ip4_addr("0.0.0.0", 8080, &addr), "Failed to parse address");
    CHECK(uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0), "Failed to bind");
    CHECK(uv_listen((uv_stream_t*)&server, 128, NULL), "Failed to listen");
    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}
```

**Pattern 2: Resource cleanup with close callbacks**
```cpp
typedef struct {
    uv_tcp_t* client;
    uv_write_t* write_req;
    char* data;
} client_t;

void on_client_close(uv_handle_t* handle) {
    client_t* client = (client_t*)handle->data;
    free(client->data);
    free(client->write_req);
    free(client);
}

void cleanup_client(client_t* client) {
    uv_close((uv_handle_t*)client->client, on_client_close);
}
```

**Pattern 3: Using work queue for blocking operations**
```cpp
typedef struct {
    uv_work_t req;
    char* input;
    char* output;
} work_data_t;

void work_cb(uv_work_t* req) {
    work_data_t* data = (work_data_t*)req->data;
    // Perform blocking operation
    data->output = process_data(data->input);
}

void after_work_cb(uv_work_t* req, int status) {
    work_data_t* data = (work_data_t*)req->data;
    printf("Result: %s\n", data->output);
    free(data->input);
    free(data->output);
    free(data);
}

void start_async_work(const char* input) {
    work_data_t* data = (work_data_t*)malloc(sizeof(work_data_t));
    data->input = strdup(input);
    data->req.data = data;
    uv_queue_work(uv_default_loop(), &data->req, work_cb, after_work_cb);
}
```

**Pattern 4: Timer-based heartbeat for connections**
```cpp
void heartbeat_cb(uv_timer_t* handle) {
    connection_t* conn = (connection_t*)handle->data;
    if (conn->last_activity + TIMEOUT < uv_now(uv_default_loop())) {
        cleanup_connection(conn); // Timeout
    }
}

void setup_heartbeat(connection_t* conn) {
    uv_timer_init(uv_default_loop(), &conn->heartbeat);
    conn->heartbeat.data = conn;
    uv_timer_start(&conn->heartbeat, heartbeat_cb, 1000, 1000);
}
```
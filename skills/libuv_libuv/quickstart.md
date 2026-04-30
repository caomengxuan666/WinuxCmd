# Quickstart

```cpp
// Basic event loop with timer
#include <uv.h>
#include <stdio.h>

void timer_cb(uv_timer_t* handle) {
    printf("Timer fired!\n");
    uv_timer_stop(handle);
    uv_stop(uv_default_loop());
}

int main() {
    uv_loop_t* loop = uv_default_loop();
    uv_timer_t timer;
    uv_timer_init(loop, &timer);
    uv_timer_start(&timer, timer_cb, 1000, 0); // 1 second delay
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;
}
```

```cpp
// TCP echo server
#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
    if (nread > 0) {
        uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
        uv_buf_t write_buf = uv_buf_init(buf->base, nread);
        uv_write(req, client, &write_buf, 1, NULL);
        return;
    }
    if (nread < 0) {
        uv_close((uv_handle_t*)client, NULL);
    }
    free(buf->base);
}

void on_new_connection(uv_stream_t* server, int status) {
    if (status < 0) return;
    uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);
    if (uv_accept(server, (uv_stream_t*)client) == 0) {
        uv_read_start((uv_stream_t*)client, alloc_buffer, echo_read);
    }
}

int main() {
    uv_tcp_t server;
    uv_tcp_init(uv_default_loop(), &server);
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", 7000, &addr);
    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*)&server, 128, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error: %s\n", uv_strerror(r));
        return 1;
    }
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}
```

```cpp
// Async file read
#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

void on_read(uv_fs_t* req) {
    if (req->result < 0) {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    } else {
        printf("Read %ld bytes: %s\n", req->result, (char*)req->data);
    }
    uv_fs_req_cleanup(req);
    free(req);
}

int main() {
    uv_fs_t* req = (uv_fs_t*)malloc(sizeof(uv_fs_t));
    req->data = malloc(4096);
    uv_buf_t buf = uv_buf_init((char*)req->data, 4096);
    uv_fs_read(uv_default_loop(), req, open("test.txt", O_RDONLY), &buf, 1, 0, on_read);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    free(req->data);
    return 0;
}
```

```cpp
// DNS resolution
#include <uv.h>
#include <stdio.h>

void on_resolve(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res) {
    if (status < 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", uv_strerror(status));
        return;
    }
    char addr[17] = {'\0'};
    uv_ip4_name((struct sockaddr_in*)res->ai_addr, addr, 16);
    printf("Resolved: %s\n", addr);
    uv_freeaddrinfo(res);
}

int main() {
    uv_getaddrinfo_t resolver;
    struct addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    uv_getaddrinfo(uv_default_loop(), &resolver, on_resolve, "google.com", "80", &hints);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}
```

```cpp
// Signal handling
#include <uv.h>
#include <stdio.h>

void signal_cb(uv_signal_t* handle, int signum) {
    printf("Received signal %d\n", signum);
    uv_signal_stop(handle);
    uv_stop(uv_default_loop());
}

int main() {
    uv_signal_t sig;
    uv_signal_init(uv_default_loop(), &sig);
    uv_signal_start(&sig, signal_cb, SIGINT);
    printf("Press Ctrl+C to stop...\n");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}
```

```cpp
// Idle handle for background tasks
#include <uv.h>
#include <stdio.h>

void idle_cb(uv_idle_t* handle) {
    static int count = 0;
    count++;
    if (count >= 5) {
        printf("Idle task done\n");
        uv_idle_stop(handle);
        uv_stop(uv_default_loop());
    }
}

int main() {
    uv_idle_t idle;
    uv_idle_init(uv_default_loop(), &idle);
    uv_idle_start(&idle, idle_cb);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}
```
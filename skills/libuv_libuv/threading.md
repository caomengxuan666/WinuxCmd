# Threading

**Thread safety guarantees**
```cpp
// libuv is NOT thread-safe by default
// Only call uv_* functions from the event loop thread

// BAD: Calling from worker thread
void worker_thread(void* arg) {
    uv_timer_start(&timer, timer_cb, 1000, 0); // Unsafe!
}

// GOOD: Use uv_async_send to communicate with event loop
uv_async_t async;

void async_cb(uv_async_t* handle) {
    // This runs on event loop thread
    uv_timer_start(&timer, timer_cb, 1000, 0);
}

void worker_thread(void* arg) {
    uv_async_send(&async); // Thread-safe way to signal
}
```

**Using uv_async for cross-thread communication**
```cpp
#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uv_async_t async;
    uv_loop_t* loop;
    int data;
} async_data_t;

void async_cb(uv_async_t* handle) {
    async_data_t* data = (async_data_t*)handle->data;
    printf("Received data: %d\n", data->data);
}

void* worker(void* arg) {
    async_data_t* data = (async_data_t*)arg;
    for (int i = 0; i < 5; i++) {
        data->data = i;
        uv_async_send(&data->async);
        sleep(1);
    }
    return NULL;
}

int main() {
    uv_loop_t* loop = uv_default_loop();
    async_data_t data;
    data.loop = loop;
    
    uv_async_init(loop, &data.async, async_cb);
    data.async.data = &data;
    
    pthread_t thread;
    pthread_create(&thread, NULL, worker, &data);
    
    uv_run(loop, UV_RUN_DEFAULT);
    
    uv_close((uv_handle_t*)&data.async, NULL);
    uv_run(loop, UV_RUN_NOWAIT);
    
    pthread_join(thread, NULL);
    return 0;
}
```

**Thread pool work queue**
```cpp
#include <uv.h>
#include <stdio.h>

typedef struct {
    uv_work_t req;
    int input;
    int result;
} work_t;

void work_cb(uv_work_t* req) {
    work_t* work = (work_t*)req->data;
    // This runs on thread pool thread
    work->result = work->input * 2;
}

void after_work_cb(uv_work_t* req, int status) {
    work_t* work = (work_t*)req->data;
    // This runs on event loop thread
    printf("Result: %d\n", work->result);
    free(work);
}

void queue_work(int value) {
    work_t* work = (work_t*)malloc(sizeof(work_t));
    work->input = value;
    work->req.data = work;
    uv_queue_work(uv_default_loop(), &work->req, work_cb, after_work_cb);
}
```

**Mutex and synchronization primitives**
```cpp
#include <uv.h>
#include <stdio.h>

uv_mutex_t mutex;
uv_cond_t cond;
int shared_data = 0;

void producer(void* arg) {
    uv_mutex_lock(&mutex);
    shared_data = 42;
    uv_cond_signal(&cond);
    uv_mutex_unlock(&mutex);
}

void consumer(void* arg) {
    uv_mutex_lock(&mutex);
    while (shared_data == 0) {
        uv_cond_wait(&cond, &mutex);
    }
    printf("Got data: %d\n", shared_data);
    uv_mutex_unlock(&mutex);
}

int main() {
    uv_mutex_init(&mutex);
    uv_cond_init(&cond);
    
    uv_thread_t producer_thread, consumer_thread;
    uv_thread_create(&producer_thread, producer, NULL);
    uv_thread_create(&consumer_thread, consumer, NULL);
    
    uv_thread_join(&producer_thread);
    uv_thread_join(&consumer_thread);
    
    uv_mutex_destroy(&mutex);
    uv_cond_destroy(&cond);
    
    return 0;
}
```
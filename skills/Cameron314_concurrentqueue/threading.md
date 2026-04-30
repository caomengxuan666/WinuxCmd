# Threading

### Thread safety guarantees
```cpp
// ConcurrentQueue is fully thread-safe for concurrent access
moodycamel::ConcurrentQueue<int> q;

// Multiple threads can enqueue simultaneously
std::vector<std::thread> producers;
for (int i = 0; i < 4; ++i) {
    producers.emplace_back([&q, i]() {
        moodycamel::ProducerToken ptok(q);
        for (int j = 0; j < 100; ++j) {
            q.enqueue(ptok, i * 100 + j);
        }
    });
}

// Multiple threads can dequeue simultaneously
std::vector<std::thread> consumers;
std::atomic<int> total{0};
for (int i = 0; i < 4; ++i) {
    consumers.emplace_back([&q, &total]() {
        moodycamel::ConsumerToken ctok(q);
        int item;
        while (q.try_dequeue(ctok, item)) {
            total.fetch_add(1);
        }
    });
}
```

### Producer tokens are thread-safe per thread
```cpp
moodycamel::ConcurrentQueue<int> q;

// Each thread should have its own token
std::thread t1([&q]() {
    moodycamel::ProducerToken ptok(q);
    q.enqueue(ptok, 1);
});

std::thread t2([&q]() {
    moodycamel::ProducerToken ptok(q);
    q.enqueue(ptok, 2);
});

// NEVER share a token between threads without synchronization
moodycamel::ProducerToken shared_ptok(q);
// BAD: Both threads using same token
std::thread t3([&q, &shared_ptok]() { q.enqueue(shared_ptok, 3); });
std::thread t4([&q, &shared_ptok]() { q.enqueue(shared_ptok, 4); }); // Race!
```

### Consumer tokens are thread-safe per thread
```cpp
moodycamel::ConcurrentQueue<int> q;

// Each consumer thread gets its own token
std::thread c1([&q]() {
    moodycamel::ConsumerToken ctok(q);
    int item;
    while (q.try_dequeue(ctok, item)) {
        process(item);
    }
});

std::thread c2([&q]() {
    moodycamel::ConsumerToken ctok(q);
    int item;
    while (q.try_dequeue(ctok, item)) {
        process(item);
    }
});
```

### Blocking version thread safety
```cpp
moodycamel::BlockingConcurrentQueue<int> q;

// wait_dequeue blocks until item available
std::thread consumer([&q]() {
    int item;
    q.wait_dequeue(item); // Thread-safe blocking
    process(item);
});

// Multiple consumers can block simultaneously
std::vector<std::thread> consumers;
for (int i = 0; i < 4; ++i) {
    consumers.emplace_back([&q]() {
        int item;
        while (q.wait_dequeue_timed(item, std::chrono::seconds(1))) {
            process(item);
        }
    });
}
```

### Memory ordering considerations
```cpp
// Queue provides happens-before for enqueue -> dequeue
std::atomic<int> data{0};
moodycamel::ConcurrentQueue<int> q;

// Thread 1
data.store(42, std::memory_order_release);
q.enqueue(1); // Makes data visible to consumer

// Thread 2
int item;
if (q.try_dequeue(item)) {
    int val = data.load(std::memory_order_acquire);
    // val is guaranteed to be 42
}

// But NOT for non-queue operations between enqueues
// Thread 1
q.enqueue(1);
data.store(42); // May not be visible to consumer of item 1
```

### Thread termination safety
```cpp
// Implicit producers use thread-local storage
// When a thread exits, its producer may be recycled
moodycamel::ConcurrentQueue<int> q;

// Safe pattern: Use explicit tokens
void worker_thread() {
    moodycamel::ProducerToken ptok(q);
    q.enqueue(ptok, 42);
    // Token destructor handles cleanup
}

// Unsafe pattern: Implicit operations on detached threads
std::thread([]() {
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(42); // Implicit - may cause issues on thread exit
}).detach();
```
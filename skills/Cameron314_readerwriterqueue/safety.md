# Safety

### Condition 1: NEVER call enqueue from multiple threads
```cpp
// BAD: Multiple producers
moodycamel::ReaderWriterQueue<int> q(100);
std::vector<std::thread> producers;
for (int i = 0; i < 4; ++i) {
    producers.emplace_back([&]() { q.enqueue(i); });  // Data race!
}

// GOOD: Single producer
std::thread producer([&]() {
    for (int i = 0; i < 100; ++i) {
        q.enqueue(i);
    }
});
```

### Condition 2: NEVER call dequeue from multiple threads
```cpp
// BAD: Multiple consumers
moodycamel::ReaderWriterQueue<int> q(100);
std::vector<std::thread> consumers;
for (int i = 0; i < 4; ++i) {
    consumers.emplace_back([&]() { 
        int item;
        q.try_dequeue(item);  // Data race!
    });
}

// GOOD: Single consumer
std::thread consumer([&]() {
    int item;
    while (q.try_dequeue(item)) {
        // Process item
    }
});
```

### Condition 3: NEVER destroy queue while threads are blocked
```cpp
// BAD: Destruction with blocked threads
auto q = std::make_unique<moodycamel::BlockingReaderWriterQueue<int>>();
std::thread t([&]() {
    int item;
    q->wait_dequeue(item);  // Will block forever
});
q.reset();  // Undefined behavior!
t.join();

// GOOD: Signal termination before destruction
moodycamel::BlockingReaderWriterQueue<int> q2;
std::atomic<bool> stop{false};
std::thread t2([&]() {
    int item;
    while (!stop) {
        if (q2.wait_dequeue_timed(item, std::chrono::milliseconds(10))) {
            // Process
        }
    }
});
stop = true;
t2.join();
```

### Condition 4: NEVER use with GCC 4.6 or earlier
```cpp
// BAD: Using with GCC 4.6
// #if defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 6
// #error "GCC 4.6 has atomic fence bugs. Use GCC 4.7+"
// #endif

// GOOD: Check compiler version
#if defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 6
    static_assert(false, "GCC 4.6 has atomic fence bugs");
#endif
```

### Condition 5: NEVER switch producer/consumer roles
```cpp
// BAD: Thread switching roles
moodycamel::ReaderWriterQueue<int> q(100);
std::thread t([&]() {
    q.enqueue(1);  // Producer role
    int item;
    q.try_dequeue(item);  // Consumer role - undefined!
});

// GOOD: Keep roles consistent
moodycamel::ReaderWriterQueue<int> q2(100);
std::thread producer([&]() { q2.enqueue(1); });
std::thread consumer([&]() { 
    int item;
    q2.try_dequeue(item);
});
```
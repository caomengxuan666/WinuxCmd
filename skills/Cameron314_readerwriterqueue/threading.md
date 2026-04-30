# Threading

### Thread safety guarantees
```cpp
// ReaderWriterQueue is SPSC (Single Producer, Single Consumer)
// Thread-safe for exactly one producer and one consumer

moodycamel::ReaderWriterQueue<int> q(100);

// Thread 1 (Producer only)
std::thread producer([&]() {
    for (int i = 0; i < 1000; ++i) {
        q.enqueue(i);  // Safe - only one producer
    }
});

// Thread 2 (Consumer only)
std::thread consumer([&]() {
    int value;
    while (q.try_dequeue(value)) {
        // Process value - safe, only one consumer
    }
});

producer.join();
consumer.join();
```

### Concurrent access patterns
```cpp
// CORRECT: Single producer, single consumer
moodycamel::ReaderWriterQueue<Task> task_queue(256);

// Producer thread
void producer_thread() {
    while (has_work()) {
        Task t = generate_task();
        task_queue.enqueue(std::move(t));  // Only this thread calls enqueue
    }
}

// Consumer thread
void consumer_thread() {
    Task t;
    while (task_queue.try_dequeue(t)) {  // Only this thread calls dequeue
        process_task(t);
    }
}

// WRONG: Multiple producers
void wrong_producer() {
    // This would be called from multiple threads
    task_queue.enqueue(Task{});  // Data race!
}
```

### Blocking queue thread safety
```cpp
// BlockingReaderWriterQueue adds wait_dequeue and wait_dequeue_timed
// Same SPSC constraints apply

moodycamel::BlockingReaderWriterQueue<int> bq(100);

// Consumer with timeout
std::thread consumer([&]() {
    int item;
    while (true) {
        // wait_dequeue_timed is safe for single consumer
        if (bq.wait_dequeue_timed(item, std::chrono::milliseconds(100))) {
            process(item);
        } else {
            // Timeout - check for shutdown signal
            if (should_stop()) break;
        }
    }
});

// Producer
std::thread producer([&]() {
    for (int i = 0; i < 100; ++i) {
        bq.enqueue(i);  // Safe - single producer
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
});
```

### Thread safety for peek operations
```cpp
// peek() is only safe for the consumer thread
moodycamel::ReaderWriterQueue<int> q(100);
q.enqueue(42);

// Consumer thread only
std::thread consumer([&]() {
    int* front = q.peek();  // Safe - only consumer calls peek
    if (front) {
        int value = *front;
        q.try_dequeue(value);  // Safe - consumer
    }
});

// Producer should never call peek
std::thread producer([&]() {
    q.enqueue(1);  // Safe - producer
    // int* front = q.peek();  // WRONG - undefined behavior
});
```
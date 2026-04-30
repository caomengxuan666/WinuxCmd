# Best Practices

### Pre-allocate sufficient capacity
```cpp
// GOOD: Pre-allocate based on expected workload
moodycamel::ReaderWriterQueue<Message> q(1024);  // Reserve 1024 slots

// For high-throughput scenarios, allocate generously
constexpr size_t kExpectedBurst = 4096;
moodycamel::ReaderWriterQueue<Packet> packet_queue(kExpectedBurst);
```

### Use try_enqueue for real-time paths
```cpp
// GOOD: Use try_enqueue in real-time or latency-critical code
moodycamel::ReaderWriterQueue<Event> event_queue(256);

void realtime_handler() {
    Event e = generate_event();
    if (!event_queue.try_enqueue(e)) {
        // Queue full - handle overflow (e.g., drop event, log warning)
        handle_queue_overflow(e);
    }
}

// Use enqueue for non-critical paths where allocation is acceptable
void background_task() {
    Event e = generate_large_event();
    event_queue.enqueue(e);  // May allocate
}
```

### Implement graceful shutdown
```cpp
// GOOD: Pattern for clean shutdown with blocking queue
class Worker {
    moodycamel::BlockingReaderWriterQueue<Task> queue_;
    std::atomic<bool> running_{true};
    std::thread consumer_;
    
public:
    Worker() : queue_(256) {
        consumer_ = std::thread([this] { consume(); });
    }
    
    ~Worker() {
        running_ = false;
        consumer_.join();
    }
    
    void enqueue(Task task) {
        queue_.enqueue(std::move(task));
    }
    
private:
    void consume() {
        Task task;
        while (running_) {
            if (queue_.wait_dequeue_timed(task, std::chrono::milliseconds(10))) {
                process(task);
            }
        }
    }
};
```

### Use move semantics for expensive objects
```cpp
// GOOD: Move large objects instead of copying
moodycamel::ReaderWriterQueue<std::vector<uint8_t>> buffer_queue(100);

// Producer
std::vector<uint8_t> data = generate_large_buffer();
buffer_queue.enqueue(std::move(data));  // No copy

// Consumer
std::vector<uint8_t> result;
buffer_queue.try_dequeue(result);  // Move semantics
```

### Monitor queue size for health checks
```cpp
// GOOD: Periodically check queue depth
moodycamel::ReaderWriterQueue<Message> queue(1024);

void health_check() {
    size_t approx_size = queue.size_approx();
    if (approx_size > 800) {  // 80% capacity
        log_warning("Queue nearing capacity: ", approx_size);
    }
    if (approx_size == 0 && !producer_active()) {
        log_info("Queue empty, system idle");
    }
}
```
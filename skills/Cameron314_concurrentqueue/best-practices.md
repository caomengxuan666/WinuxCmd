# Best Practices

### Always use tokens for repeated operations
```cpp
moodycamel::ConcurrentQueue<Task> q;

// Producer thread
void producer_thread() {
    moodycamel::ProducerToken ptok(q);
    while (has_work()) {
        Task t = get_task();
        q.enqueue(ptok, std::move(t));
    }
}

// Consumer thread
void consumer_thread() {
    moodycamel::ConsumerToken ctok(q);
    Task t;
    while (q.try_dequeue(ctok, t)) {
        process_task(t);
    }
}
```

### Pre-allocate memory for predictable performance
```cpp
// Estimate maximum queue size
const size_t MAX_QUEUE_SIZE = 100000;

// Pre-allocate to avoid runtime allocations
moodycamel::ConcurrentQueue<Message> q(MAX_QUEUE_SIZE);

// Now enqueue without allocation overhead
for (int i = 0; i < MAX_QUEUE_SIZE; ++i) {
    q.enqueue(Message{});
}
```

### Use bulk operations for batch processing
```cpp
moodycamel::ConcurrentQueue<DataPoint> q;

// Producer: batch enqueue
void produce_batch(const std::vector<DataPoint>& batch) {
    moodycamel::ProducerToken ptok(q);
    q.enqueue_bulk(ptok, batch.begin(), batch.size());
}

// Consumer: batch dequeue
std::vector<DataPoint> consume_batch(size_t max_items) {
    moodycamel::ConsumerToken ctok(q);
    std::vector<DataPoint> result(max_items);
    size_t actual = q.try_dequeue_bulk(ctok, result.begin(), max_items);
    result.resize(actual);
    return result;
}
```

### Handle blocking operations with timeouts
```cpp
moodycamel::BlockingConcurrentQueue<Request> q;

void consumer_with_timeout() {
    Request req;
    while (running) {
        if (q.wait_dequeue_timed(req, std::chrono::milliseconds(100))) {
            process_request(req);
        } else {
            // Check shutdown flag or do maintenance
            check_shutdown();
        }
    }
}
```

### Use RAII for token management
```cpp
class QueueProducer {
    moodycamel::ConcurrentQueue<Item>& queue_;
    moodycamel::ProducerToken token_;
public:
    explicit QueueProducer(moodycamel::ConcurrentQueue<Item>& q) 
        : queue_(q), token_(q) {}
    
    void enqueue(Item item) {
        queue_.enqueue(token_, std::move(item));
    }
};

class QueueConsumer {
    moodycamel::ConcurrentQueue<Item>& queue_;
    moodycamel::ConsumerToken token_;
public:
    explicit QueueConsumer(moodycamel::ConcurrentQueue<Item>& q) 
        : queue_(q), token_(q) {}
    
    bool try_dequeue(Item& item) {
        return queue_.try_dequeue(token_, item);
    }
};
```

### Proper shutdown pattern
```cpp
std::atomic<bool> shutdown_flag{false};
moodycamel::BlockingConcurrentQueue<Work> q;

// Producer
void producer() {
    while (!shutdown_flag) {
        Work w = generate_work();
        q.enqueue(std::move(w));
    }
    // Signal end of work
    q.enqueue(Work::poison_pill());
}

// Consumer
void consumer() {
    Work w;
    while (true) {
        q.wait_dequeue(w);
        if (w.is_poison_pill()) break;
        process(w);
    }
}
```
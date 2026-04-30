# Lifecycle

### Construction
```cpp
// Default construction (minimal capacity)
moodycamel::ReaderWriterQueue<int> q1;

// Construction with initial capacity
moodycamel::ReaderWriterQueue<int> q2(1000);  // Pre-allocate 1000 slots

// Blocking variant
moodycamel::BlockingReaderWriterQueue<int> bq(500);

// Circular buffer variant (fixed size)
moodycamel::BlockingReaderWriterCircularBuffer<int> cb(1024);
```

### Destruction
```cpp
// GOOD: Ensure no threads are blocked before destruction
{
    moodycamel::BlockingReaderWriterQueue<int> q(100);
    std::thread consumer([&]() {
        int item;
        while (q.wait_dequeue_timed(item, std::chrono::milliseconds(10))) {
            // Process
        }
    });
    
    // Signal termination
    // Queue goes out of scope after consumer finishes
    consumer.join();
}  // Safe destruction

// BAD: Destruction with blocked threads
moodycamel::BlockingReaderWriterQueue<int>* q = 
    new moodycamel::BlockingReaderWriterQueue<int>();
std::thread t([&]() { 
    int item;
    q->wait_dequeue(item);  // Blocks forever
});
delete q;  // Undefined behavior!
```

### Move semantics
```cpp
// Move construction
moodycamel::ReaderWriterQueue<int> q1(100);
q1.enqueue(42);

moodycamel::ReaderWriterQueue<int> q2(std::move(q1));  // Move queue
int value;
q2.try_dequeue(value);  // Works
// q1 is now in valid but unspecified state

// Move assignment
moodycamel::ReaderWriterQueue<int> q3(50);
q3 = std::move(q2);  // Move assign

// Note: Moving is not thread-safe - ensure no concurrent access
```

### Resource management
```cpp
// GOOD: Use RAII for queue management
class Pipeline {
    moodycamel::ReaderWriterQueue<Data> queue_;
    std::thread consumer_;
    
public:
    Pipeline(size_t capacity) : queue_(capacity) {
        consumer_ = std::thread([this] { process(); });
    }
    
    ~Pipeline() {
        // Signal stop and wait for consumer
        consumer_.join();
    }
    
    void submit(Data data) {
        queue_.enqueue(std::move(data));
    }
    
private:
    void process() {
        Data data;
        while (queue_.try_dequeue(data)) {
            // Process data
        }
    }
};
```
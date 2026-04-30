# Pitfalls

### Pitfall 1: Using with multiple producers
```cpp
// BAD: Multiple threads calling enqueue
moodycamel::ReaderWriterQueue<int> q(100);
std::thread t1([&]() { q.enqueue(1); });  // Undefined behavior!
std::thread t2([&]() { q.enqueue(2); });  // Only one producer allowed
t1.join(); t2.join();

// GOOD: Use concurrentqueue for multiple producers
moodycamel::ConcurrentQueue<int> mq(100);
std::thread t1([&]() { mq.enqueue(1); });
std::thread t2([&]() { mq.enqueue(2); });
t1.join(); t2.join();
```

### Pitfall 2: Destroying queue while thread is blocked
```cpp
// BAD: Destroying queue with blocked consumer
moodycamel::BlockingReaderWriterQueue<int>* q = 
    new moodycamel::BlockingReaderWriterQueue<int>();
std::thread consumer([&]() {
    int item;
    q->wait_dequeue(item);  // Blocks forever
});
delete q;  // Undefined behavior!
consumer.join();

// GOOD: Signal consumer to stop first
moodycamel::BlockingReaderWriterQueue<int> q2;
std::atomic<bool> done{false};
std::thread consumer2([&]() {
    int item;
    while (!done) {
        if (q2.wait_dequeue_timed(item, std::chrono::milliseconds(10))) {
            // Process item
        }
    }
});
done = true;
consumer2.join();
```

### Pitfall 3: Using GCC 4.6 or earlier
```cpp
// BAD: Compiling with GCC 4.6
// This will produce undefined behavior due to atomic fence bugs
moodycamel::ReaderWriterQueue<int> q(100);
q.enqueue(1);

// GOOD: Use GCC 4.7+ or add workaround
// #if __GNUC__ == 4 && __GNUC_MINOR__ <= 6
// #error "GCC 4.6 has atomic fence bugs. Use GCC 4.7+"
// #endif
```

### Pitfall 4: Missing includes for fixed-width types
```cpp
// BAD: Missing <cstdint> include
#include <readerwriterqueue.h>  // May fail on MSVC x64

// GOOD: Include <cstdint> before the queue header
#include <cstdint>
#include <readerwriterqueue.h>
```

### Pitfall 5: Using try_enqueue when queue is full
```cpp
// BAD: Assuming try_enqueue always succeeds
moodycamel::ReaderWriterQueue<int> q(1);
q.try_enqueue(1);
q.try_enqueue(2);  // May fail silently!

// GOOD: Check return value
if (!q.try_enqueue(2)) {
    // Handle full queue - maybe use enqueue instead
    q.enqueue(2);  // This will allocate more space
}
```

### Pitfall 6: Incorrect timed wait with nanosecond boundary
```cpp
// BAD: Using exactly 1 billion nanoseconds
moodycamel::BlockingReaderWriterQueue<int> q(10);
int item;
// This may fail on some platforms
q.wait_dequeue_timed(item, std::chrono::nanoseconds(1000000000));

// GOOD: Use slightly less than 1 billion
q.wait_dequeue_timed(item, std::chrono::nanoseconds(999999999));
// Or use milliseconds
q.wait_dequeue_timed(item, std::chrono::milliseconds(1000));
```
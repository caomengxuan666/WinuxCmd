# Quickstart

```cpp
#include <readerwriterqueue.h>
#include <thread>
#include <cassert>

// Pattern 1: Basic enqueue and dequeue
moodycamel::ReaderWriterQueue<int> q(100);
q.enqueue(42);
int value;
bool success = q.try_dequeue(value);
assert(success && value == 42);

// Pattern 2: Try operations (non-allocating)
moodycamel::ReaderWriterQueue<int> q2(10);
bool enqueued = q2.try_enqueue(1);  // Never allocates
int result;
bool dequeued = q2.try_dequeue(result);

// Pattern 3: Peek at front element
moodycamel::ReaderWriterQueue<std::string> q3(50);
q3.enqueue("hello");
std::string* front = q3.peek();
assert(front && *front == "hello");

// Pattern 4: Move semantics
moodycamel::ReaderWriterQueue<std::vector<int>> q4(10);
std::vector<int> data = {1, 2, 3};
q4.enqueue(std::move(data));  // Moves instead of copying

// Pattern 5: Blocking queue with wait_dequeue
moodycamel::BlockingReaderWriterQueue<int> bq(10);
std::thread consumer([&]() {
    int item;
    bq.wait_dequeue(item);  // Blocks until item available
    // Process item
});
bq.enqueue(99);
consumer.join();

// Pattern 6: Blocking with timeout
moodycamel::BlockingReaderWriterQueue<int> bq2(10);
std::thread consumer2([&]() {
    int item;
    if (bq2.wait_dequeue_timed(item, std::chrono::milliseconds(100))) {
        // Item received within timeout
    }
});
bq2.enqueue(42);
consumer2.join();

// Pattern 7: Circular buffer (fixed size)
moodycamel::BlockingReaderWriterCircularBuffer<int> cb(1024);
cb.try_enqueue(1);
int out;
cb.try_dequeue(out);
assert(out == 1);
```
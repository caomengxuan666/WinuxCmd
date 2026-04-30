# Performance

### Performance characteristics
```cpp
// ConcurrentQueue is designed for high throughput under contention
moodycamel::ConcurrentQueue<int> q;

// Fast path: Single producer, single consumer
// ~50-100ns per operation

// Contended path: Multiple producers/consumers
// ~100-500ns per operation (still very fast)

// Bulk operations are significantly faster per element
std::vector<int> items(1000);
// Bulk enqueue: ~10ns per element
q.enqueue_bulk(items.begin(), items.size());

// Individual enqueue: ~50ns per element
for (auto& item : items) {
    q.enqueue(item);
}
```

### Allocation patterns
```cpp
// Pre-allocation eliminates runtime allocations
moodycamel::ConcurrentQueue<int> q(100000);
// All memory allocated in constructor
// Subsequent enqueues are allocation-free until capacity exceeded

// Without pre-allocation, memory grows dynamically
moodycamel::ConcurrentQueue<int> q2;
// First enqueue allocates initial block
q2.enqueue(1);
// Subsequent enqueues may allocate more blocks

// Use try_enqueue to avoid allocation
if (!q2.try_enqueue(2)) {
    // Would need to allocate - handle differently
}
```

### Optimization tips
```cpp
// 1. Always use tokens in tight loops
moodycamel::ConcurrentQueue<int> q;
moodycamel::ProducerToken ptok(q);
for (int i = 0; i < 1000000; ++i) {
    q.enqueue(ptok, i); // 2-3x faster than without token
}

// 2. Use bulk operations for batches
std::vector<int> batch(1000);
moodycamel::ProducerToken ptok(q);
q.enqueue_bulk(ptok, batch.begin(), batch.size()); // Much faster

// 3. Pre-allocate to known size
q = moodycamel::ConcurrentQueue<int>(expected_max_size);

// 4. Use consumer tokens for dequeue
moodycamel::ConsumerToken ctok(q);
int item;
while (q.try_dequeue(ctok, item)) {
    process(item);
}

// 5. Avoid size_approx() in performance-critical paths
// size_approx() has O(number of sub-queues) complexity
```

### Memory overhead
```cpp
// Each sub-queue has a block size (default 32 elements)
// Each producer thread gets its own sub-queue
// Memory is reused internally

// Example: 4 producer threads, each enqueuing 1000 elements
moodycamel::ConcurrentQueue<int> q;
// Internal memory: ~4 * (1000/32 + 1) * block_size * sizeof(int)
// Plus overhead for management structures

// Block size can be controlled via template parameter
// moodycamel::ConcurrentQueue<int, 64> q; // 64-element blocks
```
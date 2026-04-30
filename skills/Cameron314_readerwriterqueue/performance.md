# Performance

### Performance characteristics
```cpp
// Enqueue/dequeue are O(1) and wait-free (no CAS loops)
// On x86, memory barriers compile to no-ops
// try_enqueue never allocates - guaranteed O(1)

// Benchmark example
moodycamel::ReaderWriterQueue<int> q(65536);
auto start = std::chrono::high_resolution_clock::now();

// Producer
for (int i = 0; i < 1000000; ++i) {
    q.enqueue(i);
}

// Consumer
int value;
while (q.try_dequeue(value)) {
    // Process
}

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
// Typically < 10 microseconds per million operations
```

### Allocation patterns
```cpp
// try_enqueue never allocates - use for real-time paths
moodycamel::ReaderWriterQueue<int> q(1000);

// First enqueue may allocate if initial capacity insufficient
q.enqueue(1);  // No allocation if capacity > 0

// enqueue may allocate when full
for (int i = 0; i < 2000; ++i) {
    q.enqueue(i);  // May allocate when capacity exceeded
}

// try_enqueue never allocates
for (int i = 0; i < 1000; ++i) {
    q.try_enqueue(i);  // Never allocates
}
```

### Optimization tips
```cpp
// 1. Pre-allocate sufficient capacity to avoid runtime allocations
moodycamel::ReaderWriterQueue<LargeObject> q(10000);  // Large initial capacity

// 2. Use try_enqueue in hot paths
void fast_path() {
    if (!q.try_enqueue(data)) {
        // Fallback to slower path
        slow_path_enqueue(data);
    }
}

// 3. Batch operations for better cache locality
void batch_enqueue(const std::vector<Item>& items) {
    for (const auto& item : items) {
        q.enqueue(item);  // Sequential enqueues are cache-friendly
    }
}

// 4. Use move semantics for large objects
void enqueue_large(LargeObject obj) {
    q.enqueue(std::move(obj));  // Avoid copy overhead
}
```

### Memory considerations
```cpp
// Queue uses contiguous memory blocks
// Memory is allocated in blocks, not per-element

// Check approximate memory usage
moodycamel::ReaderWriterQueue<int> q(1000);
// Initial allocation: ~1000 * sizeof(int) + overhead

// After growth
for (int i = 0; i < 10000; ++i) {
    q.enqueue(i);  // May allocate additional blocks
}

// Monitor size for memory management
size_t current_size = q.size_approx();
if (current_size > 8000) {
    // Consider throttling producer
}
```
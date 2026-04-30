# Pitfalls

### Pitfall 1: Using queue before construction is complete
```cpp
// BAD: Queue used from another thread before construction
moodycamel::ConcurrentQueue<int>* q;
std::thread t([&]() { q->enqueue(42); });
q = new moodycamel::ConcurrentQueue<int>();
t.join(); // Race condition!

// GOOD: Ensure queue fully constructed before sharing
moodycamel::ConcurrentQueue<int> q;
std::thread t([&]() { q.enqueue(42); });
t.join(); // Safe
```

### Pitfall 2: Assuming FIFO ordering across producers
```cpp
// BAD: Expecting order from two producers
moodycamel::ConcurrentQueue<int> q;
std::thread t1([&]() { q.enqueue(1); });
std::thread t2([&]() { q.enqueue(2); });
t1.join(); t2.join();
int a, b;
q.try_dequeue(a); // Could be 1 or 2
q.try_dequeue(b); // Could be 1 or 2

// GOOD: Use single producer token for ordering
moodycamel::ConcurrentQueue<int> q;
moodycamel::ProducerToken ptok(q);
std::thread t1([&]() { q.enqueue(ptok, 1); });
std::thread t2([&]() { q.enqueue(ptok, 2); }); // Need synchronization on ptok!
```

### Pitfall 3: Using size_approx() for precise counting
```cpp
// BAD: Relying on size_approx() for exact count
moodycamel::ConcurrentQueue<int> q;
q.enqueue(1);
q.enqueue(2);
if (q.size_approx() == 2) { // May be false due to eventual consistency
    // Wrong assumption
}

// GOOD: Use size_approx() only for estimates
size_t approx = q.size_approx();
if (approx > 0) {
    // There might be items, but no guarantee
}
```

### Pitfall 4: Not using tokens for performance
```cpp
// BAD: Implicit operations in tight loops
moodycamel::ConcurrentQueue<int> q;
for (int i = 0; i < 1000000; ++i) {
    q.enqueue(i); // Slower, allocates sub-queue each time
}

// GOOD: Use tokens for repeated operations
moodycamel::ConcurrentQueue<int> q;
moodycamel::ProducerToken ptok(q);
for (int i = 0; i < 1000000; ++i) {
    q.enqueue(ptok, i); // Faster, reuses sub-queue
}
```

### Pitfall 5: Destructing queue while threads are using it
```cpp
// BAD: Queue destroyed while threads still active
moodycamel::ConcurrentQueue<int>* q = new moodycamel::ConcurrentQueue<int>();
std::thread t([&]() { 
    q->enqueue(42);
    // Thread still running when queue destroyed
});
delete q; // Undefined behavior!
t.join();

// GOOD: Ensure all threads finish before destruction
moodycamel::ConcurrentQueue<int> q;
std::thread t([&]() { q.enqueue(42); });
t.join(); // Thread done before queue goes out of scope
```

### Pitfall 6: Using try_enqueue without checking return value
```cpp
// BAD: Ignoring try_enqueue return value
moodycamel::ConcurrentQueue<int> q;
q.try_enqueue(42); // May silently fail

// GOOD: Check return value
if (!q.try_enqueue(42)) {
    // Handle failure - allocate more space or retry
    q.enqueue(42); // Will allocate if needed
}
```

### Pitfall 7: Mixing tokens across threads without synchronization
```cpp
// BAD: Using same token from multiple threads
moodycamel::ConcurrentQueue<int> q;
moodycamel::ProducerToken ptok(q);
std::thread t1([&]() { q.enqueue(ptok, 1); });
std::thread t2([&]() { q.enqueue(ptok, 2); }); // Race on token!

// GOOD: Each thread gets its own token
moodycamel::ConcurrentQueue<int> q;
std::thread t1([&]() { 
    moodycamel::ProducerToken ptok(q);
    q.enqueue(ptok, 1); 
});
std::thread t2([&]() { 
    moodycamel::ProducerToken ptok(q);
    q.enqueue(ptok, 2); 
});
t1.join(); t2.join();
```

### Pitfall 8: Using blocking queue without timeout handling
```cpp
// BAD: Blocking indefinitely
moodycamel::BlockingConcurrentQueue<int> q;
int item;
q.wait_dequeue(item); // May block forever if no producer

// GOOD: Use timeout
if (q.wait_dequeue_timed(item, std::chrono::milliseconds(100))) {
    // Got item within timeout
} else {
    // Handle timeout
}
```
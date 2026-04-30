# Lifecycle

### Construction
```cpp
// Default construction
moodycamel::ConcurrentQueue<int> q1;

// With initial size estimate (pre-allocates memory)
moodycamel::ConcurrentQueue<int> q2(10000);

// Blocking version
moodycamel::BlockingConcurrentQueue<int> q3;

// Note: Queue must be fully constructed before any thread uses it
moodycamel::ConcurrentQueue<int> q4;
// Add memory barrier if needed
std::atomic_thread_fence(std::memory_order_release);
// Now safe to share with other threads
```

### Destruction
```cpp
// Automatic destruction when going out of scope
{
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(42);
} // q destroyed here - all threads must be done

// Dynamic allocation - must delete properly
auto q = new moodycamel::ConcurrentQueue<int>();
// ... use queue ...
// Ensure all threads finished before deletion
delete q; // Safe only if no threads are using it
```

### Move semantics
```cpp
// Move construction
moodycamel::ConcurrentQueue<int> q1;
q1.enqueue(1);
moodycamel::ConcurrentQueue<int> q2 = std::move(q1); // q1 now empty

// Move assignment
moodycamel::ConcurrentQueue<int> q3;
q3 = std::move(q2); // q2 now empty

// Note: Tokens are NOT transferable between queues
moodycamel::ProducerToken ptok(q1);
// q1 = std::move(q2); // ptok now invalid!
```

### Resource management
```cpp
// Memory is managed internally - no manual cleanup needed
moodycamel::ConcurrentQueue<LargeObject> q;

// But you can control allocation:
// 1. Pre-allocate to avoid runtime allocations
q = moodycamel::ConcurrentQueue<LargeObject>(1000);

// 2. Use try_enqueue to avoid allocation
if (!q.try_enqueue(obj)) {
    // Handle case where no memory available
}

// 3. Tokens are lightweight and should be reused
moodycamel::ProducerToken ptok(q); // Created once
for (int i = 0; i < 1000; ++i) {
    q.enqueue(ptok, i); // Reuses token
}
```

### Exception safety
```cpp
// Queue is exception-safe for noexcept types
struct SafeType {
    SafeType(SafeType&&) noexcept = default;
};

moodycamel::ConcurrentQueue<SafeType> q;
try {
    q.enqueue(SafeType{});
} catch (...) {
    // Queue remains in valid state
}

// For potentially throwing types, be careful
struct MaybeThrowing {
    MaybeThrowing(MaybeThrowing&&) noexcept(false) {
        if (rand() % 2) throw std::runtime_error("bad");
    }
};

moodycamel::ConcurrentQueue<MaybeThrowing> q;
// Enqueue may leave queue in inconsistent state if move throws
```
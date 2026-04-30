# Safety

### Red Line 1: Never use queue before construction is complete
```cpp
// BAD: Using queue from another thread during construction
moodycamel::ConcurrentQueue<int> q;
std::thread t([&]() { q.enqueue(1); }); // May access partially constructed queue
// q not fully constructed yet!
t.join();

// GOOD: Ensure full construction before sharing
moodycamel::ConcurrentQueue<int> q;
// Add memory barrier if needed
std::atomic_thread_fence(std::memory_order_release);
std::thread t([&]() { 
    std::atomic_thread_fence(std::memory_order_acquire);
    q.enqueue(1); 
});
t.join();
```

### Red Line 2: Never destroy queue while threads are using it
```cpp
// BAD: Destruction while threads active
moodycamel::ConcurrentQueue<int> q;
std::thread t([&]() { 
    q.enqueue(1);
    // Thread still running
});
// q goes out of scope here - UNDEFINED BEHAVIOR
t.join();

// GOOD: Ensure all threads finished
moodycamel::ConcurrentQueue<int> q;
std::thread t([&]() { q.enqueue(1); });
t.join(); // Thread done
// Now safe to destroy q
```

### Red Line 3: Never use implicit operations from threads that may exit
```cpp
// BAD: Thread-local producer may be recycled incorrectly
moodycamel::ConcurrentQueue<int> q;
for (int i = 0; i < 100; ++i) {
    std::thread t([&]() { 
        q.enqueue(i); // Implicit - uses thread-local storage
    });
    t.detach(); // Thread may exit, recycling issues
}

// GOOD: Use explicit tokens
moodycamel::ConcurrentQueue<int> q;
for (int i = 0; i < 100; ++i) {
    auto ptok = std::make_shared<moodycamel::ProducerToken>(q);
    std::thread t([&, ptok, i]() { 
        q.enqueue(*ptok, i); // Explicit - no thread-local issues
    });
    t.detach();
}
```

### Red Line 4: Never assume sequential consistency
```cpp
// BAD: Expecting happens-before for non-queue operations
moodycamel::ConcurrentQueue<int> q;
int shared_data = 0;
std::thread t1([&]() { 
    shared_data = 42;
    q.enqueue(1); // May not make shared_data visible to consumer
});
std::thread t2([&]() { 
    int item;
    if (q.try_dequeue(item)) {
        // shared_data may still be 0!
    }
});
t1.join(); t2.join();

// GOOD: Use explicit memory ordering
std::atomic<int> shared_data{0};
std::thread t1([&]() { 
    shared_data.store(42, std::memory_order_release);
    q.enqueue(1);
});
std::thread t2([&]() { 
    int item;
    if (q.try_dequeue(item)) {
        int val = shared_data.load(std::memory_order_acquire);
        // val is guaranteed to be 42
    }
});
t1.join(); t2.join();
```

### Red Line 5: Never use with types that throw in move/copy constructors
```cpp
// BAD: Type that throws
struct BadType {
    BadType() = default;
    BadType(BadType&&) noexcept(false) { throw std::runtime_error("oops"); }
};
moodycamel::ConcurrentQueue<BadType> q;
try {
    q.enqueue(BadType{}); // May leave queue in inconsistent state
} catch (...) {}

// GOOD: Use noexcept move constructors
struct GoodType {
    GoodType() = default;
    GoodType(GoodType&&) noexcept = default;
};
moodycamel::ConcurrentQueue<GoodType> q;
q.enqueue(GoodType{}); // Safe
```
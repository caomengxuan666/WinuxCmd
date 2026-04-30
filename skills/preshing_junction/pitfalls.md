# Pitfalls

```cpp
// BAD: Forgetting to call DefaultQSBR.update()
void workerThread() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    for (int i = 0; i < 1000; ++i) {
        map.assign(i, i * 10);
        // Memory leak! No QSBR update
    }
}

// GOOD: Periodic QSBR updates
void workerThread() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    for (int i = 0; i < 1000; ++i) {
        map.assign(i, i * 10);
        junction::DefaultQSBR.update(); // Prevents memory leaks
    }
}
```

```cpp
// BAD: Using non-pointer-sized keys/values
void badTypes() {
    junction::ConcurrentMap_Leapfrog<int, double> map; // Compile error
    map.assign(42, 3.14);
}

// GOOD: Using pointer-sized types
void goodTypes() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    map.assign(42, 3); // Must be pointer-sized
}
```

```cpp
// BAD: Concurrent assign and iteration
void badConcurrentAccess() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    
    std::thread writer([&]() {
        for (int i = 0; i < 100; ++i) {
            map.assign(i, i * 10);
            junction::DefaultQSBR.update();
        }
    });
    
    std::thread reader([&]() {
        auto iter = map.begin(); // Undefined behavior!
        while (iter != map.end()) {
            ++iter;
        }
    });
    
    writer.join();
    reader.join();
}

// GOOD: Separate access patterns
void goodConcurrentAccess() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    
    // First populate
    for (int i = 0; i < 100; ++i) {
        map.assign(i, i * 10);
        junction::DefaultQSBR.update();
    }
    
    // Then iterate (read-only, no concurrent writes)
    auto iter = map.begin();
    while (iter != map.end()) {
        ++iter;
    }
}
```

```cpp
// BAD: Using null key or value
void badNullValues() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    map.assign(0, 100); // Key 0 is null key - undefined behavior
    map.assign(1, 0);   // Value 0 is null value - undefined behavior
}

// GOOD: Avoiding reserved values
void goodValues() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    map.assign(100, 200); // Safe - not using null/redirect values
    map.assign(101, 201);
}
```

```cpp
// BAD: Using non-invertible hash function
struct BadHash {
    static uintptr_t hash(uintptr_t key) { 
        return key % 10; // Multiple keys map to same hash
    }
};

void badHash() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t, BadHash> map;
    map.assign(10, 100); // hash(10) = 0
    map.assign(20, 200); // hash(20) = 0 - collision!
}

// GOOD: Invertible hash function
struct GoodHash {
    static uintptr_t hash(uintptr_t key) { 
        return key; // Each key has unique hash
    }
};

void goodHash() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t, GoodHash> map;
    map.assign(10, 100);
    map.assign(20, 200); // Different hashes, no collision
}
```

```cpp
// BAD: Assuming immediate visibility across threads
void badVisibility() {
    junction::ConcurrentMap_Crude<uintptr_t, uintptr_t> map;
    int sharedData = 0;
    
    std::thread writer([&]() {
        sharedData = 42;
        map.assign(1, &sharedData);
    });
    
    std::thread reader([&]() {
        void* ptr = map.get(1);
        // Crude map uses relaxed semantics - may not see sharedData update
        if (ptr) {
            int value = *(int*)ptr; // May read stale data
        }
    });
    
    writer.join();
    reader.join();
}

// GOOD: Using proper memory ordering
void goodVisibility() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    int sharedData = 0;
    
    std::thread writer([&]() {
        sharedData = 42;
        map.assign(1, &sharedData); // Release semantics
    });
    
    std::thread reader([&]() {
        void* ptr = map.get(1); // Consume semantics
        if (ptr) {
            int value = *(int*)ptr; // Properly synchronized
        }
    });
    
    writer.join();
    reader.join();
}
```
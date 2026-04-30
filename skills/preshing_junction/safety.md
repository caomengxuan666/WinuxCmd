# Safety

```cpp
// RED LINE 1: NEVER forget to call DefaultQSBR.update()
// BAD: Memory leak guaranteed
void unsafeThread() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    while (true) {
        map.assign(1, 100);
        // No QSBR update - memory grows unbounded
    }
}

// GOOD: Always call update periodically
void safeThread() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    while (true) {
        map.assign(1, 100);
        junction::DefaultQSBR.update(); // Required for memory reclamation
    }
}
```

```cpp
// RED LINE 2: NEVER use null or redirect values as actual data
// BAD: Using reserved values
void unsafeValues() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    map.assign(100, 0); // 0 is null value - undefined behavior
    map.assign(200, 1); // 1 is redirect value - undefined behavior
}

// GOOD: Avoid reserved values
void safeValues() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    map.assign(100, 2); // Safe - not using reserved values
    map.assign(200, 3);
}
```

```cpp
// RED LINE 3: NEVER assign while iterating
// BAD: Concurrent modification during iteration
void unsafeIteration() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    auto iter = map.begin();
    map.assign(1, 100); // Undefined behavior!
    while (iter != map.end()) {
        ++iter;
    }
}

// GOOD: Separate iteration from modification
void safeIteration() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    // Complete all modifications first
    map.assign(1, 100);
    junction::DefaultQSBR.update();
    
    // Then iterate
    auto iter = map.begin();
    while (iter != map.end()) {
        ++iter;
    }
}
```

```cpp
// RED LINE 4: NEVER use non-pointer-sized types
// BAD: Using incompatible types
void unsafeTypes() {
    junction::ConcurrentMap_Leapfrog<std::string, int> map; // Compile error
    junction::ConcurrentMap_Leapfrog<double, float> map2;   // Compile error
}

// GOOD: Use pointer-sized types only
void safeTypes() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    junction::ConcurrentMap_Leapfrog<void*, void*> map2;
}
```

```cpp
// RED LINE 5: NEVER use non-invertible hash functions
// BAD: Hash collisions cause undefined behavior
struct CollisionHash {
    static uintptr_t hash(uintptr_t key) { return key % 100; }
};

void unsafeHash() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t, CollisionHash> map;
    map.assign(100, 1); // hash = 0
    map.assign(200, 2); // hash = 0 - collision!
}

// GOOD: Invertible hash function
struct IdentityHash {
    static uintptr_t hash(uintptr_t key) { return key; }
};

void safeHash() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t, IdentityHash> map;
    map.assign(100, 1);
    map.assign(200, 2); // Different hashes
}
```
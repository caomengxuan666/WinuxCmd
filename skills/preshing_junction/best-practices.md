# Best Practices

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>
#include <atomic>
#include <thread>

// Best Practice 1: Use RAII wrapper for QSBR
class QSBRGuard {
public:
    QSBRGuard() = default;
    ~QSBRGuard() { junction::DefaultQSBR.update(); }
    
    void update() { junction::DefaultQSBR.update(); }
};

void raiiPattern() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    QSBRGuard guard;
    for (int i = 0; i < 100; ++i) {
        map.assign(i, i * 10);
        guard.update(); // Safe even if exception occurs
    }
}
```

```cpp
// Best Practice 2: Choose the right map implementation
void chooseMapType() {
    // Crude: Fastest for single-threaded or low-contention
    junction::ConcurrentMap_Crude<uintptr_t, uintptr_t> crudeMap;
    
    // Linear: Good for read-heavy workloads
    junction::ConcurrentMap_Linear<uintptr_t, uintptr_t> linearMap;
    
    // Leapfrog: Best all-around performance
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> leapfrogMap;
    
    // Grampa: Best for write-heavy workloads
    junction::ConcurrentMap_Grampa<uintptr_t, uintptr_t> grampaMap;
}
```

```cpp
// Best Practice 3: Batch operations with Mutator
void batchOperations() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    // Use Mutator for multiple related operations
    auto mutator = map.makeMutator();
    
    // Batch insert
    for (uintptr_t i = 0; i < 1000; ++i) {
        mutator.assign(i, i * 10);
    }
    
    // Batch read
    for (uintptr_t i = 0; i < 1000; ++i) {
        uintptr_t val = mutator.get(i);
        // Process val
    }
}
```

```cpp
// Best Practice 4: Proper error handling for missing keys
void safeKeyAccess() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    map.assign(42, 100);
    
    // Check for null value (key not found)
    uintptr_t value = map.get(42);
    if (value != 0 && value != 1) { // Not null or redirect
        // Key exists
    }
    
    // For pointer values
    void* ptrValue = map.get(42);
    if (ptrValue != nullptr && ptrValue != (void*)1) {
        // Key exists
    }
}
```

```cpp
// Best Practice 5: Thread-local QSBR management
thread_local junction::DefaultQSBR::Context qsbrContext;

void threadWorker() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    for (int i = 0; i < 100; ++i) {
        map.assign(i, i * 10);
        qsbrContext.update(); // Thread-local update
    }
}

void spawnWorkers() {
    std::thread t1(threadWorker);
    std::thread t2(threadWorker);
    t1.join();
    t2.join();
}
```
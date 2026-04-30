# Threading

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>
#include <thread>
#include <vector>

// Thread safety - Basic concurrent access
void basicConcurrentAccess() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    
    std::thread writer([&]() {
        for (uintptr_t i = 0; i < 1000; ++i) {
            map.assign(i, i * 10);
            junction::DefaultQSBR.update();
        }
    });
    
    std::thread reader([&]() {
        for (uintptr_t i = 0; i < 1000; ++i) {
            uintptr_t val = map.get(i);
            junction::DefaultQSBR.update();
        }
    });
    
    writer.join();
    reader.join();
}
```

```cpp
// Thread safety - Multiple concurrent writers
void concurrentWriters() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    const int NUM_THREADS = 4;
    const int ITEMS_PER_THREAD = 1000;
    
    std::vector<std::thread> threads;
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&map, t]() {
            for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
                uintptr_t key = t * ITEMS_PER_THREAD + i;
                map.assign(key, key * 10);
                junction::DefaultQSBR.update();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}
```

```cpp
// Thread safety - Memory ordering guarantees
void memoryOrdering() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    std::atomic<int> sharedData{0};
    
    // Writer thread
    std::thread writer([&]() {
        sharedData.store(42, std::memory_order_release);
        map.assign(1, &sharedData); // Release semantics
        junction::DefaultQSBR.update();
    });
    
    // Reader thread
    std::thread reader([&]() {
        void* ptr = map.get(1); // Consume semantics
        if (ptr) {
            std::atomic<int>* data = static_cast<std::atomic<int>*>(ptr);
            int value = data->load(std::memory_order_consume);
            // value is guaranteed to be 42 due to release-consume ordering
        }
        junction::DefaultQSBR.update();
    });
    
    writer.join();
    reader.join();
}
```

```cpp
// Thread safety - QSBR requirements per thread
void qsbrPerThread() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    
    // Each thread must call QSBR.update() periodically
    auto worker = [&map](int id) {
        for (int i = 0; i < 100; ++i) {
            map.assign(id * 100 + i, i);
            junction::DefaultQSBR.update(); // Required for this thread
        }
    };
    
    std::thread t1(worker, 0);
    std::thread t2(worker, 1);
    
    t1.join();
    t2.join();
}
```

```cpp
// Thread safety - Concurrent read-only access
void concurrentReadOnly() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    
    // Pre-populate
    for (uintptr_t i = 0; i < 10000; ++i) {
        map.assign(i, i * 10);
    }
    
    // Multiple readers can safely access concurrently
    auto reader = [&map]() {
        for (int i = 0; i < 1000; ++i) {
            volatile uintptr_t val = map.get(i);
            junction::DefaultQSBR.update();
        }
    };
    
    std::thread r1(reader);
    std::thread r2(reader);
    std::thread r3(reader);
    
    r1.join();
    r2.join();
    r3.join();
}
```
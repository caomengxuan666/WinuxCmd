# Quickstart

```cpp
#include "concurrentqueue.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>

// Pattern 1: Basic enqueue/dequeue
void basic_usage() {
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(42);
    
    int item;
    bool found = q.try_dequeue(item);
    std::cout << "Dequeued: " << item << ", found: " << found << "\n";
}

// Pattern 2: Using producer and consumer tokens (faster)
void token_usage() {
    moodycamel::ConcurrentQueue<int> q;
    
    moodycamel::ProducerToken ptok(q);
    moodycamel::ConsumerToken ctok(q);
    
    q.enqueue(ptok, 100);
    q.enqueue(ptok, 200);
    
    int item;
    while (q.try_dequeue(ctok, item)) {
        std::cout << "Dequeued: " << item << "\n";
    }
}

// Pattern 3: Bulk operations
void bulk_operations() {
    moodycamel::ConcurrentQueue<int> q;
    std::vector<int> items = {1, 2, 3, 4, 5};
    
    // Bulk enqueue
    q.enqueue_bulk(items.begin(), items.size());
    
    // Bulk dequeue
    std::vector<int> result(10);
    size_t count = q.try_dequeue_bulk(result.begin(), result.size());
    result.resize(count);
    
    std::cout << "Dequeued " << count << " items\n";
}

// Pattern 4: Multi-threaded producer-consumer
void multi_threaded() {
    moodycamel::ConcurrentQueue<int> q;
    const int num_items = 1000;
    
    std::thread producer([&]() {
        moodycamel::ProducerToken ptok(q);
        for (int i = 0; i < num_items; ++i) {
            q.enqueue(ptok, i);
        }
    });
    
    std::thread consumer([&]() {
        moodycamel::ConsumerToken ctok(q);
        int item;
        int count = 0;
        while (count < num_items) {
            if (q.try_dequeue(ctok, item)) {
                ++count;
            }
        }
    });
    
    producer.join();
    consumer.join();
}

// Pattern 5: Blocking version
#include "blockingconcurrentqueue.h"

void blocking_usage() {
    moodycamel::BlockingConcurrentQueue<int> q;
    
    std::thread producer([&]() {
        q.enqueue(42);
    });
    
    int item;
    q.wait_dequeue(item);  // Blocks until item available
    std::cout << "Blocking dequeue: " << item << "\n";
    
    producer.join();
}

// Pattern 6: Pre-allocated queue
void preallocated_queue() {
    // Pre-allocate for 10000 elements
    moodycamel::ConcurrentQueue<int> q(10000);
    
    // Will not allocate during enqueue (faster)
    for (int i = 0; i < 10000; ++i) {
        q.enqueue(i);
    }
}

// Pattern 7: Try operations (non-blocking)
void try_operations() {
    moodycamel::ConcurrentQueue<int> q;
    
    // try_enqueue - only if memory already allocated
    q.try_enqueue(1);  // May fail if no space
    
    // try_dequeue - non-blocking
    int item;
    if (q.try_dequeue(item)) {
        // Item was available
    }
}

// Pattern 8: Size estimation
void size_estimation() {
    moodycamel::ConcurrentQueue<int> q;
    q.enqueue(1);
    q.enqueue(2);
    
    // size_approx() is eventually consistent
    size_t approx_size = q.size_approx();
    std::cout << "Approximate size: " << approx_size << "\n";
}
```
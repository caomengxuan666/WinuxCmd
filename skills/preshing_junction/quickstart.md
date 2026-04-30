# Quickstart

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>
#include <iostream>

// Basic usage pattern with pointer keys and values
void basicUsage() {
    using Map = junction::ConcurrentMap_Leapfrog<void*, void*>;
    Map map;
    
    // Insert key-value pairs
    int key1 = 42, value1 = 100;
    map.assign(&key1, &value1);
    
    // Retrieve values
    void* result = map.get(&key1);
    std::cout << "Value: " << *(int*)result << std::endl;
    
    // Update existing key
    int value2 = 200;
    map.assign(&key1, &value2);
    
    // Remove key
    map.erase(&key1);
}
```

```cpp
#include <junction/ConcurrentMap_Linear.h>
#include <junction/DefaultQSBR.h>

// Using integer keys and values (pointer-sized)
void integerKeysAndValues() {
    using Map = junction::ConcurrentMap_Linear<uintptr_t, uintptr_t>;
    Map map;
    
    // Keys and values must be pointer-sized integers
    uintptr_t key = 1000;
    uintptr_t value = 2000;
    
    map.assign(key, value);
    uintptr_t result = map.get(key);
    
    // Check for null/redirect values
    if (result != 0 && result != 1) {
        std::cout << "Found value: " << result << std::endl;
    }
}
```

```cpp
#include <junction/ConcurrentMap_Grampa.h>
#include <junction/DefaultQSBR.h>

// Using custom key/value traits
struct MyKeyTraits {
    static const uintptr_t nullKey = 0xFFFFFFFF;
    static uintptr_t hash(uintptr_t key) { return key; }
};

void customTraits() {
    using Map = junction::ConcurrentMap_Grampa<uintptr_t, uintptr_t, 
                                               MyKeyTraits>;
    Map map;
    
    map.assign(100, 200);
    uintptr_t result = map.get(100);
}
```

```cpp
#include <junction/ConcurrentMap_Crude.h>
#include <junction/DefaultQSBR.h>

// Using Mutator for batch operations
void batchOperations() {
    using Map = junction::ConcurrentMap_Crude<uintptr_t, uintptr_t>;
    Map map;
    
    // Create a mutator for multiple operations
    auto mutator = map.makeMutator();
    
    mutator.assign(1, 10);
    mutator.assign(2, 20);
    mutator.assign(3, 30);
    
    uintptr_t val = mutator.get(2);
    mutator.erase(1);
}
```

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>

// Thread-safe iteration (read-only)
void readOnlyIteration() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    map.assign(1, 100);
    map.assign(2, 200);
    
    // Create iterator for read-only traversal
    auto iter = map.begin();
    while (iter != map.end()) {
        uintptr_t key = iter->key;
        uintptr_t value = iter->value;
        ++iter;
    }
}
```

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>

// Periodic QSBR update (required for all threads)
void threadFunction() {
    using Map = junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t>;
    Map map;
    
    for (int i = 0; i < 100; ++i) {
        map.assign(i, i * 10);
        
        // Must call periodically to prevent memory leaks
        junction::DefaultQSBR.update();
    }
}
```
# Lifecycle

```cpp
#include <junction/ConcurrentMap_Leapfrog.h>
#include <junction/DefaultQSBR.h>

// Construction - Default constructor
void construction() {
    // Default construction
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map1;
    
    // Construction with initial capacity hint
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map2(1000);
    
    // Construction with custom traits
    struct CustomTraits {
        static const uintptr_t nullKey = 0xFFFFFFFF;
        static uintptr_t hash(uintptr_t key) { return key; }
    };
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t, CustomTraits> map3;
}
```

```cpp
// Destruction - Automatic cleanup
void destruction() {
    {
        junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
        map.assign(1, 100);
        map.assign(2, 200);
        
        // Map is destroyed when going out of scope
        // All memory is reclaimed
    } // map destroyed here
    
    // Ensure QSBR is called after destruction
    junction::DefaultQSBR.update();
}
```

```cpp
// Resource management - Clearing and resizing
void resourceManagement() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map;
    
    // Populate map
    for (uintptr_t i = 0; i < 1000; ++i) {
        map.assign(i, i * 10);
    }
    
    // Clear all entries
    for (uintptr_t i = 0; i < 1000; ++i) {
        map.erase(i);
    }
    
    // Map is now empty but memory may still be allocated
    junction::DefaultQSBR.update(); // Reclaim memory
}
```

```cpp
// Move semantics
void moveSemantics() {
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map1;
    map1.assign(1, 100);
    map1.assign(2, 200);
    
    // Move construction
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map2(std::move(map1));
    
    // map1 is now empty, map2 has the data
    uintptr_t val = map2.get(1); // Returns 100
    
    // Move assignment
    junction::ConcurrentMap_Leapfrog<uintptr_t, uintptr_t> map3;
    map3 = std::move(map2);
    
    // map2 is now empty, map3 has the data
    val = map3.get(1); // Returns 100
}
```
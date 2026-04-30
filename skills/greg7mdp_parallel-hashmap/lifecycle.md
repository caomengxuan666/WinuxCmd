# Lifecycle


**Construction and initialization**

```cpp
#include <parallel_hashmap/phmap.h>
#include <string>

// Default construction
phmap::flat_hash_map<int, std::string> empty_map;

// Initializer list construction
phmap::flat_hash_map<int, std::string> map = {
    {1, "one"},
    {2, "two"},
    {3, "three"}
};

// Copy construction
auto copy = map;

// Move construction
auto moved = std::move(map);  // map is now empty

// Construction with reserved capacity
phmap::flat_hash_map<int, std::string> reserved(1000);
```

**Move semantics and resource management**

```cpp
#include <parallel_hashmap/phmap.h>
#include <memory>

class ResourceManager {
    phmap::flat_hash_map<int, std::unique_ptr<Resource>> resources;
    
public:
    // Move assignment
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        resources = std::move(other.resources);
        return *this;
    }
    
    // Insert with move semantics
    void add_resource(int id, std::unique_ptr<Resource> res) {
        resources.try_emplace(id, std::move(res));
    }
    
    // Extract and move out
    std::unique_ptr<Resource> extract(int id) {
        auto node = resources.extract(id);
        if (!node.empty()) {
            return std::move(node.mapped());
        }
        return nullptr;
    }
};
```

**Destruction and cleanup**

```cpp
#include <parallel_hashmap/phmap.h>

void lifecycle_example() {
    // Automatic cleanup on scope exit
    {
        phmap::flat_hash_map<int, std::string> temp;
        temp[1] = "temporary";
        // temp is destroyed here, all memory freed
    }
    
    // Manual clearing
    phmap::flat_hash_map<int, int> map;
    map[1] = 10;
    map[2] = 20;
    map.clear();  // Removes all elements, capacity may be preserved
    
    // Release memory completely
    map = {};  // Assign empty map, frees all memory
}
```

**Swap and merge operations**

```cpp
#include <parallel_hashmap/phmap.h>
#include <iostream>

void swap_merge_example() {
    phmap::flat_hash_map<int, std::string> map1 = {{1, "a"}, {2, "b"}};
    phmap::flat_hash_map<int, std::string> map2 = {{3, "c"}, {4, "d"}};
    
    // Swap contents efficiently
    map1.swap(map2);
    std::cout << "After swap, map1 has " << map1.size() << " elements\n";
    
    // Merge from another map
    phmap::flat_hash_map<int, std::string> source = {{5, "e"}, {6, "f"}};
    map1.merge(source);  // Moves elements from source to map1
    std::cout << "After merge, map1 has " << map1.size() << " elements\n";
    std::cout << "Source now has " << source.size() << " elements\n";
}
```
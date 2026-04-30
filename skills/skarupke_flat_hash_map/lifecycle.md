# Lifecycle

### Construction and Initialization
```cpp
#include <flat_hash_map.hpp>
#include <string>

// Default construction
ska::flat_hash_map<int, std::string> map1;

// Initializer list construction
ska::flat_hash_map<int, std::string> map2 = {{1, "a"}, {2, "b"}};

// Range construction
std::vector<std::pair<int, std::string>> vec = {{1, "a"}, {2, "b"}};
ska::flat_hash_map<int, std::string> map3(vec.begin(), vec.end());

// Copy construction
ska::flat_hash_map<int, std::string> map4(map2);

// Move construction
ska::flat_hash_map<int, std::string> map5(std::move(map3));
```

### Destruction and Resource Management
```cpp
// Automatic destruction - no special cleanup needed
{
    ska::flat_hash_map<int, std::string> map;
    map[1] = "hello";
    // map destroyed here, all memory freed
}

// Manual clearing
ska::flat_hash_map<int, int> map;
map[1] = 10;
map.clear();  // Removes all elements, keeps capacity
map.shrink_to_fit();  // Releases unused memory
```

### Move Semantics
```cpp
ska::flat_hash_map<int, std::string> create_map() {
    ska::flat_hash_map<int, std::string> map;
    map[1] = "hello";
    map[2] = "world";
    return map;  // Move or RVO
}

void move_example() {
    ska::flat_hash_map<int, std::string> source = {{1, "a"}, {2, "b"}};
    
    // Move assignment
    ska::flat_hash_map<int, std::string> dest;
    dest = std::move(source);  // source is now empty
    
    // After move, source is in valid but unspecified state
    std::cout << source.size();  // 0 (typically)
    source[3] = "c";  // Safe to reuse
}
```

### Resource Management with Custom Types
```cpp
struct Resource {
    int* data;
    Resource() : data(new int(0)) {}
    ~Resource() { delete data; }
    Resource(Resource&& other) noexcept : data(other.data) { 
        other.data = nullptr; 
    }
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }
};

void resource_management() {
    ska::flat_hash_map<int, Resource> map;
    map[1] = Resource();  // Move semantics handle cleanup
    
    // When map is destroyed, all Resource destructors run
    // When rehash occurs, elements are moved (not copied)
}
```
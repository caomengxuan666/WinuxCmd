# Best Practices

### 1. Reserve Space for Known Sizes
```cpp
// Best practice: Reserve upfront for performance
ska::flat_hash_map<int, std::string> map;
size_t expected_size = 100000;
map.reserve(expected_size);
for (int i = 0; i < expected_size; ++i) {
    map[i] = "value_" + std::to_string(i);
}
```

### 2. Use try_emplace for Efficient Insertion
```cpp
// Best practice: try_emplace avoids unnecessary copies
ska::flat_hash_map<int, std::string> map;
auto [it, inserted] = map.try_emplace(1, "hello");
if (inserted) {
    // Successfully inserted
} else {
    // Key already exists, it points to existing element
}
```

### 3. Prefer find() Over operator[] for Lookups
```cpp
// Best practice: Use find() for read-only access
ska::flat_hash_map<int, int> map = {{1, 10}, {2, 20}};

auto it = map.find(1);
if (it != map.end()) {
    process(it->second);  // No side effects
}

// Avoid: operator[] inserts if key missing
if (map.contains(1)) {
    process(map[1]);  // Safe but less efficient
}
```

### 4. Use Heterogeneous Lookup When Possible
```cpp
// Best practice: Heterogeneous lookup avoids temporary strings
ska::flat_hash_map<std::string, int, ska::transparent_hash> map;
map["hello"] = 1;

// Lookup with string_view avoids constructing std::string
std::string_view sv = "hello";
auto it = map.find(sv);  // No temporary std::string created
```

### 5. Batch Erase for Multiple Keys
```cpp
// Best practice: Batch erase is more efficient
ska::flat_hash_map<int, int> map;
// ... populate map ...

// Efficient batch erase
std::vector<int> keys_to_remove = {1, 2, 3};
for (int key : keys_to_remove) {
    map.erase(key);
}

// Alternative: Erase while iterating (careful with invalidation)
for (auto it = map.begin(); it != map.end(); ) {
    if (should_remove(it->first)) {
        it = map.erase(it);
    } else {
        ++it;
    }
}
```

### 6. Use Custom Allocators for Memory Control
```cpp
// Best practice: Custom allocator for memory management
template<typename T>
using ArenaAllocator = std::allocator<T>;  // Replace with your allocator

using CustomMap = ska::flat_hash_map<
    int, 
    std::string,
    std::hash<int>,
    std::equal_to<int>,
    ArenaAllocator<std::pair<const int, std::string>>
>;

CustomMap map;
map.reserve(1000);  // Allocates from your arena
```
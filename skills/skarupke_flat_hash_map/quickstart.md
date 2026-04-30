# Quickstart

```cpp
#include <flat_hash_map.hpp>
#include <string>
#include <iostream>

// 1. Basic creation and insertion
void basic_usage() {
    ska::flat_hash_map<int, std::string> map;
    map[1] = "one";
    map[2] = "two";
    map.insert({3, "three"});
}

// 2. Lookup operations
void lookup_examples() {
    ska::flat_hash_map<int, std::string> map = {{1, "a"}, {2, "b"}};
    
    auto it = map.find(1);
    if (it != map.end()) {
        std::cout << it->second << '\n';
    }
    
    if (map.contains(2)) {
        std::cout << map.at(2) << '\n';
    }
}

// 3. Iteration
void iteration_example() {
    ska::flat_hash_map<int, std::string> map = {{1, "a"}, {2, "b"}, {3, "c"}};
    
    for (const auto& [key, value] : map) {
        std::cout << key << ": " << value << '\n';
    }
}

// 4. Erasing elements
void erase_example() {
    ska::flat_hash_map<int, std::string> map = {{1, "a"}, {2, "b"}, {3, "c"}};
    
    map.erase(2);                    // Erase by key
    auto it = map.find(1);
    if (it != map.end()) {
        map.erase(it);               // Erase by iterator
    }
}

// 5. Bulk operations
void bulk_operations() {
    ska::flat_hash_map<int, std::string> map;
    
    // Reserve space for efficiency
    map.reserve(1000);
    
    // Batch insert
    for (int i = 0; i < 100; ++i) {
        map[i] = "value_" + std::to_string(i);
    }
}

// 6. Custom types as keys
struct CustomKey {
    int id;
    std::string name;
    
    bool operator==(const CustomKey& other) const {
        return id == other.id && name == other.name;
    }
};

struct CustomKeyHash {
    std::size_t operator()(const CustomKey& key) const {
        return std::hash<int>{}(key.id) ^ std::hash<std::string>{}(key.name);
    }
};

void custom_key_example() {
    ska::flat_hash_map<CustomKey, double, CustomKeyHash> map;
    map[{1, "test"}] = 3.14;
}
```
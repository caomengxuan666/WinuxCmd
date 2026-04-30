# Quickstart


```cpp
#include <parallel_hashmap/phmap.h>
#include <iostream>
#include <string>

// Basic flat_hash_map usage
void basic_example() {
    phmap::flat_hash_map<std::string, int> ages = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };
    
    // Insert and access
    ages["David"] = 28;
    std::cout << "Alice's age: " << ages["Alice"] << "\n";
}

// Using try_emplace (C++17 style)
void try_emplace_example() {
    phmap::flat_hash_map<int, std::string> map;
    auto [it, inserted] = map.try_emplace(42, "answer");
    if (inserted) {
        std::cout << "Inserted: " << it->second << "\n";
    }
}

// Heterogeneous lookup with string_view
void heterogeneous_lookup() {
    phmap::flat_hash_map<std::string, int> scores = {
        {"Alice", 95},
        {"Bob", 87}
    };
    
    std::string_view name = "Alice";
    auto it = scores.find(name);  // No string copy needed
    if (it != scores.end()) {
        std::cout << it->first << ": " << it->second << "\n";
    }
}

// Parallel flat hash map for concurrent access
#include <thread>
#include <vector>

void parallel_example() {
    phmap::parallel_flat_hash_map<int, int> parallel_map;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&parallel_map, i]() {
            for (int j = 0; j < 1000; ++j) {
                parallel_map.try_emplace(i * 1000 + j, j);
            }
        });
    }
    
    for (auto& t : threads) t.join();
    std::cout << "Size: " << parallel_map.size() << "\n";
}

// Btree map as ordered alternative
#include <parallel_hashmap/btree.h>

void btree_example() {
    phmap::btree_map<int, std::string> ordered_map;
    ordered_map[3] = "three";
    ordered_map[1] = "one";
    ordered_map[2] = "two";
    
    for (const auto& [key, value] : ordered_map) {
        std::cout << key << ": " << value << "\n";  // Prints in order
    }
}

// Serialization for trivially copyable types
#include <fstream>

void serialize_example() {
    phmap::flat_hash_map<int, double> data = {{1, 1.1}, {2, 2.2}, {3, 3.3}};
    
    // Save to file
    {
        std::ofstream file("data.bin", std::ios::binary);
        data.serialize(file);
    }
    
    // Load from file
    phmap::flat_hash_map<int, double> loaded;
    {
        std::ifstream file("data.bin", std::ios::binary);
        loaded.load(file);
    }
}

// Reserve space for performance
void reserve_example() {
    phmap::flat_hash_map<int, int> map;
    map.reserve(10000);  // Pre-allocate for 10000 elements
    
    for (int i = 0; i < 10000; ++i) {
        map[i] = i * 2;
    }
}
```
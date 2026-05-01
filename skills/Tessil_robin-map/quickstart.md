# Quickstart

```cpp
#include <tsl/robin_map.h>
#include <tsl/robin_set.h>
#include <iostream>
#include <string>

// Basic usage patterns for tsl::robin_map and tsl::robin_set

// Pattern 1: Construction and insertion
void basic_usage() {
    tsl::robin_map<int, std::string> map;
    map.insert({1, "one"});
    map[2] = "two";
    map.emplace(3, "three");
    
    tsl::robin_set<int> set;
    set.insert(1);
    set.insert(2);
}

// Pattern 2: Lookup with find
void lookup_example() {
    tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
    
    auto it = map.find(1);
    if (it != map.end()) {
        std::cout << it->second << std::endl;  // Prints "one"
        it.value() = "ONE";  // Modify value through iterator
    }
}

// Pattern 3: Iteration with value modification
void iteration_example() {
    tsl::robin_map<int, int> map = {{1, 1}, {2, 2}, {3, 3}};
    
    for (auto it = map.begin(); it != map.end(); ++it) {
        // it->second = 2; // Illegal! Use value() instead
        it.value() = it.value() * 2;
    }
}

// Pattern 4: Heterogeneous lookup (C++20)
void heterogeneous_lookup() {
    tsl::robin_map<std::string, int> map = {{"hello", 1}, {"world", 2}};
    
    // Find using std::string_view without constructing std::string
    std::string_view sv = "hello";
    auto it = map.find(sv);
    if (it != map.end()) {
        std::cout << it->second << std::endl;
    }
}

// Pattern 5: Serialization
void serialization_example() {
    tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
    
    // Serialize to string
    std::string serialized = map.serialize([](const std::pair<int, std::string>& value) {
        return std::to_string(value.first) + ":" + value.second;
    });
    
    // Deserialize back
    tsl::robin_map<int, std::string> deserialized;
    deserialized.deserialize(serialized, [](const std::string& str) {
        auto pos = str.find(':');
        return std::make_pair(std::stoi(str.substr(0, pos)), str.substr(pos + 1));
    });
}

// Pattern 6: Pre-calculated hash
void precalculated_hash_example() {
    tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
    
    std::size_t hash = map.hash_function()(1);
    auto it = map.find(1, hash);
    if (it != map.end()) {
        std::cout << it->second << std::endl;
    }
}

// Pattern 7: Prime growth policy for better hash distribution
void prime_growth_example() {
    tsl::robin_pg_map<int, std::string> map;  // Uses prime growth policy
    map[1] = "one";
    map[2] = "two";
}

// Pattern 8: Move-only types
void move_only_example() {
    tsl::robin_map<int, std::unique_ptr<int>> map;
    map.insert({1, std::make_unique<int>(42)});
    
    auto it = map.find(1);
    if (it != map.end()) {
        std::cout << *it->second << std::endl;  // Prints 42
    }
}
```
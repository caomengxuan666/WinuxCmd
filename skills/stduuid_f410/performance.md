# Performance

```cpp
// Performance: UUID generation is relatively expensive
#include <uuid.h>
#include <chrono>
#include <iostream>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        uuids::uuid id = uuids::uuid_system_generator{}();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Generated 10000 UUIDs in " << duration.count() << "ms" << std::endl;
    return 0;
}
```

```cpp
// Performance: Pre-allocate storage for UUIDs
#include <uuid.h>
#include <vector>

// BAD: Repeated reallocation
std::vector<uuids::uuid> generate_bad(int count) {
    std::vector<uuids::uuid> ids;
    for (int i = 0; i < count; ++i) {
        ids.push_back(uuids::uuid_system_generator{}());
    }
    return ids;
}

// GOOD: Reserve space upfront
std::vector<uuids::uuid> generate_good(int count) {
    std::vector<uuids::uuid> ids;
    ids.reserve(count);
    for (int i = 0; i < count; ++i) {
        ids.push_back(uuids::uuid_system_generator{}());
    }
    return ids;
}
```

```cpp
// Performance: Use byte comparison instead of string comparison
#include <uuid.h>
#include <unordered_set>
#include <chrono>

int main() {
    std::unordered_set<uuids::uuid> uuid_set;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100000; ++i) {
        uuid_set.insert(uuids::uuid_system_generator{}());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Inserted 100000 UUIDs in " << duration.count() << "ms" << std::endl;
    return 0;
}
```

```cpp
// Performance: Avoid unnecessary string conversions
#include <uuid.h>
#include <iostream>

// BAD: Convert to string for every operation
void process_bad(const uuids::uuid& id) {
    std::string str = uuids::to_string(id);
    std::cout << str << std::endl;
    // More string operations...
}

// GOOD: Keep as UUID until output is needed
void process_good(const uuids::uuid& id) {
    // Work with UUID directly
    if (!id.is_nil()) {
        // Process...
    }
    // Only convert to string when necessary
    std::cout << uuids::to_string(id) << std::endl;
}
```
# Performance

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Performance characteristics:
// - Parsing: O(n) where n is input size
// - Access: O(1) average for objects, O(1) for arrays
// - Serialization: O(n) where n is JSON size
// - Memory: ~32 bytes per value + actual data

// 1. Pre-allocate when you know size
json arr;
arr.reserve(10000);  // Avoids reallocations
for (int i = 0; i < 10000; ++i) {
    arr.emplace_back(i);
}

// 2. Use raw string literals for compile-time JSON
const json config = R"({
    "host": "localhost",
    "port": 8080
})"_json;  // Parsed at compile time!

// 3. Avoid deep copies with references
void process_large_json(const json& j) {  // Pass by const reference
    for (const auto& [key, val] : j.items()) {
        // Process without copying
    }
}

// 4. Use SAX parsing for streaming large files
// (avoids building entire DOM)
std::ifstream file("huge.json");
json::sax_parse(file, my_handler);

// 5. Batch operations instead of individual access
// BAD:
for (int i = 0; i < 1000; ++i) {
    arr[i] = i;  // Repeated bounds checking
}

// GOOD:
json new_arr = json::array();
new_arr.reserve(1000);
for (int i = 0; i < 1000; ++i) {
    new_arr.emplace_back(i);
}
arr.swap(new_arr);

// 6. Use flattened access for deep structures
json nested = {{"a", {{"b", {{"c", 42}}}}}};
// BAD:
int val1 = nested["a"]["b"]["c"];  // Multiple lookups
// GOOD:
int val2 = nested["/a/b/c"_json_pointer];  // Single lookup

// 7. Consider binary formats for performance-critical paths
json j = {{"data", std::vector<int>(100000, 42)}};
std::vector<uint8_t> cbor = json::to_cbor(j);  // More compact than JSON
```
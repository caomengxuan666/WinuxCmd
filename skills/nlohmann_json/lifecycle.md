# Lifecycle

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Construction
json j1;                    // Default: null
json j2 = nullptr;          // Explicit null
json j3 = true;             // Boolean
json j4 = 42;               // Integer
json j5 = 3.14;             // Float
json j6 = "hello";          // String
json j7 = {1, 2, 3};        // Array
json j8 = {{"a", 1}};       // Object
json j9 = json::array();    // Empty array
json j10 = json::object();  // Empty object

// Copy construction
json copy(j8);

// Move construction
json moved(std::move(j8));  // j8 is now null

// Assignment
json j11;
j11 = j7;                   // Copy assignment
j11 = std::move(j7);        // Move assignment

// Resource management - automatic cleanup
{
    json temp = json::parse(large_json_string);
    // temp is automatically destroyed at end of scope
}

// Swap
json a = {1, 2, 3};
json b = {4, 5, 6};
a.swap(b);  // Efficient O(1) swap

// Emplace - construct in-place
json obj = json::object();
obj.emplace("key", "value");

// Clear
j1.clear();  // Resets to null

// Memory management hints
json large_array;
large_array.reserve(1000);  // Pre-allocate for known size

// Destruction - automatic, no manual cleanup needed
// json uses RAII - destructor handles all cleanup
```
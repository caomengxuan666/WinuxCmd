# Quickstart

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// 1. Parse JSON from string
json j1 = json::parse(R"({"name": "John", "age": 30})");

// 2. Create JSON objects programmatically
json j2;
j2["name"] = "Jane";
j2["age"] = 25;
j2["hobbies"] = {"reading", "coding"};

// 3. Read JSON from file
std::ifstream f("data.json");
json j3 = json::parse(f);

// 4. Serialize to string
std::string serialized = j2.dump(4); // pretty-print with 4 spaces

// 5. Access values with type checking
if (j2.contains("name") && j2["name"].is_string()) {
    std::string name = j2["name"];
}

// 6. Iterate over array elements
json arr = {1, 2, 3, 4, 5};
for (auto& element : arr) {
    std::cout << element << " ";
}

// 7. Use JSON Pointer for deep access
json nested = {{"a", {{"b", {{"c", 42}}}}}};
int value = nested["/a/b/c"_json_pointer];

// 8. Convert to/from STL containers
std::vector<int> vec = {1, 2, 3};
json j_vec(vec);
std::vector<int> vec2 = j_vec.get<std::vector<int>>();
```
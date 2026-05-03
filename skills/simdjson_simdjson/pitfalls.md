# Pitfalls

**Pitfall 1: Forgetting to pad input strings**

BAD:
```cpp
std::string json = R"({"key": "value"})";
ondemand::parser parser;
auto doc = parser.iterate(json); // CRASH: no padding
```

GOOD:
```cpp
std::string json = R"({"key": "value"})";
padded_string padded = padded_string(json); // Adds padding
ondemand::parser parser;
auto doc = parser.iterate(padded);
```

**Pitfall 2: Using string_view after source is destroyed**

BAD:
```cpp
std::string_view get_value() {
    padded_string json = padded_string::load("data.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    return doc["name"].get_string().value(); // Dangling reference!
}
```

GOOD:
```cpp
std::string get_value() {
    padded_string json = padded_string::load("data.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    return std::string(doc["name"].get_string().value()); // Copy to string
}
```

**Pitfall 3: Accessing elements after moving the document**

BAD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto doc2 = std::move(doc);
std::cout << doc["key"].get_string(); // Undefined behavior
```

GOOD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto doc2 = std::move(doc);
std::cout << doc2["key"].get_string(); // Use the moved-to object
```

**Pitfall 4: Not checking for errors**

BAD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto value = doc["missing_key"].get_uint64(); // May crash on error
std::cout << value << std::endl;
```

GOOD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto result = doc["missing_key"].get_uint64();
if (!result.error()) {
    std::cout << result.value() << std::endl;
} else {
    std::cerr << "Error: " << result.error() << std::endl;
}
```

**Pitfall 5: Reusing parser without resetting**

BAD:
```cpp
ondemand::parser parser;
auto json1 = padded_string::load("file1.json");
auto doc1 = parser.iterate(json1);
auto json2 = padded_string::load("file2.json");
auto doc2 = parser.iterate(json2); // Parser still has state from doc1
```

GOOD:
```cpp
ondemand::parser parser;
auto json1 = padded_string::load("file1.json");
{
    auto doc1 = parser.iterate(json1);
    // Use doc1
} // doc1 destroyed, parser reset
auto json2 = padded_string::load("file2.json");
auto doc2 = parser.iterate(json2); // Safe now
```

**Pitfall 6: Assuming array elements are ordered**

BAD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto arr = doc["items"].get_array();
auto first = arr.begin(); // May not be the first element if accessed out of order
```

GOOD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto arr = doc["items"].get_array();
for (auto element : arr) {
    // Process in order
}
```

**Pitfall 7: Using get_string() on non-string values**

BAD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto value = doc["count"].get_string(); // count is a number, not string
```

GOOD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto value = doc["count"].get_uint64(); // Correct type
```

**Pitfall 8: Not handling null values**

BAD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto value = doc["optional_field"].get_string(); // May be null
```

GOOD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto result = doc["optional_field"].get_string();
if (result.error() == SUCCESS) {
    std::cout << result.value() << std::endl;
} else if (result.error() == INCORRECT_TYPE) {
    std::cout << "Value is null or wrong type" << std::endl;
}
```
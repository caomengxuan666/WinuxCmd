# Safety

**Red Line 1: NEVER pass unpadded strings to simdjson functions**

BAD:
```cpp
std::string json = R"({"data": "test"})";
ondemand::parser parser;
auto doc = parser.iterate(json); // Buffer overflow risk
```

GOOD:
```cpp
padded_string json = padded_string(R"({"data": "test"})");
ondemand::parser parser;
auto doc = parser.iterate(json); // Safe
```

**Red Line 2: NEVER use string_view after the source padded_string is destroyed**

BAD:
```cpp
std::string_view name;
{
    padded_string json = padded_string::load("data.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    name = doc["name"].get_string().value();
} // json destroyed, name is dangling
std::cout << name; // Undefined behavior
```

GOOD:
```cpp
std::string name;
{
    padded_string json = padded_string::load("data.json");
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    name = std::string(doc["name"].get_string().value());
} // json destroyed, but name is a copy
std::cout << name; // Safe
```

**Red Line 3: NEVER access a document after the parser is destroyed or reused**

BAD:
```cpp
ondemand::document doc;
{
    ondemand::parser parser;
    auto json = padded_string::load("data.json");
    doc = parser.iterate(json);
} // parser destroyed, doc is invalid
std::cout << doc["key"].get_string(); // Undefined behavior
```

GOOD:
```cpp
{
    ondemand::parser parser;
    auto json = padded_string::load("data.json");
    auto doc = parser.iterate(json);
    std::cout << doc["key"].get_string(); // Safe within scope
} // Everything destroyed properly
```

**Red Line 4: NEVER ignore error returns from simdjson functions**

BAD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto value = doc["key"].get_uint64(); // Assumes success, may crash
process_value(value);
```

GOOD:
```cpp
ondemand::parser parser;
auto json = padded_string::load("data.json");
auto doc = parser.iterate(json);
auto result = doc["key"].get_uint64();
if (result.error()) {
    handle_error(result.error());
    return;
}
process_value(result.value());
```

**Red Line 5: NEVER modify the underlying padded_string while a document references it**

BAD:
```cpp
padded_string json = padded_string::load("data.json");
ondemand::parser parser;
auto doc = parser.iterate(json);
json.data()[0] = '{'; // Modifying buffer while doc references it
std::cout << doc["key"].get_string(); // Undefined behavior
```

GOOD:
```cpp
padded_string json = padded_string::load("data.json");
{
    ondemand::parser parser;
    auto doc = parser.iterate(json);
    std::cout << doc["key"].get_string(); // Read-only access
} // doc destroyed
json.data()[0] = '{'; // Safe now, no active references
```
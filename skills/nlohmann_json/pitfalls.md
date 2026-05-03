# Pitfalls

### 1. Using operator[] on non-existent keys creates them
**BAD:**
```cpp
json j;
if (j["non_existent"] == nullptr) { // Creates key "non_existent"!
    // This will always be true, but j now has a key
}
```

**GOOD:**
```cpp
json j;
if (j.contains("non_existent") && j["non_existent"].is_null()) {
    // Safe check without creating keys
}
// Or use find():
if (j.find("non_existent") != j.end()) {
    // Key exists
}
```

### 2. Assuming type of parsed values
**BAD:**
```cpp
json j = json::parse(R"({"count": "42"})");
int count = j["count"]; // Throws because value is string, not number
```

**GOOD:**
```cpp
json j = json::parse(R"({"count": "42"})");
if (j["count"].is_number()) {
    int count = j["count"];
} else if (j["count"].is_string()) {
    int count = std::stoi(j["count"].get<std::string>());
}
```

### 3. Modifying while iterating over object
**BAD:**
```cpp
json j = {{"a", 1}, {"b", 2}, {"c", 3}};
for (auto& [key, val] : j.items()) {
    if (val == 2) {
        j.erase(key); // Undefined behavior - invalidates iterator
    }
}
```

**GOOD:**
```cpp
json j = {{"a", 1}, {"b", 2}, {"c", 3}};
std::vector<std::string> to_erase;
for (auto& [key, val] : j.items()) {
    if (val == 2) {
        to_erase.push_back(key);
    }
}
for (const auto& key : to_erase) {
    j.erase(key);
}
```

### 4. Not handling parse exceptions
**BAD:**
```cpp
std::ifstream f("config.json");
json j = json::parse(f); // Crashes if file doesn't exist or contains invalid JSON
```

**GOOD:**
```cpp
try {
    std::ifstream f("config.json");
    if (!f.is_open()) {
        throw std::runtime_error("Cannot open file");
    }
    json j = json::parse(f);
} catch (const json::parse_error& e) {
    std::cerr << "Parse error: " << e.what() << '\n';
}
```

### 5. Assuming dump() returns valid UTF-8
**BAD:**
```cpp
json j = "Invalid UTF-8: \xFF\xFE";
std::string s = j.dump(); // May throw or produce invalid output
```

**GOOD:**
```cpp
json j = "Invalid UTF-8: \xFF\xFE";
std::string s = j.dump(-1, ' ', false, json::error_handler_t::replace);
// Replaces invalid sequences with replacement character
```

### 6. Using implicit conversion without checking
**BAD:**
```cpp
json j = "hello";
int x = j; // Compiles but throws at runtime!
```

**GOOD:**
```cpp
json j = "hello";
if (j.is_number()) {
    int x = j;
} else {
    // Handle type mismatch
}
```
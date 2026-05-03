# Safety

### Condition 1: NEVER access array elements without bounds checking
**BAD:**
```cpp
json arr = {1, 2, 3};
int val = arr[5]; // Throws json::out_of_range
```

**GOOD:**
```cpp
json arr = {1, 2, 3};
if (arr.size() > 5) {
    int val = arr[5];
} else {
    // Handle out of bounds
}
```

### Condition 2: NEVER parse untrusted input without error handling
**BAD:**
```cpp
json j = json::parse(user_input); // Can throw on malformed input
```

**GOOD:**
```cpp
try {
    json j = json::parse(user_input);
} catch (const json::parse_error& e) {
    // Log error, reject input
}
```

### Condition 3: NEVER use operator[] on const objects
**BAD:**
```cpp
const json j = {{"key", "value"}};
auto val = j["key"]; // Compiles but creates temporary, modifies const object!
```

**GOOD:**
```cpp
const json j = {{"key", "value"}};
auto val = j.at("key"); // Throws if key doesn't exist
// Or use value() with default:
auto val = j.value("key", "default");
```

### Condition 4: NEVER assume dump() won't throw
**BAD:**
```cpp
json j = "Some string";
std::string s = j.dump(); // Assumes no exceptions
```

**GOOD:**
```cpp
json j = "Some string";
try {
    std::string s = j.dump();
} catch (const json::type_error& e) {
    // Handle serialization error
}
```

### Condition 5: NEVER mix different json types in arithmetic
**BAD:**
```cpp
json j1 = 42;
json j2 = "hello";
auto result = j1 + j2; // Compiles but throws at runtime
```

**GOOD:**
```cpp
json j1 = 42;
json j2 = 10;
if (j1.is_number() && j2.is_number()) {
    auto result = j1.get<int>() + j2.get<int>();
}
```
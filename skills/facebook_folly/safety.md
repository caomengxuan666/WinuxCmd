# Safety

### Condition 1: NEVER access folly::Optional without checking
```cpp
// BAD: Unsafe access
folly::Optional<int> opt;
int val = *opt;  // Undefined behavior if empty
```

```cpp
// GOOD: Safe access with check
folly::Optional<int> opt;
if (opt.hasValue()) {
    int val = *opt;
}
```

### Condition 2: NEVER use StringPiece with temporary strings
```cpp
// BAD: Dangling reference
folly::StringPiece getPiece() {
    std::string temp = "hello";
    return temp;  // Returns dangling pointer
}
```

```cpp
// GOOD: Ensure lifetime extends
folly::StringPiece getPiece(const std::string& s) {
    return s;  // Safe - caller manages lifetime
}
```

### Condition 3: NEVER assume dynamic type without checking
```cpp
// BAD: Unsafe type assumption
folly::dynamic d = folly::dynamic::array(1, 2, 3);
d["key"] = "value";  // Throws - array doesn't support key access
```

```cpp
// GOOD: Check type before operation
folly::dynamic d = folly::dynamic::array(1, 2, 3);
if (d.isObject()) {
    d["key"] = "value";
}
```

### Condition 4: NEVER use folly::to with potentially null pointers
```cpp
// BAD: Null pointer conversion
const char* ptr = nullptr;
auto str = folly::to<std::string>(ptr);  // Undefined behavior
```

```cpp
// GOOD: Check for null first
const char* ptr = nullptr;
if (ptr) {
    auto str = folly::to<std::string>(ptr);
}
```

### Condition 5: NEVER ignore folly::Format exceptions
```cpp
// BAD: Unchecked format
auto result = folly::format("{}", some_value);  // May throw
```

```cpp
// GOOD: Handle format errors
try {
    auto result = folly::format("{}", some_value);
} catch (const std::exception& e) {
    // Handle formatting error
}
```
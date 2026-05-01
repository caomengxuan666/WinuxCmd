# Safety

**Red Line 1: NEVER modify the map while iterating with range-based for**
```cpp
// BAD: Insertion during iteration
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}};
for (auto& [key, value] : map) {  // Undefined behavior
    map.insert({key + 10, value});
}

// GOOD: Collect modifications first
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}};
std::vector<std::pair<int, int>> to_insert;
for (const auto& [key, value] : map) {
    to_insert.emplace_back(key + 10, value);
}
for (const auto& pair : to_insert) {
    map.insert(pair);
}
```

**Red Line 2: NEVER use iterators after any modifying operation**
```cpp
// BAD: Iterator use after erase
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}};
auto it = map.find(1);
map.erase(2);  // Invalidates all iterators
if (it != map.end()) {  // Undefined behavior
    std::cout << it->second;
}

// GOOD: Re-acquire iterator after modification
tsl::robin_map<int, int> map = {{1, 1}, {2, 2}};
map.erase(2);
auto it = map.find(1);  // Safe to use after re-acquisition
if (it != map.end()) {
    std::cout << it->second;
}
```

**Red Line 3: NEVER assume strong exception guarantee with non-noexcept types**
```cpp
// BAD: Using types that may throw during move/swap
struct ThrowingType {
    ThrowingType(ThrowingType&&) noexcept(false) {}
};
tsl::robin_map<int, ThrowingType> map;
try {
    map.insert({1, ThrowingType{}});
} catch (...) {
    // Map may be in undefined state
    map.clear();  // May crash or behave unexpectedly
}

// GOOD: Use noexcept types or handle carefully
struct SafeType {
    SafeType(SafeType&&) noexcept = default;
};
tsl::robin_map<int, SafeType> map;
try {
    map.insert({1, SafeType{}});
} catch (...) {
    // Map is in valid state
    map.clear();  // Safe
}
```

**Red Line 4: NEVER use heterogeneous lookup without proper comparator**
```cpp
// BAD: Heterogeneous lookup without transparent comparator
tsl::robin_map<std::string, int> map = {{"hello", 1}};
std::string_view sv = "hello";
auto it = map.find(sv);  // May not compile or give wrong results

// GOOD: Use transparent comparator
struct StringHash {
    using is_transparent = void;
    std::size_t operator()(const std::string& s) const { return std::hash<std::string>{}(s); }
    std::size_t operator()(std::string_view sv) const { return std::hash<std::string_view>{}(sv); }
};
struct StringEqual {
    using is_transparent = void;
    bool operator()(const std::string& a, const std::string& b) const { return a == b; }
    bool operator()(const std::string& a, std::string_view b) const { return a == b; }
};
tsl::robin_map<std::string, int, StringHash, StringEqual> map = {{"hello", 1}};
auto it = map.find(std::string_view("hello"));  // Safe
```

**Red Line 5: NEVER use serialized data across different architectures without handling endianness**
```cpp
// BAD: Assuming binary compatibility
tsl::robin_map<int, int> map = {{1, 0x01020304}};
std::string data = map.serialize([](const auto& pair) {
    return std::string(reinterpret_cast<const char*>(&pair), sizeof(pair));
});
// On big-endian system, deserialization will produce wrong values

// GOOD: Use portable serialization
tsl::robin_map<int, int> map = {{1, 0x01020304}};
std::string data = map.serialize([](const auto& pair) {
    std::string result;
    // Serialize in network byte order
    uint32_t key = htonl(pair.first);
    uint32_t value = htonl(pair.second);
    result.append(reinterpret_cast<const char*>(&key), sizeof(key));
    result.append(reinterpret_cast<const char*>(&value), sizeof(value));
    return result;
});
```
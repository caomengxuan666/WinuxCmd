# Pitfalls

**Pitfall 1: Modifying values through iterator incorrectly**
```cpp
// BAD: Direct modification of pair through iterator
tsl::robin_map<int, int> map = {{1, 1}};
auto it = map.begin();
it->second = 2;  // Compilation error or undefined behavior

// GOOD: Use value() method
tsl::robin_map<int, int> map = {{1, 1}};
auto it = map.begin();
it.value() = 2;  // Correct way to modify value
```

**Pitfall 2: Assuming iterator stability after modifications**
```cpp
// BAD: Using iterator after insertion
tsl::robin_map<int, int> map = {{1, 1}};
auto it = map.find(1);
map.insert({2, 2});  // Invalidates all iterators
std::cout << it->second;  // Undefined behavior

// GOOD: Re-find after modification
tsl::robin_map<int, int> map = {{1, 1}};
map.insert({2, 2});
auto it = map.find(1);  // Re-acquire iterator
std::cout << it->second;  // Safe
```

**Pitfall 3: Using power-of-two policy with poor hash function**
```cpp
// BAD: Identity hash with power-of-two policy
tsl::robin_map<int*, std::string> map;  // Default power-of-two
int a, b, c;
map[&a] = "first";
map[&b] = "second";  // May cause many collisions

// GOOD: Use prime growth policy for pointers
tsl::robin_pg_map<int*, std::string> map;  // Prime growth
int a, b, c;
map[&a] = "first";
map[&b] = "second";  // Better distribution
```

**Pitfall 4: Forgetting about strong exception guarantee limitations**
```cpp
// BAD: Using types without noexcept move/swap
struct MyType {
    std::vector<int> data;
    // No noexcept move constructor or swap
};
tsl::robin_map<int, MyType> map;  // May leave map in undefined state on exception

// GOOD: Ensure noexcept operations
struct MyType {
    std::vector<int> data;
    MyType(MyType&& other) noexcept : data(std::move(other.data)) {}
    void swap(MyType& other) noexcept { data.swap(other.data); }
};
tsl::robin_map<int, MyType> map;  // Safe
```

**Pitfall 5: Incorrect serialization/deserialization**
```cpp
// BAD: Assuming binary compatibility
tsl::robin_map<int, double> map = {{1, 1.5}};
std::string data = map.serialize([](const auto& pair) {
    return std::to_string(pair.first) + ":" + std::to_string(pair.second);
});
// Deserializing on different architecture may fail

// GOOD: Handle endianness explicitly
tsl::robin_map<int, double> map = {{1, 1.5}};
std::string data = map.serialize([](const auto& pair) {
    // Use portable serialization format
    return portable_serialize(pair.first, pair.second);
});
```

**Pitfall 6: Using pre-calculated hash with wrong hash function**
```cpp
// BAD: Mismatched hash calculation
tsl::robin_map<int, std::string> map = {{1, "one"}};
std::size_t hash = std::hash<int>{}(1);  // Wrong hash function
auto it = map.find(1, hash);  // May not find element

// GOOD: Use map's hash function
tsl::robin_map<int, std::string> map = {{1, "one"}};
std::size_t hash = map.hash_function()(1);  // Correct hash function
auto it = map.find(1, hash);  // Correct lookup
```

**Pitfall 7: Memory overhead from storing hash values**
```cpp
// BAD: Unnecessary hash storage for small keys
tsl::robin_map<int, std::string, std::hash<int>, std::equal_to<int>, 
               std::true_type> map;  // StoreHash = true, wastes memory

// GOOD: Let library decide based on alignment
tsl::robin_map<int, std::string> map;  // Default StoreHash = false
```
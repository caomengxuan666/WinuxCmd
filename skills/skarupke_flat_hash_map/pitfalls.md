# Pitfalls

### 1. Iterator Invalidation on Insertion
```cpp
// BAD: Iterators invalidate when rehash occurs
ska::flat_hash_map<int, int> map = {{1, 10}, {2, 20}};
auto it = map.find(1);
map[3] = 30;  // May cause rehash, invalidating 'it'
std::cout << it->second;  // UNDEFINED BEHAVIOR

// GOOD: Re-obtain iterator after insertion
ska::flat_hash_map<int, int> map = {{1, 10}, {2, 20}};
auto it = map.find(1);
map.reserve(map.size() + 1);  // Prevent rehash
map[3] = 30;
std::cout << it->second;  // Safe
```

### 2. Using operator[] for Lookup Only
```cpp
// BAD: operator[] inserts default value if key doesn't exist
ska::flat_hash_map<int, int> map;
if (map[42] == 0) {  // Inserts {42, 0} even if you just wanted to check
    // ...
}

// GOOD: Use find() or contains() for lookup-only
ska::flat_hash_map<int, int> map;
auto it = map.find(42);
if (it != map.end() && it->second == 0) {
    // ...
}
```

### 3. Modifying Keys During Iteration
```cpp
// BAD: Modifying key through iterator corrupts hash table
ska::flat_hash_map<int, int> map = {{1, 10}, {2, 20}};
for (auto& [key, value] : map) {
    key = 42;  // Changes hash value, table becomes corrupted
}

// GOOD: Remove and re-insert with new key
ska::flat_hash_map<int, int> map = {{1, 10}, {2, 20}};
int old_key = 1;
int new_key = 42;
auto it = map.find(old_key);
if (it != map.end()) {
    int value = it->second;
    map.erase(it);
    map[new_key] = value;
}
```

### 4. Assuming Stable References
```cpp
// BAD: References invalidate on rehash
ska::flat_hash_map<int, std::string> map;
map[1] = "hello";
std::string& ref = map[1];  // Reference to internal storage
map[2] = "world";  // May cause rehash, 'ref' becomes dangling
ref = "changed";   // UNDEFINED BEHAVIOR

// GOOD: Use pointers or copy values
ska::flat_hash_map<int, std::string> map;
map[1] = "hello";
std::string copy = map[1];  // Safe copy
map[2] = "world";
// Use 'copy' instead of dangling reference
```

### 5. Incorrect Hash Function for Custom Types
```cpp
// BAD: Weak hash function causes collisions
struct BadHash {
    std::size_t operator()(int x) const { return x % 100; }  // Only 100 buckets
};

// GOOD: Use standard hash or well-distributed custom hash
struct GoodHash {
    std::size_t operator()(int x) const { 
        return std::hash<int>{}(x);  // Uses std::hash
    }
};
```

### 6. Not Reserving Space for Known Size
```cpp
// BAD: Multiple rehashes during insertion
ska::flat_hash_map<int, int> map;
for (int i = 0; i < 10000; ++i) {
    map[i] = i;  // Multiple rehashes as map grows
}

// GOOD: Reserve space upfront
ska::flat_hash_map<int, int> map;
map.reserve(10000);  // Single allocation
for (int i = 0; i < 10000; ++i) {
    map[i] = i;  // No rehashes
}
```
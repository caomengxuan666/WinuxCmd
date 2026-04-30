# Safety

### 1. NEVER use iterators after insertion that may cause rehash
```cpp
// BAD: Iterator used after potential rehash
ska::flat_hash_map<int, int> map;
auto it = map.begin();
map[1] = 10;  // May rehash
it->second = 20;  // UNDEFINED BEHAVIOR

// GOOD: Re-obtain iterator after modifications
ska::flat_hash_map<int, int> map;
map[1] = 10;
auto it = map.begin();
it->second = 20;  // Safe
```

### 2. NEVER modify keys through iterators
```cpp
// BAD: Key modification corrupts hash table
ska::flat_hash_map<std::string, int> map;
map["hello"] = 1;
for (auto& [key, value] : map) {
    key = "world";  // Corrupts hash table
}

// GOOD: Remove and re-insert
ska::flat_hash_map<std::string, int> map;
map["hello"] = 1;
auto it = map.find("hello");
int val = it->second;
map.erase(it);
map["world"] = val;
```

### 3. NEVER assume pointer/reference stability across insertions
```cpp
// BAD: Storing pointer to value
ska::flat_hash_map<int, int> map;
map[1] = 10;
int* ptr = &map[1];
map[2] = 20;  // May invalidate ptr
*ptr = 30;    // UNDEFINED BEHAVIOR

// GOOD: Store by value or use stable container
ska::flat_hash_map<int, std::unique_ptr<int>> map;
map[1] = std::make_unique<int>(10);
int* ptr = map[1].get();  // Pointer to heap memory, not map storage
map[2] = std::make_unique<int>(20);
*ptr = 30;  // Safe
```

### 4. NEVER use operator[] for read-only access in multithreaded code
```cpp
// BAD: operator[] may modify map even for read
ska::flat_hash_map<int, int> map;
// Thread 1
if (map[42] == 0) { /* ... */ }  // May insert if missing
// Thread 2
map[43] = 10;  // Race condition

// GOOD: Use find() for read-only access
ska::flat_hash_map<int, int> map;
// Thread 1 (with external mutex)
auto it = map.find(42);
if (it != map.end() && it->second == 0) { /* ... */ }
// Thread 2 (with same mutex)
map[43] = 10;
```

### 5. NEVER assume the map is empty after clear() if you have external pointers
```cpp
// BAD: External pointers to values become dangling
ska::flat_hash_map<int, int> map;
map[1] = 10;
int* ptr = &map[1];  // DANGEROUS
map.clear();
*ptr = 20;  // UNDEFINED BEHAVIOR

// GOOD: Don't store pointers to map elements
ska::flat_hash_map<int, int> map;
map[1] = 10;
int value = map[1];  // Copy value
map.clear();
// Use 'value' instead of dangling pointer
```
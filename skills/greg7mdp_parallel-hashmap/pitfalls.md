# Pitfalls


**Pitfall 1: Pointer invalidation with flat_hash_map**

```cpp
// BAD: Storing pointers to elements in flat_hash_map
phmap::flat_hash_map<int, std::string> map;
map[1] = "hello";
std::string* ptr = &map[1];  // Dangerous!
map[2] = "world";  // May rehash and invalidate ptr
// ptr is now dangling!

// GOOD: Use node_hash_map for pointer stability
phmap::node_hash_map<int, std::string> node_map;
node_map[1] = "hello";
std::string* safe_ptr = &node_map[1];  // Safe - node_hash_map preserves pointers
node_map[2] = "world";  // safe_ptr remains valid
```

**Pitfall 2: Iterator invalidation during insertion**

```cpp
// BAD: Inserting while iterating
phmap::flat_hash_map<int, int> map = {{1, 10}, {2, 20}};
for (auto it = map.begin(); it != map.end(); ++it) {
    if (it->first == 1) {
        map[3] = 30;  // May invalidate iterator!
    }
}

// GOOD: Collect keys first, then modify
std::vector<int> keys_to_modify;
for (const auto& [key, value] : map) {
    if (key == 1) keys_to_modify.push_back(key);
}
for (int key : keys_to_modify) {
    map[3] = 30;  // Safe - not iterating
}
```

**Pitfall 3: Assuming thread safety with non-parallel maps**

```cpp
// BAD: Concurrent writes to flat_hash_map
phmap::flat_hash_map<int, int> map;
std::thread t1([&]() { for (int i = 0; i < 100; ++i) map[i] = i; });
std::thread t2([&]() { for (int i = 100; i < 200; ++i) map[i] = i; });
t1.join(); t2.join();  // Data race!

// GOOD: Use parallel variant
phmap::parallel_flat_hash_map<int, int> parallel_map;
std::thread t3([&]() { for (int i = 0; i < 100; ++i) parallel_map[i] = i; });
std::thread t4([&]() { for (int i = 100; i < 200; ++i) parallel_map[i] = i; });
t3.join(); t4.join();  // Safe
```

**Pitfall 4: Forgetting to reserve for known size**

```cpp
// BAD: No reservation, causing multiple rehashes
phmap::flat_hash_map<int, int> map;
for (int i = 0; i < 100000; ++i) {
    map[i] = i;  // Multiple rehashes as map grows
}

// GOOD: Reserve space upfront
phmap::flat_hash_map<int, int> map2;
map2.reserve(100000);
for (int i = 0; i < 100000; ++i) {
    map2[i] = i;  // No rehashes
}
```

**Pitfall 5: Using operator[] when you mean find**

```cpp
// BAD: operator[] creates default entries
phmap::flat_hash_map<std::string, int> scores;
if (scores["Alice"] == 0) {  // Creates entry "Alice" with value 0!
    std::cout << "Alice not found\n";
}

// GOOD: Use find or contains
auto it = scores.find("Alice");
if (it == scores.end()) {
    std::cout << "Alice not found\n";
}
```

**Pitfall 6: Ignoring heterogeneous lookup benefits**

```cpp
// BAD: Creating temporary strings for lookup
phmap::flat_hash_map<std::string, int> map;
const char* key = "hello";
auto it = map.find(std::string(key));  // Unnecessary string copy

// GOOD: Use string_view or const char* directly
auto it2 = map.find(std::string_view(key));  // No copy needed
```
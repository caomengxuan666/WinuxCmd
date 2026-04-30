# Safety


**Red Line 1: NEVER use flat_hash_map when pointer stability is required**

```cpp
// BAD: Pointer to flat_hash_map element
phmap::flat_hash_map<int, int> map;
map[1] = 100;
int* ptr = &map[1];  // CRITICAL: May be invalidated
map.rehash(1000);    // ptr is now dangling!

// GOOD: Use node_hash_map for pointer stability
phmap::node_hash_map<int, int> node_map;
node_map[1] = 100;
int* safe_ptr = &node_map[1];  // Safe
node_map.rehash(1000);         // safe_ptr still valid
```

**Red Line 2: NEVER perform concurrent writes on non-parallel maps**

```cpp
// BAD: Concurrent writes to flat_hash_map
phmap::flat_hash_map<int, int> map;
auto worker = [&map](int start) {
    for (int i = start; i < start + 100; ++i)
        map[i] = i;  // CRITICAL: Data race!
};
std::thread t1(worker, 0);
std::thread t2(worker, 100);
t1.join(); t2.join();

// GOOD: Use parallel variant or external mutex
phmap::parallel_flat_hash_map<int, int> safe_map;
auto safe_worker = [&safe_map](int start) {
    for (int i = start; i < start + 100; ++i)
        safe_map[i] = i;  // Safe with parallel variant
};
std::thread t3(safe_worker, 0);
std::thread t4(safe_worker, 100);
t3.join(); t4.join();
```

**Red Line 3: NEVER use operator[] for existence checking**

```cpp
// BAD: Checking existence with operator[]
phmap::flat_hash_map<std::string, int> map;
if (map["nonexistent"] == 0) {  // CRITICAL: Creates entry!
    // This branch always executes, even for missing keys
}

// GOOD: Use contains() or find()
if (!map.contains("nonexistent")) {
    std::cout << "Key doesn't exist\n";
}
```

**Red Line 4: NEVER assume iterator validity after insertion**

```cpp
// BAD: Using iterator after insertion
phmap::flat_hash_map<int, int> map = {{1, 10}};
auto it = map.begin();
map[2] = 20;  // May rehash
int val = it->second;  // CRITICAL: it may be invalidated!

// GOOD: Re-acquire iterator after modification
auto it2 = map.find(1);  // Get fresh iterator
if (it2 != map.end()) {
    int val = it2->second;  // Safe
}
```

**Red Line 5: NEVER use serialization with non-trivially copyable types**

```cpp
// BAD: Serializing map with non-trivially copyable values
phmap::flat_hash_map<int, std::string> bad_map;
bad_map[1] = "hello";
std::ofstream file("bad.bin", std::ios::binary);
bad_map.serialize(file);  // CRITICAL: Undefined behavior!

// GOOD: Only serialize trivially copyable types
phmap::flat_hash_map<int, double> good_map;
good_map[1] = 3.14;
std::ofstream file2("good.bin", std::ios::binary);
good_map.serialize(file2);  // Safe - double is trivially copyable
```
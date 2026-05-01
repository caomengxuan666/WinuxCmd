# Best Practices

**Best Practice 1: Choose the right growth policy**
```cpp
// For general use with good hash functions
tsl::robin_map<int, std::string> map;  // Power-of-two, fastest

// For pointers or identity hashes
tsl::robin_pg_map<int*, std::string> ptr_map;  // Prime growth, better distribution

// For custom growth requirements
tsl::robin_map<int, std::string, std::hash<int>, std::equal_to<int>,
               tsl::rh::mod_growth_policy<>> custom_map;  // Customizable growth
```

**Best Practice 2: Reserve space when size is known**
```cpp
// BAD: Letting map grow incrementally
tsl::robin_map<int, std::string> map;
for (int i = 0; i < 10000; ++i) {
    map[i] = "value";  // Multiple rehashes
}

// GOOD: Pre-allocate space
tsl::robin_map<int, std::string> map;
map.reserve(10000);  // Single allocation
for (int i = 0; i < 10000; ++i) {
    map[i] = "value";  // No rehashes
}
```

**Best Practice 3: Use emplace for efficient insertion**
```cpp
// BAD: Creating temporary objects
tsl::robin_map<int, std::pair<int, std::string>> map;
map.insert({1, {2, "hello"}});  // Creates temporary pair

// GOOD: Construct in-place
tsl::robin_map<int, std::pair<int, std::string>> map;
map.emplace(1, std::piecewise_construct,
            std::forward_as_tuple(2),
            std::forward_as_tuple("hello"));  // No temporaries
```

**Best Practice 4: Use try_emplace for conditional insertion**
```cpp
// BAD: Checking existence then inserting
tsl::robin_map<int, ExpensiveObject> map;
if (map.find(1) == map.end()) {
    map[1] = ExpensiveObject{};  // May construct twice
}

// GOOD: Use try_emplace
tsl::robin_map<int, ExpensiveObject> map;
auto [it, inserted] = map.try_emplace(1, args...);  // Single construction
if (inserted) {
    // Successfully inserted
}
```

**Best Practice 5: Use extract/insert for moving between maps**
```cpp
// BAD: Copying then erasing
tsl::robin_map<int, std::string> source = {{1, "one"}, {2, "two"}};
tsl::robin_map<int, std::string> dest;
auto it = source.find(1);
if (it != source.end()) {
    dest[it->first] = it->second;  // Copy
    source.erase(it);  // Extra operation
}

// GOOD: Move between maps
tsl::robin_map<int, std::string> source = {{1, "one"}, {2, "two"}};
tsl::robin_map<int, std::string> dest;
auto it = source.find(1);
if (it != source.end()) {
    dest.insert(source.extract(it));  // Move without copy
}
```

**Best Practice 6: Use pre-calculated hash for repeated lookups**
```cpp
// BAD: Recalculating hash each time
tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
for (int i = 0; i < 100; ++i) {
    auto it = map.find(1);  // Hash recalculated each iteration
}

// GOOD: Calculate hash once
tsl::robin_map<int, std::string> map = {{1, "one"}, {2, "two"}};
std::size_t hash = map.hash_function()(1);
for (int i = 0; i < 100; ++i) {
    auto it = map.find(1, hash);  // Uses pre-calculated hash
}
```
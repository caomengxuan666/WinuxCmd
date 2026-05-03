# Performance

**Flat Hash Map Performance Characteristics**

```cpp
#include "absl/container/flat_hash_map.h"

// flat_hash_map is optimized for:
// - Fast lookups (O(1) average)
// - Good cache locality (contiguous storage)
// - Low memory overhead compared to std::unordered_map

absl::flat_hash_map<int, int> map;
map.reserve(1000000); // Pre-allocate to avoid rehashing

// Insertion is faster than std::unordered_map
for (int i = 0; i < 1000000; ++i) {
    map[i] = i * 2;
}

// Lookups are also faster
auto it = map.find(500000);
```

**String Operations Performance**

```cpp
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

// absl::StrCat is more efficient than operator+
std::string result = absl::StrCat("a", "b", "c", "d"); // Single allocation

// absl::StrJoin is efficient for joining containers
std::vector<std::string> parts = {"one", "two", "three"};
std::string joined = absl::StrJoin(parts, ","); // Efficient single pass
```

**InlinedVector Performance**

```cpp
#include "absl/container/inlined_vector.h"

// InlinedVector avoids heap allocation for small sizes
absl::InlinedVector<int, 8> vec; // No heap allocation for up to 8 elements

// For small vectors, this is much faster than std::vector
for (int i = 0; i < 8; ++i) {
    vec.push_back(i); // All inline, no heap allocation
}

// When size exceeds inline capacity, it falls back to heap
vec.push_back(9); // Now heap allocated
```

**Allocation Pattern Optimization**

```cpp
#include "absl/container/flat_hash_map.h"

// Avoid repeated allocations by reserving space
absl::flat_hash_map<int, int> map;
map.reserve(1000); // Single allocation for 1000 elements

// Use try_emplace to avoid unnecessary copies
for (int i = 0; i < 1000; ++i) {
    map.try_emplace(i, i * 2); // More efficient than operator[]
}
```
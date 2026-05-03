# Lifecycle

**Construction and Destruction of absl::flat_hash_map**

```cpp
#include "absl/container/flat_hash_map.h"

// Default construction
absl::flat_hash_map<int, std::string> map;

// Construction with initializer list
absl::flat_hash_map<int, std::string> map2 = {{1, "one"}, {2, "two"}};

// Destruction is automatic when map goes out of scope
{
    absl::flat_hash_map<int, std::string> temp = {{1, "temp"}};
} // temp is destroyed here
```

**Move Semantics**

```cpp
#include "absl/container/flat_hash_map.h"

absl::flat_hash_map<int, std::string> source = {{1, "one"}, {2, "two"}};

// Move construction - source is left in valid but unspecified state
absl::flat_hash_map<int, std::string> dest = std::move(source);

// Move assignment
absl::flat_hash_map<int, std::string> another;
another = std::move(dest);
```

**Resource Management with InlinedVector**

```cpp
#include "absl/container/inlined_vector.h"

// InlinedVector stores small number of elements inline
absl::InlinedVector<int, 4> vec;

// When size exceeds inline capacity, it allocates on heap
vec.push_back(1);
vec.push_back(2);
vec.push_back(3);
vec.push_back(4);
vec.push_back(5); // Now allocated on heap

// clear() deallocates heap memory
vec.clear(); // If previously heap-allocated, memory is freed
```

**Lifecycle of absl::Status**

```cpp
#include "absl/status/status.h"

// Status objects are lightweight and can be copied/moved
absl::Status ok_status = absl::OkStatus();
absl::Status error_status = absl::InternalError("Something went wrong");

// StatusOr manages the lifecycle of contained value
absl::StatusOr<std::string> result = absl::InternalError("Failed");
// result is in error state, no string was constructed

result = "success"; // Now result contains a string
// The string will be properly destroyed when result goes out of scope
```
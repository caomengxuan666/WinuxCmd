# Performance

### Small string optimization with fbstring
```cpp
#include <folly/FBString.h>

// SSO: No allocation for strings <= 23 chars
folly::fbstring small = "Hello";  // No heap allocation
folly::fbstring large = "This string exceeds the SSO threshold";  // Heap allocated

// Pre-allocation for known sizes
folly::fbstring optimized;
optimized.reserve(1000);  // Avoid reallocations
```

### Cache-friendly containers with fbvector
```cpp
#include <folly/FBVector.h>

// fbvector: Optimized for trivially copyable types
folly::fbvector<int> numbers;
numbers.reserve(10000);  // Contiguous memory, cache-friendly

// Batch operations for performance
folly::fbvector<int> source(1000, 42);
folly::fbvector<int> dest;
dest.insert(dest.end(), source.begin(), source.end());  // Efficient bulk copy
```

### Efficient string conversion
```cpp
#include <folly/Conv.h>

// folly::to avoids temporary allocations
std::string result = folly::to<std::string>(42);  // Direct conversion
int value = folly::to<int>("123");  // No string copy

// Batch conversion
std::vector<int> numbers = {1, 2, 3, 4, 5};
std::string joined;
for (auto n : numbers) {
    joined += folly::to<std::string>(n);  // Efficient per-element
}
```

### Memory allocation patterns
```cpp
#include <folly/FBString.h>
#include <folly/FBVector.h>

// Avoid unnecessary allocations
folly::fbstring efficient;
efficient.reserve(100);  // Single allocation
for (int i = 0; i < 10; ++i) {
    efficient += "data";  // No reallocation
}

// Use emplace_back for in-place construction
folly::fbvector<std::pair<int, std::string>> pairs;
pairs.emplace_back(1, "one");  // Constructs in-place
pairs.emplace_back(2, "two");
```
# Lifecycle

### Construction and initialization
```cpp
#include <folly/FBString.h>
#include <folly/Optional.h>
#include <folly/dynamic.h>

// Default construction
folly::fbstring empty;  // Empty string, no allocation
folly::Optional<int> none;  // Empty optional
folly::dynamic null_dynamic;  // Null dynamic

// Value construction
folly::fbstring hello = "Hello";  // SSO if <= 23 chars
folly::Optional<int> value = 42;  // Contains value
folly::dynamic obj = folly::dynamic::object("key", "value");
```

### Move semantics for efficient transfers
```cpp
#include <folly/FBString.h>

// Move construction - efficient pointer swap
folly::fbstring source = "Large string that exceeds SSO";
folly::fbstring dest = std::move(source);  // source now empty

// Move assignment
folly::fbstring a = "Hello";
folly::fbstring b = "World";
b = std::move(a);  // Efficient transfer
```

### Resource management with RAII
```cpp
#include <folly/FBString.h>
#include <folly/dynamic.h>

// Automatic cleanup
void processData() {
    folly::fbstring data = "temporary";
    folly::dynamic config = folly::dynamic::object;
    // Resources automatically freed when going out of scope
}

// Explicit resource management
folly::fbstring createLargeString() {
    folly::fbstring result;
    result.reserve(1000000);  // Explicit allocation
    // ... fill string ...
    return result;  // Move semantics avoid copy
}
```

### Copy semantics and sharing
```cpp
#include <folly/FBString.h>

// Copy-on-write behavior for large strings
folly::fbstring original = "Large string that exceeds SSO";
folly::fbstring copy = original;  // Shares internal buffer
copy += " modified";  // Creates new copy on modification
```
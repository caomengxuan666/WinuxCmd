# Best Practices

### Use fbstring for performance-critical string operations
```cpp
#include <folly/FBString.h>

// Best practice: Use fbstring for heavy string manipulation
folly::fbstring buildLargeString() {
    folly::fbstring result;
    result.reserve(1000000);  // Pre-allocate for performance
    for (int i = 0; i < 100000; ++i) {
        result += "data";
    }
    return result;
}
```

### Prefer folly::Optional over raw pointers for nullable values
```cpp
#include <folly/Optional.h>

// Best practice: Clear ownership semantics
folly::Optional<int> findValue(const std::vector<int>& vec, int target) {
    for (auto v : vec) {
        if (v == target) return v;
    }
    return folly::none;  // Explicit empty state
}
```

### Use folly::dynamic with type-safe access patterns
```cpp
#include <folly/dynamic.h>
#include <folly/json.h>

// Best practice: Type-safe JSON handling
folly::dynamic parseConfig(const std::string& json) {
    auto config = folly::parseJson(json);
    if (config.isObject() && config.count("timeout")) {
        auto timeout = config["timeout"];
        if (timeout.isInt()) {
            return timeout.getInt();
        }
    }
    return 30;  // Default timeout
}
```

### Leverage folly::Format for type-safe string building
```cpp
#include <folly/Format.h>

// Best practice: Type-safe formatting
std::string createMessage(const std::string& user, int count) {
    return folly::format("User {0} has {1} items", user, count).str();
}
```

### Use folly::split for efficient string parsing
```cpp
#include <folly/String.h>

// Best practice: Efficient string splitting
std::vector<std::string> parseCSV(const std::string& line) {
    std::vector<std::string> fields;
    folly::split(",", line, fields);
    return fields;
}
```
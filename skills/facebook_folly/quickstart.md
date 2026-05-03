# Quickstart

```cpp
#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/Conv.h>
#include <folly/Optional.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <folly/Format.h>
#include <folly/String.h>
#include <iostream>

// 1. FBString - optimized string replacement
void fbstring_example() {
    folly::fbstring hello = "Hello, Folly!";
    folly::fbstring world = "World";
    folly::fbstring combined = hello + " " + world;
    std::cout << combined << std::endl;  // "Hello, Folly! World"
}

// 2. FBVector - cache-friendly vector
void fbvector_example() {
    folly::fbvector<int> numbers = {1, 2, 3, 4, 5};
    numbers.push_back(6);
    for (auto n : numbers) {
        std::cout << n << " ";
    }
}

// 3. to/from string conversion
void conversion_example() {
    std::string str = folly::to<std::string>(42);
    int val = folly::to<int>("123");
    double d = folly::to<double>("3.14");
    std::cout << str << " " << val << " " << d << std::endl;
}

// 4. Optional - nullable value type
void optional_example() {
    folly::Optional<int> maybe_value;
    if (!maybe_value.hasValue()) {
        maybe_value = 42;
    }
    std::cout << maybe_value.value() << std::endl;  // 42
}

// 5. dynamic - JSON-like dynamic typing
void dynamic_example() {
    folly::dynamic obj = folly::dynamic::object;
    obj["name"] = "Folly";
    obj["version"] = 2024;
    obj["features"] = folly::dynamic::array("fast", "safe");
    
    std::string json = folly::toJson(obj);
    std::cout << json << std::endl;
}

// 6. Format - type-safe string formatting
void format_example() {
    std::string result = folly::format("Value: {0}, Name: {1}", 42, "test").str();
    std::cout << result << std::endl;
}

// 7. String utilities
void string_example() {
    std::string input = "a,b,c,d";
    std::vector<folly::StringPiece> parts;
    folly::split(",", input, parts);
    for (auto& part : parts) {
        std::cout << part << " ";
    }
}

int main() {
    fbstring_example();
    fbvector_example();
    conversion_example();
    optional_example();
    dynamic_example();
    format_example();
    string_example();
    return 0;
}
```
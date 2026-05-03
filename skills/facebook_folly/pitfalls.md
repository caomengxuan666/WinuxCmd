# Pitfalls

### BAD: Using std::string when fbstring would be faster
```cpp
// BAD: std::string may cause unnecessary allocations
std::string result;
for (int i = 0; i < 100000; ++i) {
    result += "data";
}
```

```cpp
// GOOD: fbstring uses small string optimization (SSO) up to 23 chars
folly::fbstring result;
for (int i = 0; i < 100000; ++i) {
    result += "data";
}
```

### BAD: Ignoring folly::Optional null state
```cpp
// BAD: Accessing value without checking
folly::Optional<int> opt;
int val = opt.value();  // Throws if empty
```

```cpp
// GOOD: Always check before access
folly::Optional<int> opt;
if (opt.hasValue()) {
    int val = opt.value();
}
```

### BAD: Using dynamic without type checking
```cpp
// BAD: Assuming type without checking
folly::dynamic d = 42;
std::string s = d.getString();  // Throws if not string
```

```cpp
// GOOD: Check type before access
folly::dynamic d = 42;
if (d.isString()) {
    std::string s = d.getString();
} else if (d.isInt()) {
    int i = d.getInt();
}
```

### BAD: Using folly::to with incompatible types
```cpp
// BAD: May throw or produce unexpected results
auto result = folly::to<std::string>(nullptr);
```

```cpp
// GOOD: Use appropriate types
auto result = folly::to<std::string>(42);
auto result2 = folly::to<std::string>("hello");
```

### BAD: Modifying StringPiece after source destruction
```cpp
// BAD: StringPiece doesn't own memory
folly::StringPiece sp;
{
    std::string temp = "temporary";
    sp = temp;
}
std::cout << sp;  // Undefined behavior - temp destroyed
```

```cpp
// GOOD: Ensure source outlives StringPiece
std::string data = "persistent";
folly::StringPiece sp = data;
std::cout << sp;  // Safe
```

### BAD: Using fbvector with non-trivially copyable types
```cpp
// BAD: fbvector optimized for trivially copyable types
struct Complex {
    std::vector<int> data;
};
folly::fbvector<Complex> vec;  // May not perform as expected
```

```cpp
// GOOD: Use std::vector for complex types
struct Complex {
    std::vector<int> data;
};
std::vector<Complex> vec;
```
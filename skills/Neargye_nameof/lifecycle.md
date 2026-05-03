# Lifecycle

`Neargye/nameof` is a header-only library with no runtime state. All operations produce `std::string_view` or `std::string` values that are constructed from compile-time string literals.

**Construction:**
```cpp
// All nameof operations return by value (string or string_view)
std::string_view name = NAMEOF(somevar); // points to static storage
std::string name_str = nameof::nameof_type<MyType>(); // heap allocated copy
```

**Destruction:**
```cpp
// No special cleanup needed - strings are either static or heap allocated
{
    auto name = NAMEOF_ENUM(my_enum); // string_view to static data
    // name is valid until end of scope (always valid for static data)
}
```

**Move semantics:**
```cpp
// Efficient move for std::string returns
std::string get_name() {
    return std::string(nameof::nameof_type<MyType>());
}

auto name = get_name(); // move or RVO, no extra copies
```

**Resource management:**
```cpp
// No resources to manage for string_view returns
// For string returns, standard RAII applies
class Logger {
    std::string class_name;
public:
    Logger() : class_name(nameof::nameof_short_type<decltype(*this)>()) {}
    // class_name cleaned up automatically in destructor
};
```

**Copy semantics:**
```cpp
// String_views are trivially copyable
auto name1 = NAMEOF(somevar);
auto name2 = name1; // cheap copy of pointer+size

// Strings follow normal copy rules
auto str1 = nameof::nameof_type<int>();
auto str2 = str1; // deep copy
```
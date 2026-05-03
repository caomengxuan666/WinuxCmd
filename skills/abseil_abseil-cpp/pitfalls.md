# Pitfalls

**Pitfall 1: Forgetting to link required dependencies**

BAD:
```cpp
// CMakeLists.txt
target_link_libraries(my_app PRIVATE absl::container)
// This will cause linker errors because absl::container depends on absl::hash
```

GOOD:
```cpp
// CMakeLists.txt
target_link_libraries(my_app PRIVATE absl::container absl::hash)
```

**Pitfall 2: Using InlinedVector::clear() and expecting memory retention**

BAD:
```cpp
absl::InlinedVector<int, 10> vec;
vec.push_back(1);
vec.push_back(2);
vec.clear(); // This deallocates out-of-line memory
vec.push_back(3); // This will reallocate unnecessarily
```

GOOD:
```cpp
absl::InlinedVector<int, 10> vec;
vec.push_back(1);
vec.push_back(2);
vec.clear(); // Consider using vec.assign() or manual resize if you want to avoid reallocation
// Or use a different approach if you need to retain capacity
```

**Pitfall 3: Using SSE/AVX intrinsics on unsupported architectures**

BAD:
```cpp
// Building on ARM with -msse4.1 flag
// This will fail because ARM doesn't support SSE instructions
```

GOOD:
```cpp
// Use CMake to detect architecture and set appropriate flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    # Don't add SSE flags for ARM
else()
    add_compile_options(-msse4.1)
endif()
```

**Pitfall 4: Ignoring StatusOr return values**

BAD:
```cpp
absl::StatusOr<int> result = divide(10, 0);
int value = *result; // This will crash if result is an error
```

GOOD:
```cpp
absl::StatusOr<int> result = divide(10, 0);
if (result.ok()) {
    int value = *result;
    // Use value safely
} else {
    LOG(ERROR) << "Division failed: " << result.status();
}
```

**Pitfall 5: Using absl::Time without proper timezone handling**

BAD:
```cpp
absl::Time t = absl::Now();
std::string formatted = absl::FormatTime(t); // Uses local time, may be ambiguous
```

GOOD:
```cpp
absl::Time t = absl::Now();
std::string formatted = absl::FormatTime(t, absl::UTCTimeZone()); // Explicit UTC
// Or use a specific timezone
std::string formatted_tz = absl::FormatTime(t, absl::FixedTimeZone(-5 * 60 * 60)); // EST
```

**Pitfall 6: Using absl::BitGen without proper seeding**

BAD:
```cpp
absl::BitGen gen; // Default seed may be predictable
int random = absl::Uniform(gen, 1, 100);
```

GOOD:
```cpp
absl::BitGen gen(std::random_device{}()); // Seed with hardware entropy
int random = absl::Uniform(gen, 1, 100);
```

**Pitfall 7: Mixing absl::string_view with temporary strings**

BAD:
```cpp
absl::string_view sv = std::string("temporary"); // sv points to destroyed memory
std::cout << sv; // Undefined behavior
```

GOOD:
```cpp
std::string s = "permanent";
absl::string_view sv = s; // sv points to valid memory
std::cout << sv; // Safe
```

**Pitfall 8: Using absl::flat_hash_map with custom types without hash support**

BAD:
```cpp
struct MyType {
    int x;
    int y;
};
absl::flat_hash_map<MyType, int> map; // Compilation error: no hash for MyType
```

GOOD:
```cpp
struct MyType {
    int x;
    int y;
    template <typename H>
    friend H AbslHashValue(H h, const MyType& m) {
        return H::combine(std::move(h), m.x, m.y);
    }
};
absl::flat_hash_map<MyType, int> map; // Works now
```
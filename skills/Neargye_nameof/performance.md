# Performance

**Performance characteristics:**
- All macro-based operations (`NAMEOF`, `NAMEOF_ENUM`, etc.) are resolved at compile time
- `nameof::nameof_type<T>()` and similar template functions are evaluated at compile time
- Enum name lookup is O(1) for `NAMEOF_ENUM_CONST` and O(n) for `NAMEOF_ENUM` (linear scan within range)
- No dynamic memory allocation for `string_view` returns
- `std::string` returns allocate on heap

**Allocation patterns:**
```cpp
// Zero allocation - returns string_view
auto name = NAMEOF(somevar); // no heap allocation

// Heap allocation - returns std::string
auto name = nameof::nameof_type<MyType>(); // allocates

// Zero allocation - compile-time constant
constexpr auto name = nameof::nameof_short_type<int>(); // "int"
```

**Optimization tips:**
```cpp
// 1. Prefer string_view returns when possible
auto name = NAMEOF(somevar); // string_view, no allocation

// 2. Cache results for repeated use
constexpr auto type_name = nameof::nameof_short_type<MyClass>();
for (int i = 0; i < 1000; ++i) {
    log(type_name); // no repeated computation
}

// 3. Use NAMEOF_ENUM_CONST for hot paths
enum class State { IDLE, BUSY, ERROR };
State s = State::BUSY;
// Fast: compile-time constant
auto name = NAMEOF_ENUM_CONST(State::BUSY);
// Slower: runtime lookup
auto name2 = NAMEOF_ENUM(s);

// 4. Avoid NAMEOF_TYPE_RTTI in performance-critical code
// Slow: uses runtime type information
auto name = NAMEOF_TYPE_RTTI(*ptr);
// Fast: compile-time type name
auto name = nameof::nameof_type<decltype(*ptr)>();
```

**Benchmark results (typical):**
- `NAMEOF` macro: ~0 CPU cycles (compile-time constant)
- `NAMEOF_ENUM` lookup: ~10-50 ns (depends on range size)
- `NAMEOF_ENUM_CONST`: ~0 CPU cycles
- `nameof::nameof_type<T>()`: ~0 CPU cycles
- `NAMEOF_TYPE_RTTI`: ~100-200 ns (requires RTTI)
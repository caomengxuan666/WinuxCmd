# Threading

`Neargye/nameof` is fully thread-safe. All operations are either compile-time constants or read-only operations on static data.

**Thread safety guarantees:**
- All `NAMEOF` macros produce compile-time string literals - no shared mutable state
- `nameof::nameof_type<T>()` and similar template functions are stateless
- Enum name lookup tables are generated at compile time and are read-only
- No global mutable state is modified during any operation

**Concurrent access patterns:**
```cpp
// Safe: multiple threads can use NAMEOF simultaneously
#include <thread>
#include <vector>

void worker(int id) {
    // All these are safe to call concurrently
    auto var_name = NAMEOF(id);
    auto type_name = nameof::nameof_short_type<decltype(id)>();
    auto enum_name = NAMEOF_ENUM_CONST(Color::RED);
}

// Safe: concurrent enum name lookup
enum class Status { OK, ERROR, PENDING };
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([]() {
        Status s = Status::OK;
        auto name = NAMEOF_ENUM(s); // thread-safe read-only operation
    });
}
```

**Static storage safety:**
```cpp
// All string_views point to static storage - safe for concurrent reads
constexpr auto name1 = NAMEOF(somevar); // static storage duration
constexpr auto name2 = nameof::nameof_short_type<int>(); // static storage

// Safe to pass string_views between threads
auto get_name() -> std::string_view {
    return NAMEOF(somevar); // returns view to static data
}

void thread_func() {
    auto name = get_name(); // safe - no data race
}
```

**What NOT to do:**
```cpp
// BAD: modifying shared string (though nameof doesn't encourage this)
std::string* shared = new std::string(nameof::nameof_type<int>());
// Thread 1: delete shared;
// Thread 2: *shared = "new"; // data race

// GOOD: use string_views which are read-only
std::string_view shared = NAMEOF(somevar);
// All threads can safely read shared
```
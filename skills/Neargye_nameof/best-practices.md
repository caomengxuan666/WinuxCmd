# Best Practices

**1. Prefer NAMEOF_ENUM_CONST for known enum values**
```cpp
// Best practice: use static enum name lookup
enum class LogLevel { DEBUG, INFO, WARN, ERROR };
std::string_view level_name(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return NAMEOF_ENUM_CONST(LogLevel::DEBUG);
        case LogLevel::INFO:  return NAMEOF_ENUM_CONST(LogLevel::INFO);
        case LogLevel::WARN:  return NAMEOF_ENUM_CONST(LogLevel::WARN);
        case LogLevel::ERROR: return NAMEOF_ENUM_CONST(LogLevel::ERROR);
    }
    return "UNKNOWN";
}
```

**2. Use constexpr variables for frequently accessed names**
```cpp
// Best practice: cache names at compile time
constexpr auto type_name = nameof::nameof_short_type<MyClass>();
constexpr auto field_name = NAMEOF_MEMBER(&MyClass::value);

template<typename T>
void log_value(const T& val) {
    constexpr auto tname = nameof::nameof_short_type<T>();
    std::cout << tname << " = " << val << "\n";
}
```

**3. Combine with static_assert for compile-time validation**
```cpp
// Best practice: verify names at compile time
struct Config {
    int timeout;
    std::string host;
};

static_assert(NAMEOF_MEMBER(&Config::timeout) == std::string_view("timeout"));
static_assert(NAMEOF_MEMBER(&Config::host) == std::string_view("host"));
```

**4. Use NAMEOF_FULL for template function names**
```cpp
// Best practice: get full template specialization name
template<typename T>
T add(T a, T b) { return a + b; }

auto result = add(1.5, 2.5);
std::cout << NAMEOF_FULL(add(1.5, 2.5)); // "add" (full name may include <double>)
```

**5. Configure enum range globally for your project**
```cpp
// Best practice: set enum range once in a common header
// common_defines.h
#define NAMEOF_ENUM_RANGE_MIN 0
#define NAMEOF_ENUM_RANGE_MAX 512

// Then include this before nameof.hpp
#include "common_defines.h"
#include <nameof.hpp>
```
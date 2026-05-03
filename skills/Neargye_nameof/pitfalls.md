# Pitfalls

**1. Using NAMEOF_ENUM with out-of-range values**
```cpp
// BAD: enum value outside default range [0, 256)
enum class Color { RED = 1000 };
Color c = Color::RED;
std::cout << NAMEOF_ENUM(c); // undefined behavior or empty string

// GOOD: configure range or use NAMEOF_ENUM_CONST
#define NAMEOF_ENUM_RANGE_MIN 0
#define NAMEOF_ENUM_RANGE_MAX 2000
std::cout << NAMEOF_ENUM(c); // "RED"
```

**2. Passing temporaries to NAMEOF_TYPE_EXPR**
```cpp
// BAD: temporary reference may dangle
auto& name = NAMEOF_TYPE_EXPR(std::string("hello")); // UB

// GOOD: use a named variable
std::string s = "hello";
auto& name = NAMEOF_TYPE_EXPR(s); // "std::string"
```

**3. Confusing NAMEOF with NAMEOF_RAW**
```cpp
// BAD: expects raw expression
std::cout << NAMEOF(somevar + 1); // may not compile or give unexpected result

// GOOD: use NAMEOF_RAW for expressions
std::cout << NAMEOF_RAW(somevar + 1); // "somevar + 1"
```

**4. Using NAMEOF_ENUM with enum class having non-contiguous values**
```cpp
// BAD: sparse enum values waste memory and may fail
enum class Flags { A = 1, B = 256, C = 65536 };
Flags f = Flags::B;
std::cout << NAMEOF_ENUM(f); // may not find "B"

// GOOD: use NAMEOF_ENUM_CONST for individual values
std::cout << NAMEOF_ENUM_CONST(Flags::B); // "B"
```

**5. Forgetting to include nameof.hpp in all translation units**
```cpp
// BAD: missing include in some .cpp files
// file1.cpp uses NAMEOF(somevar) but doesn't #include <nameof.hpp>

// GOOD: always include the header
#include <nameof.hpp>
```

**6. Using NAMEOF_MEMBER with non-static data member pointers incorrectly**
```cpp
// BAD: trying to get name of member function
struct S { void func() {} };
std::cout << NAMEOF_MEMBER(&S::func); // compile error

// GOOD: only for data members
struct S { int data; };
std::cout << NAMEOF_MEMBER(&S::data); // "data"
```
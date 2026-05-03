# Quickstart

```cpp
#include <nameof.hpp>
#include <iostream>

// Basic variable name
int somevar = 42;
std::cout << NAMEOF(somevar) << "\n"; // "somevar"

// Member variable name
struct Person { std::string name; int age; };
Person p;
std::cout << NAMEOF(p.age) << "\n"; // "age"

// Function name
int foo(int x) { return x; }
std::cout << NAMEOF(foo(0)) << "\n"; // "foo"

// Enum value name
enum class Color { RED, GREEN, BLUE };
Color c = Color::GREEN;
std::cout << NAMEOF_ENUM(c) << "\n"; // "GREEN"

// Type name
using MyType = std::vector<int>;
std::cout << nameof::nameof_type<MyType>() << "\n"; // "std::vector<int>"

// Short type name
std::cout << nameof::nameof_short_type<MyType>() << "\n"; // "vector"

// Member pointer name
struct A { int field; };
std::cout << NAMEOF_MEMBER(&A::field) << "\n"; // "field"

// Macro name
std::cout << NAMEOF(__LINE__) << "\n"; // "__LINE__"
```
# Safety

**1. NEVER pass null pointers to NAMEOF_POINTER**
```cpp
// BAD: null pointer dereference
int* ptr = nullptr;
std::cout << NAMEOF_POINTER(ptr); // UB

// GOOD: check for null first
if (ptr) {
    std::cout << NAMEOF_POINTER(ptr);
}
```

**2. NEVER use NAMEOF_ENUM with values outside the configured range**
```cpp
// BAD: out-of-range enum value
enum class E { X = 9999 };
E e = E::X;
std::cout << NAMEOF_ENUM(e); // static_assert or UB

// GOOD: use NAMEOF_ENUM_CONST for known values
std::cout << NAMEOF_ENUM_CONST(E::X); // "X"
```

**3. NEVER rely on NAMEOF for runtime-generated names**
```cpp
// BAD: trying to get name of runtime string
std::string runtime_name = "foo";
std::cout << NAMEOF(runtime_name); // prints "runtime_name", not "foo"

// GOOD: use NAMEOF only for compile-time known identifiers
int foo = 42;
std::cout << NAMEOF(foo); // "foo"
```

**4. NEVER modify the string returned by NAMEOF macros**
```cpp
// BAD: modifying string literal is UB
char* name = const_cast<char*>(NAMEOF(somevar));
name[0] = 'X'; // UB

// GOOD: copy if you need to modify
std::string name = NAMEOF(somevar);
name[0] = 'X'; // safe
```

**5. NEVER use NAMEOF_TYPE_RTTI without RTTI enabled**
```cpp
// BAD: RTTI disabled, NAMEOF_TYPE_RTTI returns empty or crashes
// Compile with -fno-rtti or /GR-
std::cout << NAMEOF_TYPE_RTTI(*ptr); // undefined

// GOOD: use compile-time type name instead
std::cout << nameof::nameof_type<decltype(*ptr)>(); // safe
```
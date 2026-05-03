# Safety

**Red-line 1: NEVER use REQUIRE inside a function called by CHECK**

This causes undefined behavior in error reporting and can crash the test runner.

```cpp
// BAD: REQUIRE inside CHECK-called function
void validate(int i) {
    REQUIRE(i > 0);  // CRASH RISK
}

TEST_CASE("dangerous") {
    CHECK(validate(-1));  // Undefined behavior
}

// GOOD: Use CHECK or return error codes
bool validate_safe(int i) {
    return i > 0;
}

TEST_CASE("safe") {
    CHECK(validate_safe(-1));  // Safe, continues test
}
```

**Red-line 2: NEVER create empty test cases with runtime error checks enabled**

This causes application crashes at runtime.

```cpp
// BAD: Empty test with /RTC1 /RTCc
TEST_CASE("empty") {
    // CRASH with runtime checks
}

// GOOD: Always include assertions
TEST_CASE("not empty") {
    REQUIRE(true);
}
```

**Red-line 3: NEVER use CATCH_CONFIG_MAIN with Catch2 v3**

This macro is removed in v3 and will cause compilation errors.

```cpp
// BAD: v2 style with v3
#define CATCH_CONFIG_MAIN  // COMPILATION ERROR in v3
#include <catch2/catch_test_macros.hpp>

// GOOD: Link with catch2 library
#include <catch2/catch_test_macros.hpp>
// Compile with: g++ test.cpp -lCatch2Main -lCatch2
```

**Red-line 4: NEVER assume benchmarks run without the [!benchmark] tag**

Benchmarks silently do nothing without the required tag.

```cpp
// BAD: Missing benchmark tag
TEST_CASE("benchmark test") {
    BENCHMARK("sort") {
        // Never runs!
    };
}

// GOOD: Include required tag
TEST_CASE("benchmark test", "[!benchmark]") {
    BENCHMARK("sort") {
        // Runs when tag is specified
    };
}
```

**Red-line 5: NEVER use CHECK with side effects that modify shared state**

CHECK continues after failure, so side effects may execute unexpectedly.

```cpp
// BAD: Side effects in CHECK
int counter = 0;
TEST_CASE("side effects") {
    CHECK(counter++ == 0);  // counter increments even if first CHECK fails
    CHECK(counter++ == 1);  // May not execute if first fails
}

// GOOD: Separate assertions from side effects
int counter = 0;
TEST_CASE("no side effects") {
    int before = counter;
    counter++;
    REQUIRE(before == 0);
    REQUIRE(counter == 1);
}
```
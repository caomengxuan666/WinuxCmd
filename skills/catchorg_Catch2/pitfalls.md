# Pitfalls

**Pitfall 1: Using REQUIRE inside a CHECK assertion**

When a CHECK assertion calls a function that contains a REQUIRE, the error reporting becomes misleading. The CHECK continues after failure, but the internal REQUIRE aborts the function.

```cpp
// BAD: REQUIRE inside function called by CHECK
int foo(int i) {
    REQUIRE(i > 10);  // This will abort foo() but CHECK continues
    return 42;
}

TEST_CASE("Bad pattern", "[pitfall]") {
    CHECK(foo(2) == 2);  // Misleading error message
}

// GOOD: Use CHECK inside the function or handle errors explicitly
int foo_safe(int i) {
    if (i <= 10) return -1;  // Return error value
    return 42;
}

TEST_CASE("Good pattern", "[pitfall]") {
    CHECK(foo_safe(2) == 2);  // Clear failure
}
```

**Pitfall 2: Not handling special characters in test names**

Test names with special characters (commas, quotes) can't be easily filtered from command line.

```cpp
// BAD: Special characters in test name
TEST_CASE("Test with special, characters \"in name", "[cli][regression]") {
    SUCCEED();
}
// Can't run with: --filter "Test with special, characters \"in name"

// GOOD: Use simple names or escape properly
TEST_CASE("Test with special characters in name", "[cli][regression]") {
    SUCCEED();
}
// Can run with: --filter "Test with special characters in name"
```

**Pitfall 3: Empty test cases with /RTC1 /RTCc compiler flags**

Empty test cases can crash when compiled with runtime error checks enabled.

```cpp
// BAD: Empty test case
TEST_CASE("Empty test") {
    // No assertions
}

// GOOD: Always have at least one assertion
TEST_CASE("Non-empty test") {
    REQUIRE(true);  // Minimal assertion
}
```

**Pitfall 4: Forgetting to run benchmarks explicitly**

Benchmarks are not run by default and require the `[!benchmark]` tag.

```cpp
// BAD: Benchmark without tag
TEST_CASE("Benchmark sort") {
    BENCHMARK("sort") {
        // ...
    };
}

// GOOD: Benchmark with required tag
TEST_CASE("Benchmark sort", "[!benchmark]") {
    BENCHMARK("sort") {
        // ...
    };
}
```

**Pitfall 5: Using Catch2 v2's single-header style with v3**

Catch2 v3 is no longer single-header and requires proper linking.

```cpp
// BAD: v2 style with v3
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>  // Wrong for v3

// GOOD: v3 style
#include <catch2/catch_test_macros.hpp>
// Link with catch2 library instead of CATCH_CONFIG_MAIN
```

**Pitfall 6: Not stringifying custom types for assertion output**

Custom types need explicit stringification for readable failure messages.

```cpp
// BAD: Custom type without stringification
struct Point { int x, y; };

TEST_CASE("Point comparison", "[pitfall]") {
    Point p1{1, 2}, p2{3, 4};
    REQUIRE(p1.x == p2.x);  // Unreadable failure message
}

// GOOD: Provide StringMaker specialization
namespace Catch {
    template<>
    struct StringMaker<Point> {
        static std::string convert(Point const& p) {
            return "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")";
        }
    };
}
```

**Pitfall 7: Using sections incorrectly for cleanup**

Sections don't automatically clean up resources between runs.

```cpp
// BAD: Resource leak between sections
TEST_CASE("Resource management", "[pitfall]") {
    int* ptr = new int(42);
    
    SECTION("first") {
        REQUIRE(*ptr == 42);
    }
    
    SECTION("second") {
        // ptr still points to same memory
        REQUIRE(*ptr == 42);
    }
    delete ptr;  // Only runs once, not per section
}

// GOOD: Use RAII or explicit cleanup
TEST_CASE("Resource management fixed", "[pitfall]") {
    auto ptr = std::make_unique<int>(42);
    
    SECTION("first") {
        REQUIRE(*ptr == 42);
    }
    
    SECTION("second") {
        REQUIRE(*ptr == 42);
    }
    // unique_ptr cleans up automatically
}
```
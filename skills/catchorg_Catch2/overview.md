# Overview

Catch2 is a modern C++ unit testing framework that also provides micro-benchmarking and BDD-style macros. Its main advantage is natural syntax: test names don't need to be valid identifiers, assertions look like normal C++ boolean expressions, and sections provide local setup/teardown sharing.

**When to use Catch2:**
- Unit testing C++ code with natural, readable assertions
- Micro-benchmarking specific functions or algorithms
- BDD-style testing with GIVEN/WHEN/THEN scenarios
- Projects that need header-only (v2) or library-based (v3) testing

**When not to use Catch2:**
- When you need strict test isolation without shared state (use Google Test)
- For large-scale performance benchmarking (use Google Benchmark)
- When you need minimal binary size (Catch2 adds overhead)

**Key design features:**
- Assertions are expressions, not macros with special syntax
- Sections allow nested setup/teardown without fixtures
- Tags enable flexible test selection and grouping
- BDD macros (SCENARIO, GIVEN, WHEN, THEN) for behavioral tests
- Built-in micro-benchmarking with statistical analysis

```cpp
// Natural assertion syntax
TEST_CASE("Natural assertions", "[example]") {
    int a = 5, b = 10;
    REQUIRE(a < b);
    CHECK(a + b == 15);
}

// Sections for shared state
TEST_CASE("Sections for setup", "[example]") {
    std::string str = "hello";
    
    SECTION("length") {
        REQUIRE(str.length() == 5);
    }
    
    SECTION("uppercase") {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        REQUIRE(str == "HELLO");
    }
}
```
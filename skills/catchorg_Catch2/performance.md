# Performance

**Performance characteristics**

Catch2 adds minimal overhead to test execution. The main performance considerations are test registration (static initialization) and assertion evaluation.

```cpp
// Test registration happens at static init time
// This is fast but can add to startup time for many tests
TEST_CASE("Performance test", "[perf]") {
    // Assertion overhead is minimal
    REQUIRE(1 + 1 == 2);  // Fast: simple integer comparison
    
    // Complex expressions may have overhead
    std::vector<int> large_vec(1000000);
    REQUIRE(std::is_sorted(large_vec.begin(), large_vec.end()));  // Slower
}
```

**Allocation patterns**

Catch2 allocates memory for test names, tags, and assertion results. Minimize allocations in hot paths.

```cpp
// BAD: Allocations in assertions
TEST_CASE("Allocation heavy", "[perf]") {
    std::string long_name = "very_long_test_name_that_requires_allocation";
    REQUIRE(long_name.length() > 10);  // String allocation
}

// GOOD: Use literals when possible
TEST_CASE("Allocation light", "[perf]") {
    const char* short_name = "short";
    REQUIRE(strlen(short_name) > 0);  // No allocation
}

// BAD: Creating large objects in assertions
TEST_CASE("Large object creation", "[perf]") {
    REQUIRE(create_large_object().size() == 1000000);  // Creates and destroys large object
}

// GOOD: Create once, assert multiple times
TEST_CASE("Efficient assertions", "[perf]") {
    auto obj = create_large_object();
    REQUIRE(obj.size() == 1000000);
    REQUIRE(obj.is_valid());
}
```

**Optimization tips**

Use Catch2's features to optimize test performance.

```cpp
// Use CHECK instead of REQUIRE for non-critical assertions
// CHECK continues after failure, saving test restart time
TEST_CASE("Optimize with CHECK", "[perf]") {
    CHECK(condition1());  // Continues even if fails
    CHECK(condition2());
    CHECK(condition3());
}

// Use sections to avoid redundant setup
TEST_CASE("Avoid redundant setup", "[perf]") {
    auto expensive_resource = create_expensive_resource();
    
    SECTION("test aspect 1") {
        REQUIRE(expensive_resource.aspect1());
    }
    
    SECTION("test aspect 2") {
        REQUIRE(expensive_resource.aspect2());
    }
    // Resource created once, not per section
}

// Use GENERATE for data-driven tests efficiently
TEST_CASE("Efficient parameterized", "[perf]") {
    auto data = GENERATE(range(0, 100));
    // Generator creates values on demand
    REQUIRE(data >= 0);
    REQUIRE(data < 100);
}
```

**Benchmarking considerations**

Catch2's benchmarking has specific performance characteristics.

```cpp
// Benchmarks run multiple iterations for statistical accuracy
TEST_CASE("Benchmark considerations", "[!benchmark][perf]") {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);
    
    BENCHMARK("sort benchmark") {
        std::vector<int> copy = data;  // Copy overhead included
        std::sort(copy.begin(), copy.end());
        return copy;
    };
    
    // For more accurate benchmarks, minimize setup in the benchmark lambda
    BENCHMARK("sort benchmark optimized") {
        std::vector<int> copy(data);  // Pre-allocate
        std::sort(copy.begin(), copy.end());
        return copy;
    };
}
```
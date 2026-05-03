# Quickstart

```cpp
// Quickstart: Common Catch2 patterns

// 1. Basic test case with REQUIRE
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Basic arithmetic", "[math]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(2 * 3 == 6);
}

// 2. Using sections for shared setup/teardown
TEST_CASE("Sections example", "[sections]") {
    int value = 42;
    
    SECTION("value is positive") {
        REQUIRE(value > 0);
    }
    
    SECTION("value is 42") {
        REQUIRE(value == 42);
    }
}

// 3. CHECK vs REQUIRE (CHECK continues on failure)
TEST_CASE("CHECK continues after failure", "[check]") {
    CHECK(1 == 2);  // This fails but test continues
    CHECK(3 == 3);  // This still runs
    REQUIRE(1 == 1); // REQUIRE stops test if fails
}

// 4. BDD-style with GIVEN/WHEN/THEN
#include <catch2/catch_test_macros.hpp>

SCENARIO("Vector operations", "[vector][bdd]") {
    GIVEN("A vector with elements") {
        std::vector<int> v = {1, 2, 3};
        
        WHEN("We add an element") {
            v.push_back(4);
            
            THEN("The size increases") {
                REQUIRE(v.size() == 4);
            }
        }
    }
}

// 5. Benchmarking
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("Benchmark sorting", "[!benchmark]") {
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 0);
    
    BENCHMARK("std::sort") {
        std::vector<int> copy = data;
        std::sort(copy.begin(), copy.end());
        return copy;
    };
}

// 6. Tag-based test filtering
TEST_CASE("Network tests", "[network][integration]") {
    // Only runs with --filter-tags=[network]
    REQUIRE(true);
}

// 7. Using GENERATE for parameterized tests
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("Parameterized with GENERATE", "[generate]") {
    auto value = GENERATE(1, 2, 3, 5, 7, 11);
    REQUIRE(value > 0);
    REQUIRE(value < 20);
}

// 8. Custom stringification for types
#include <catch2/catch_test_macros.hpp>

struct Point { int x, y; };

namespace Catch {
    template<>
    struct StringMaker<Point> {
        static std::string convert(Point const& p) {
            return "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")";
        }
    };
}

TEST_CASE("Custom type stringification", "[custom]") {
    Point p{3, 4};
    REQUIRE(p.x == 3);
    // Catch2 will print Point nicely on failure
}
```
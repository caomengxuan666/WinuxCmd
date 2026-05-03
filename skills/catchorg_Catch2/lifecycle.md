# Lifecycle

**Construction and initialization**

Catch2 tests are constructed when the test binary starts. Each TEST_CASE creates a test case object that registers itself with the test runner.

```cpp
// Test cases are registered at static initialization time
#include <catch2/catch_test_macros.hpp>

// This test case is constructed before main() runs
TEST_CASE("Static registration", "[lifecycle]") {
    // Test body runs when selected by test runner
    REQUIRE(true);
}

// Manual test case creation (rarely needed)
#include <catch2/catch_test_case_info.hpp>

// Tests are typically auto-registered via TEST_CASE macro
```

**Destruction and cleanup**

Tests are destroyed after execution. Resources allocated in sections must be managed carefully.

```cpp
// BAD: Manual resource management
TEST_CASE("Manual cleanup", "[lifecycle]") {
    int* data = new int[100];
    
    SECTION("process data") {
        // Use data
        REQUIRE(data[0] == 0);
    }
    // Memory leak! delete[] never runs for each section
    delete[] data;  // Only runs once
}

// GOOD: RAII resource management
TEST_CASE("RAII cleanup", "[lifecycle]") {
    std::vector<int> data(100);
    
    SECTION("process data") {
        REQUIRE(data[0] == 0);
    }
    // vector destructor runs after each section
}
```

**Move semantics and resource transfer**

Catch2 supports move semantics in assertions and test data.

```cpp
// Move semantics in tests
TEST_CASE("Move semantics", "[lifecycle]") {
    std::vector<int> source = {1, 2, 3};
    std::vector<int> dest = std::move(source);
    
    REQUIRE(dest.size() == 3);
    REQUIRE(source.empty());  // source is moved-from state
}

// Moving test data
struct HeavyResource {
    std::unique_ptr<int[]> data;
    HeavyResource(size_t size) : data(std::make_unique<int[]>(size)) {}
    HeavyResource(HeavyResource&&) = default;
    HeavyResource& operator=(HeavyResource&&) = default;
};

TEST_CASE("Move heavy resource", "[lifecycle]") {
    HeavyResource res1(1000);
    HeavyResource res2 = std::move(res1);
    REQUIRE(res2.data != nullptr);
    REQUIRE(res1.data == nullptr);  // Moved-from state
}
```

**Resource management best practices**

Use Catch2's TEST_CASE and SECTION lifecycle to manage resources properly.

```cpp
// Resource management with sections
TEST_CASE("File operations", "[lifecycle]") {
    // Setup: Create temporary file
    std::ofstream file("test.txt");
    file << "test data";
    file.close();
    
    SECTION("read file") {
        std::ifstream reader("test.txt");
        std::string content;
        reader >> content;
        REQUIRE(content == "test");
    }
    
    SECTION("append to file") {
        std::ofstream appender("test.txt", std::ios::app);
        appender << " more data";
        appender.close();
    }
    
    // Teardown: Remove file (runs after each section)
    std::remove("test.txt");
}
```
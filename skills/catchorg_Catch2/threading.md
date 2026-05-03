# Threading

**Thread safety guarantees**

Catch2's test runner is not thread-safe by default. Tests should not be run concurrently from multiple threads without proper synchronization.

```cpp
// BAD: Concurrent test execution
#include <thread>

TEST_CASE("Thread unsafe", "[threading]") {
    std::thread t1([]() {
        // This test runs concurrently with main thread
        REQUIRE(1 + 1 == 2);  // Undefined behavior
    });
    t1.join();
}

// GOOD: Sequential test execution
TEST_CASE("Thread safe", "[threading]") {
    // Single-threaded test execution
    REQUIRE(1 + 1 == 2);
}
```

**Concurrent access patterns**

When testing thread-safe code, use proper synchronization within test bodies.

```cpp
// Testing thread-safe code
#include <mutex>
#include <thread>

std::mutex mtx;
int shared_counter = 0;

TEST_CASE("Thread-safe test", "[threading]") {
    constexpr int num_threads = 4;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([i]() {
            std::lock_guard<std::mutex> lock(mtx);
            shared_counter++;
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    REQUIRE(shared_counter == num_threads);
}
```

**Shared state between tests**

Catch2 does not guarantee isolation between tests running in different threads. Use test-level synchronization.

```cpp
// BAD: Shared state without synchronization
int global_state = 0;

TEST_CASE("Test A", "[threading]") {
    global_state = 42;
    REQUIRE(global_state == 42);
}

TEST_CASE("Test B", "[threading]") {
    // global_state might be 42 from Test A
    REQUIRE(global_state == 0);  // May fail
}

// GOOD: Isolated state per test
TEST_CASE("Test A fixed", "[threading]") {
    int local_state = 42;
    REQUIRE(local_state == 42);
}

TEST_CASE("Test B fixed", "[threading]") {
    int local_state = 0;
    REQUIRE(local_state == 0);
}
```

**Thread-safe assertion macros**

Catch2's assertion macros are not thread-safe. Use external synchronization when asserting from multiple threads.

```cpp
// BAD: Thread-unsafe assertions
TEST_CASE("Unsafe assertions", "[threading]") {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([]() {
            REQUIRE(1 + 1 == 2);  // Unsafe: concurrent assertion
        });
    }
    for (auto& t : threads) t.join();
}

// GOOD: Synchronized assertions
TEST_CASE("Safe assertions", "[threading]") {
    std::mutex assert_mutex;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&assert_mutex]() {
            std::lock_guard<std::mutex> lock(assert_mutex);
            REQUIRE(1 + 1 == 2);  // Safe: synchronized assertion
        });
    }
    for (auto& t : threads) t.join();
}
```
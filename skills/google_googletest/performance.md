# Performance

**Test discovery overhead**
```cpp
// BAD: Many small test suites cause overhead
TEST(A, Test1) { /* ... */ }
TEST(B, Test2) { /* ... */ }
TEST(C, Test3) { /* ... */ }

// GOOD: Group related tests in same suite
TEST(GroupA, Test1) { /* ... */ }
TEST(GroupA, Test2) { /* ... */ }
TEST(GroupA, Test3) { /* ... */ }
```

**Expensive setup should be shared**
```cpp
// BAD: Expensive setup repeated for each test
class ExpensiveTest : public ::testing::Test {
    void SetUp() override {
        database_.connect("large_db"); // Takes 5 seconds
    }
};

// GOOD: Share expensive setup across tests
class EfficientTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        shared_db_.connect("large_db");
    }
    
    static Database shared_db_;
};

Database EfficientTest::shared_db_;
```

**Use gtest-parallel for faster execution**
```bash
# BAD: Sequential execution
./my_test

# GOOD: Parallel execution
gtest-parallel ./my_test --workers=4
```

**Optimize death tests**
```cpp
// BAD: Slow death test with complex setup
TEST(DeathTest, Slow) {
    ASSERT_DEATH({
        ComplexSetup();
        crash();
    }, "");
}

// GOOD: Fast death test with minimal setup
TEST(DeathTest, Fast) {
    ASSERT_DEATH({
        crash();
    }, "");
}
```

**Use --gtest_filter to run specific tests**
```bash
# BAD: Running all tests when only one is needed
./my_test

# GOOD: Filter to specific test
./my_test --gtest_filter="MySuite.MyTest"
```
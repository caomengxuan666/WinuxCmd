# Safety

**Red Line 1: Never use ASSERT_* in functions called from multiple tests**
```cpp
// BAD: ASSERT_TRUE in helper function
void CheckValue(int x) {
    ASSERT_TRUE(x > 0); // Will abort the calling test
}
TEST(Suite, Test1) { CheckValue(5); }
TEST(Suite, Test2) { CheckValue(-1); } // Aborts Test2, but Test1 still passes

// GOOD: Return bool and use EXPECT_* in test
bool CheckValue(int x) {
    return x > 0;
}
TEST(Suite, Test1) { EXPECT_TRUE(CheckValue(5)); }
TEST(Suite, Test2) { EXPECT_TRUE(CheckValue(-1)); }
```

**Red Line 2: Never modify global state in SetUp/TearDown without cleanup**
```cpp
// BAD: Global state leaks between tests
static int global_flag = 0;
class BadFixture : public ::testing::Test {
    void SetUp() override { global_flag = 1; }
};

// GOOD: Reset global state in both SetUp and TearDown
class GoodFixture : public ::testing::Test {
    void SetUp() override { 
        old_flag = global_flag;
        global_flag = 1; 
    }
    void TearDown() override { global_flag = old_flag; }
    int old_flag;
};
```

**Red Line 3: Never use EXPECT_* in constructors**
```cpp
// BAD: EXPECT_* in constructor
class BadFixture : public ::testing::Test {
    BadFixture() {
        EXPECT_TRUE(condition); // Undefined behavior
    }
};

// GOOD: Use SetUp for assertions
class GoodFixture : public ::testing::Test {
    void SetUp() override {
        EXPECT_TRUE(condition);
    }
};
```

**Red Line 4: Never run death tests with --gtest_death_test_style=fast on valgrind**
```cpp
// BAD: Fast death tests don't work with valgrind
// Run: valgrind ./test --gtest_death_test_style=fast

// GOOD: Use threadsafe death tests with valgrind
// Run: valgrind ./test --gtest_death_test_style=threadsafe
```

**Red Line 5: Never use ASSERT_* in SetUp/TearDown without understanding consequences**
```cpp
// BAD: ASSERT_* in SetUp aborts all subsequent tests
class BadFixture : public ::testing::Test {
    void SetUp() override {
        ASSERT_TRUE(initialize()); // If fails, all TEST_F in this fixture are skipped
    }
};

// GOOD: Use EXPECT_* and handle failure gracefully
class GoodFixture : public ::testing::Test {
    void SetUp() override {
        if (!initialize()) {
            GTEST_SKIP() << "Skipping due to initialization failure";
        }
    }
};
```
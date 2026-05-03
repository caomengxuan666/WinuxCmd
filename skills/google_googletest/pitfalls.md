# Pitfalls

**Pitfall 1: Using ASSERT_* in non-void functions**
```cpp
// BAD: ASSERT_* returns void, can't use in value-returning function
int GetValue() {
    ASSERT_TRUE(condition); // Compilation error
    return 42;
}

// GOOD: Use EXPECT_* or restructure
int GetValue() {
    EXPECT_TRUE(condition);
    return 42;
}
```

**Pitfall 2: Forgetting to register type-parameterized tests**
```cpp
// BAD: Missing REGISTER_TYPED_TEST_SUITE_P
template <typename T>
class MyTest : public ::testing::Test {};
TYPED_TEST_SUITE_P(MyTest);
TYPED_TEST_P(MyTest, Works) { /* ... */ }
// Missing: REGISTER_TYPED_TEST_SUITE_P(MyTest, Works);

// GOOD: Always register
TYPED_TEST_SUITE_P(MyTest);
TYPED_TEST_P(MyTest, Works) { /* ... */ }
REGISTER_TYPED_TEST_SUITE_P(MyTest, Works);
```

**Pitfall 3: Using EXPECT_* in destructors**
```cpp
// BAD: EXPECT_* in destructor causes undefined behavior
class BadFixture : public ::testing::Test {
    ~BadFixture() {
        EXPECT_TRUE(cleanup_success); // Undefined behavior
    }
};

// GOOD: Use explicit cleanup methods
class GoodFixture : public ::testing::Test {
    void TearDown() override {
        EXPECT_TRUE(cleanup_success);
    }
};
```

**Pitfall 4: Sharing mutable state between tests**
```cpp
// BAD: Static state persists between tests
static int counter = 0;
TEST(Suite, Test1) { counter++; }
TEST(Suite, Test2) { EXPECT_EQ(counter, 1); } // Fails if Test2 runs first

// GOOD: Reset state in SetUp
class CounterTest : public ::testing::Test {
    void SetUp() override { counter = 0; }
    static int counter;
};
```

**Pitfall 5: Ignoring test order dependencies**
```cpp
// BAD: Tests depend on each other
TEST(Suite, CreateUser) { /* creates user with ID 1 */ }
TEST(Suite, DeleteUser) { /* deletes user with ID 1 - fails if CreateUser didn't run */ }

// GOOD: Each test is independent
TEST(Suite, CreateAndDeleteUser) {
    CreateUser(1);
    DeleteUser(1);
}
```

**Pitfall 6: Using ASSERT_DEATH with complex expressions**
```cpp
// BAD: Complex expression in death test
ASSERT_DEATH({
    auto result = someFunction();
    if (result) { crash(); }
}, "");

// GOOD: Simple crash in death test
ASSERT_DEATH({
    crash();
}, "");
```
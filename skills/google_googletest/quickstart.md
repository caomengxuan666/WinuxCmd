# Quickstart

```cpp
// Basic test structure
#include <gtest/gtest.h>

// Simple test
TEST(MyTestSuite, BasicAssertions) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

// Test with setup/teardown
class MyTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        value = 42;
    }
    
    int value;
};

TEST_F(MyTestFixture, UsesFixture) {
    EXPECT_EQ(value, 42);
}

// Value-parameterized tests
class ParamTest : public ::testing::TestWithParam<int> {};
TEST_P(ParamTest, ChecksValue) {
    int param = GetParam();
    EXPECT_GT(param, 0);
}
INSTANTIATE_TEST_SUITE_P(Default, ParamTest, ::testing::Values(1, 2, 3));

// Death test
TEST(DeathTest, DiesOnNull) {
    ASSERT_DEATH({
        int* ptr = nullptr;
        *ptr = 42; // crash
    }, "");
}

// Test with custom message
TEST(MyTestSuite, CustomMessage) {
    int actual = 5;
    int expected = 10;
    ASSERT_EQ(actual, expected) << "Expected " << expected << " but got " << actual;
}

// Disabled test
TEST(MyTestSuite, DISABLED_WillNotRun) {
    FAIL() << "This test is disabled";
}

// Type-parameterized tests
template <typename T>
class TypeParamTest : public ::testing::Test {};
TYPED_TEST_SUITE_P(TypeParamTest);
TYPED_TEST_P(TypeParamTest, WorksForType) {
    TypeParam value = TypeParam();
    SUCCEED();
}
REGISTER_TYPED_TEST_SUITE_P(TypeParamTest, WorksForType);
using MyTypes = ::testing::Types<int, double, std::string>;
INSTANTIATE_TYPED_TEST_SUITE_P(My, TypeParamTest, MyTypes);
```
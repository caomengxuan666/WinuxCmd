# Best Practices

**Use descriptive test names and suite organization**
```cpp
// GOOD: Descriptive names
TEST(MathOperations, AdditionWithPositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);
}

TEST(MathOperations, AdditionWithNegativeNumbers) {
    EXPECT_EQ(add(-2, -3), -5);
}
```

**Use test fixtures for shared setup**
```cpp
class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_.connect("test.db");
        db_.createTable("users");
    }
    
    void TearDown() override {
        db_.dropTable("users");
        db_.disconnect();
    }
    
    Database db_;
};

TEST_F(DatabaseTest, InsertUser) {
    db_.insertUser("Alice");
    EXPECT_TRUE(db_.userExists("Alice"));
}
```

**Use SCOPED_TRACE for complex assertions**
```cpp
TEST(ComplexTest, MultipleConditions) {
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Iteration " + std::to_string(i));
        EXPECT_EQ(process(i), expected[i]);
    }
}
```

**Use EXPECT_* for non-critical checks, ASSERT_* for critical ones**
```cpp
TEST(FileProcessing, ReadAndParse) {
    File file("data.txt");
    ASSERT_TRUE(file.open()) << "Failed to open file"; // Critical
    
    std::string content = file.read();
    ASSERT_FALSE(content.empty()) << "File is empty"; // Critical
    
    auto parsed = parse(content);
    EXPECT_EQ(parsed.size(), 10); // Non-critical
    EXPECT_EQ(parsed[0].name, "item1"); // Non-critical
}
```

**Use value-parameterized tests for data-driven testing**
```cpp
class SortTest : public ::testing::TestWithParam<std::vector<int>> {};
TEST_P(SortTest, SortsCorrectly) {
    auto input = GetParam();
    auto expected = input;
    std::sort(expected.begin(), expected.end());
    my_sort(input);
    EXPECT_EQ(input, expected);
}

INSTANTIATE_TEST_SUITE_P(
    VariousInputs,
    SortTest,
    ::testing::Values(
        std::vector<int>{3, 1, 2},
        std::vector<int>{5, 5, 5},
        std::vector<int>{1}
    )
);
```
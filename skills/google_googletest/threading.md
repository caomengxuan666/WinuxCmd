# Threading

**GoogleTest is not thread-safe by default**
```cpp
// BAD: Concurrent access to test state
TEST(ThreadTest, Unsafe) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([this]() {
            EXPECT_TRUE(condition); // Unsafe: concurrent EXPECT_*
        });
    }
    for (auto& t : threads) t.join();
}

// GOOD: Sequential assertions
TEST(ThreadTest, Safe) {
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(condition);
    }
}
```

**Use --gtest_filter with sharding for parallel test execution**
```bash
# GOOD: Shard tests across processes
./my_test --gtest_shard_index=0 --gtest_total_shards=4
./my_test --gtest_shard_index=1 --gtest_total_shards=4
./my_test --gtest_shard_index=2 --gtest_total_shards=4
./my_test --gtest_shard_index=3 --gtest_total_shards=4
```

**Thread-safe test fixtures**
```cpp
class ThreadSafeFixture : public ::testing::Test {
protected:
    void SetUp() override {
        mutex_.lock();
        shared_data_ = 0;
        mutex_.unlock();
    }
    
    std::mutex mutex_;
    int shared_data_;
};

// Each TEST_F runs sequentially, so no thread safety issues
TEST_F(ThreadSafeFixture, Test1) {
    shared_data_ = 42;
    EXPECT_EQ(shared_data_, 42);
}
```

**Death tests and threading**
```cpp
// BAD: Death test with threads
TEST(DeathTest, WithThreads) {
    ASSERT_DEATH({
        std::thread t([]{ /* ... */ });
        crash(); // Thread may not be joined
    }, "");
}

// GOOD: Death test without threads
TEST(DeathTest, WithoutThreads) {
    ASSERT_DEATH({
        crash();
    }, "");
}
```

**Use --gtest_death_test_style=threadsafe for thread-safe death tests**
```bash
# GOOD: Thread-safe death tests
./my_test --gtest_death_test_style=threadsafe
```
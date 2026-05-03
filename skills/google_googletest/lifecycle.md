# Lifecycle

**Test construction and destruction order**
```cpp
class LifecycleTest : public ::testing::Test {
protected:
    LifecycleTest() {
        std::cout << "Constructor" << std::endl;
    }
    
    ~LifecycleTest() override {
        std::cout << "Destructor" << std::endl;
    }
    
    void SetUp() override {
        std::cout << "SetUp" << std::endl;
    }
    
    void TearDown() override {
        std::cout << "TearDown" << std::endl;
    }
};

// Order: Constructor -> SetUp -> Test Body -> TearDown -> Destructor
TEST_F(LifecycleTest, CheckOrder) {
    std::cout << "Test Body" << std::endl;
}
```

**Resource management with SetUp/TearDown**
```cpp
class ResourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        resource_ = new Resource();
        ASSERT_NE(resource_, nullptr) << "Failed to allocate resource";
    }
    
    void TearDown() override {
        delete resource_;
        resource_ = nullptr;
    }
    
    Resource* resource_;
};

// GOOD: Use RAII instead of manual management
class RAIIResourceTest : public ::testing::Test {
protected:
    std::unique_ptr<Resource> resource_ = std::make_unique<Resource>();
};
```

**Move semantics in test fixtures**
```cpp
class MoveTest : public ::testing::Test {
protected:
    std::vector<int> data_;
    
    void SetUp() override {
        data_ = {1, 2, 3, 4, 5};
    }
};

TEST_F(MoveTest, MoveSemantics) {
    auto moved = std::move(data_);
    EXPECT_TRUE(data_.empty());
    EXPECT_EQ(moved.size(), 5);
}
```

**Shared resources across tests**
```cpp
class SharedResourceTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        shared_resource_ = new ExpensiveResource();
    }
    
    static void TearDownTestSuite() {
        delete shared_resource_;
        shared_resource_ = nullptr;
    }
    
    static ExpensiveResource* shared_resource_;
};

ExpensiveResource* SharedResourceTest::shared_resource_ = nullptr;
```
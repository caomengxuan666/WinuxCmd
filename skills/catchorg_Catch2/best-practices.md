# Best Practices

**Best Practice 1: Use sections for shared setup instead of test fixtures**

Sections provide cleaner, more localized setup/teardown than traditional fixtures.

```cpp
// RECOMMENDED: Sections for shared setup
TEST_CASE("Database operations", "[db]") {
    Database db = connect_to_test_db();
    
    SECTION("insert") {
        db.insert("key", "value");
        REQUIRE(db.get("key") == "value");
    }
    
    SECTION("delete") {
        db.insert("key", "value");
        db.delete("key");
        REQUIRE(db.exists("key") == false);
    }
    // db destructor runs after each section
}
```

**Best Practice 2: Use tags for test categorization and filtering**

Tags enable flexible test selection without modifying test names.

```cpp
// RECOMMENDED: Meaningful tags
TEST_CASE("User authentication", "[auth][integration][slow]") {
    // Integration test for authentication
}

TEST_CASE("Password hashing", "[auth][unit][fast]") {
    // Unit test for hashing
}

// Run with: --filter-tags "[auth]" or --filter-tags "[integration]"
```

**Best Practice 3: Use GENERATE for data-driven tests**

GENERATE provides cleaner parameterized tests than manual loops.

```cpp
// RECOMMENDED: GENERATE for multiple inputs
TEST_CASE("String length", "[generate]") {
    auto [input, expected] = GENERATE(
        table<std::string, size_t>({
            {"hello", 5},
            {"", 0},
            {"a", 1},
            {"abcdef", 6}
        })
    );
    REQUIRE(input.length() == expected);
}
```

**Best Practice 4: Provide StringMaker for custom types**

Custom stringification makes failure messages readable.

```cpp
// RECOMMENDED: Custom stringification
struct Color { int r, g, b; };

namespace Catch {
    template<>
    struct StringMaker<Color> {
        static std::string convert(Color const& c) {
            return "RGB(" + std::to_string(c.r) + "," 
                         + std::to_string(c.g) + "," 
                         + std::to_string(c.b) + ")";
        }
    };
}

TEST_CASE("Color comparison", "[custom]") {
    Color c1{255, 0, 0}, c2{0, 255, 0};
    REQUIRE(c1.r == c2.r);  // Prints: RGB(255,0,0) != RGB(0,255,0)
}
```

**Best Practice 5: Use BDD macros for behavioral tests**

SCENARIO/GIVEN/WHEN/THEN improve test readability for complex scenarios.

```cpp
// RECOMMENDED: BDD for behavioral tests
SCENARIO("Shopping cart checkout", "[cart][bdd]") {
    GIVEN("A cart with items") {
        Cart cart;
        cart.add_item("book", 29.99);
        cart.add_item("pen", 5.99);
        
        WHEN("User checks out") {
            double total = cart.checkout();
            
            THEN("Total should include tax") {
                REQUIRE(total > 35.98);  // Items + tax
            }
            
            THEN("Cart should be empty after checkout") {
                REQUIRE(cart.item_count() == 0);
            }
        }
    }
}
```

**Best Practice 6: Use INFO/CAPTURE for debugging failures**

Add context to failures without cluttering test logic.

```cpp
// RECOMMENDED: Contextual information
TEST_CASE("Complex calculation", "[info]") {
    int x = 42, y = 100;
    INFO("Processing values: x=" << x << ", y=" << y);
    
    int result = complex_calculation(x, y);
    CAPTURE(result);  // Prints result on failure
    
    REQUIRE(result > 0);
    REQUIRE(result < 1000);
}
```
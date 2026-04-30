# Performance

### Minimal overhead for simple parsing
```cpp
// CLI11 adds minimal overhead for basic parsing
// For a simple flag check:
bool verbose;
app.add_flag("-v,--verbose", verbose);
// Parsing is O(n) where n is number of arguments

// For large argument lists, parsing is still efficient
std::vector<std::string> files;
app.add_option("files", files)->expected(-1);
// Each argument is processed once
```

### Avoid unnecessary validators
```cpp
// BAD: Expensive validator on every parse
app.add_option("--value", value)
   ->check([](const std::string& s) {
       std::this_thread::sleep_for(std::chrono::seconds(1));
       return "";
   });

// GOOD: Use built-in validators when possible
app.add_option("--value", value)
   ->check(CLI::Range(0, 100));
```

### Use `expected()` for fixed-size arrays
```cpp
// BAD: Vector with unknown size
std::vector<int> coords;
app.add_option("--point", coords)->expected(-1);

// GOOD: Fixed-size array for known count
int coords[3];
app.add_option("--point", coords)->expected(3);
// More efficient than vector allocation
```

### Pre-allocate containers
```cpp
// BAD: Repeated reallocation
std::vector<int> values;
app.add_option("--values", values)->expected(1000);

// GOOD: Pre-allocate if you know the size
std::vector<int> values;
values.reserve(1000);
app.add_option("--values", values)->expected(1000);
```
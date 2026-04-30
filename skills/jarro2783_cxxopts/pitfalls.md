# Pitfalls

### Pitfall 1: Forgetting to use `.as<type>()` to retrieve values
```cpp
// BAD: Missing .as<type>()
auto result = options.parse(argc, argv);
int port = result["port"];  // Compilation error or undefined behavior
```

```cpp
// GOOD: Always use .as<type>()
auto result = options.parse(argc, argv);
int port = result["port"].as<int>();
```

### Pitfall 2: Using implicit values with space-separated arguments
```cpp
// BAD: Implicit value with space-separated argument
options.add_options()
    ("v,verbose", "Verbose", cxxopts::value<int>()->implicit_value("3"));
// ./app --verbose 5  // This treats "5" as positional, not verbose value
```

```cpp
// GOOD: Use = syntax with implicit values
options.add_options()
    ("v,verbose", "Verbose", cxxopts::value<int>()->implicit_value("3"));
// ./app --verbose=5  // Correctly parses verbose as 5
```

### Pitfall 3: Not handling exceptions during parsing
```cpp
// BAD: No exception handling
auto result = options.parse(argc, argv);  // Can throw on invalid input
```

```cpp
// GOOD: Handle parsing exceptions
try {
    auto result = options.parse(argc, argv);
} catch (const cxxopts::exceptions::parsing& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
    return 1;
}
```

### Pitfall 4: Confusing default values with option count
```cpp
// BAD: Assuming default values count as specified
options.add_options()
    ("p,port", "Port", cxxopts::value<int>()->default_value("8080"));
auto result = options.parse(argc, argv);
if (result.count("port") == 0) {  // This is true even with default value
    // This code runs even though port has a value
}
```

```cpp
// GOOD: Check the actual value, not count
options.add_options()
    ("p,port", "Port", cxxopts::value<int>()->default_value("8080"));
auto result = options.parse(argc, argv);
int port = result["port"].as<int>();  // Always has a value
```

### Pitfall 5: Using `parse_positional` without understanding replacement behavior
```cpp
// BAD: Assuming parse_positional appends by default
options.parse_positional({"input"});
options.parse_positional({"output"});  // This replaces "input" with "output"
```

```cpp
// GOOD: Use Append mode explicitly
options.parse_positional({"input"});
options.parse_positional({"output"}, cxxopts::PositionalMode::Append);
```

### Pitfall 6: Not handling boolean options correctly
```cpp
// BAD: Assuming boolean options need explicit value
options.add_options()
    ("d,debug", "Debug mode");
// ./app --debug false  // This doesn't set debug to false
```

```cpp
// GOOD: Boolean options are implicitly true when present
options.add_options()
    ("d,debug", "Debug mode");
// ./app --debug  // debug is true
// ./app  // debug is false (default)
```

### Pitfall 7: Using vector positional arguments without proper parsing
```cpp
// BAD: Vector positional arguments with spaces
options.add_options()
    ("files", "Files", cxxopts::value<std::vector<std::string>>());
options.parse_positional({"files"});
// ./app file1.txt file2.txt  // Works, but spaces in filenames break
```

```cpp
// GOOD: Quote filenames with spaces
options.add_options()
    ("files", "Files", cxxopts::value<std::vector<std::string>>());
options.parse_positional({"files"});
// ./app "file with spaces.txt" "another file.txt"
```

### Pitfall 8: Ignoring the version 3 breaking changes
```cpp
// BAD: Assuming argv is modified (version 2 behavior)
char* argv[] = {"program", "--option", "value"};
options.parse(2, argv);  // argv is NOT modified in v3
```

```cpp
// GOOD: Pass const arguments in version 3
const int argc = 3;
const char* argv[] = {"program", "--option", "value"};
auto result = options.parse(argc, argv);  // Safe, no modification
```
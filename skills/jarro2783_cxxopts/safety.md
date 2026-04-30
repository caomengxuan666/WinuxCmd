# Safety

### Condition 1: NEVER access an option without checking if it exists
```cpp
// BAD: Accessing non-existent option
auto result = options.parse(argc, argv);
std::string value = result["nonexistent"].as<std::string>();  // Throws exception
```

```cpp
// GOOD: Check existence first
auto result = options.parse(argc, argv);
if (result.count("option")) {
    std::string value = result["option"].as<std::string>();
} else {
    // Handle missing option
}
```

### Condition 2: NEVER use `.as<type>()` with the wrong type
```cpp
// BAD: Type mismatch
options.add_options()
    ("p,port", "Port", cxxopts::value<int>());
auto result = options.parse(argc, argv);
std::string port = result["port"].as<std::string>();  // Throws exception
```

```cpp
// GOOD: Use correct type
options.add_options()
    ("p,port", "Port", cxxopts::value<int>());
auto result = options.parse(argc, argv);
int port = result["port"].as<int>();  // Correct type
```

### Condition 3: NEVER parse without exception handling for invalid input
```cpp
// BAD: No try-catch for parsing
auto result = options.parse(argc, argv);  // Crashes on invalid input
```

```cpp
// GOOD: Always wrap parsing in try-catch
try {
    auto result = options.parse(argc, argv);
} catch (const cxxopts::exceptions::parsing& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
```

### Condition 4: NEVER use `parse_positional` without understanding the replacement behavior
```cpp
// BAD: Assuming append behavior
options.parse_positional({"input"});
options.parse_positional({"output"});  // Silently replaces input
```

```cpp
// GOOD: Explicitly specify append mode
options.parse_positional({"input"});
options.parse_positional({"output"}, cxxopts::PositionalMode::Append);
```

### Condition 5: NEVER assume `--` handling works the same as other parsers
```cpp
// BAD: Assuming everything after -- is positional
options.add_options()
    ("o,output", "Output", cxxopts::value<std::string>());
// ./app -- --output=file.txt  // --output is treated as positional, not option
```

```cpp
// GOOD: Understand that -- stops option parsing
options.add_options()
    ("o,output", "Output", cxxopts::value<std::string>());
options.allow_unrecognised_options();
auto result = options.parse(argc, argv);
// Everything after -- is in result.unmatched()
```
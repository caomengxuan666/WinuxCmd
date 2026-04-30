# Safety

### 1. NEVER access option values before parsing
```cpp
// BAD: Using variable before parse
CLI::App app;
int port = 0;
app.add_option("--port", port);
std::cout << port;  // Undefined behavior if --port not provided

// GOOD: Parse first, then use
CLI::App app;
int port = 0;
app.add_option("--port", port);
CLI11_PARSE(app, argc, argv);
std::cout << port;
```

### 2. NEVER use the same variable for multiple options
```cpp
// BAD: Same variable bound to two options
int value;
app.add_option("--value1", value);
app.add_option("--value2", value);  // Last one wins, confusing behavior

// GOOD: Use separate variables
int value1, value2;
app.add_option("--value1", value1);
app.add_option("--value2", value2);
```

### 3. NEVER ignore parse exceptions in production code
```cpp
// BAD: Swallowing exceptions
CLI::App app;
try {
    app.parse(argc, argv);
} catch(...) {}  // Silently ignores all errors

// GOOD: Handle properly
CLI::App app;
try {
    app.parse(argc, argv);
} catch(const CLI::ParseError &e) {
    std::cerr << "Parse error: " << e.what() << "\n";
    return app.exit(e);
}
```

### 4. NEVER modify the App object after parsing
```cpp
// BAD: Adding options after parse
CLI::App app;
app.add_option("--port", port);
CLI11_PARSE(app, argc, argv);
app.add_option("--debug", debug);  // Undefined behavior

// GOOD: Add all options before parsing
CLI::App app;
app.add_option("--port", port);
app.add_option("--debug", debug);
CLI11_PARSE(app, argc, argv);
```

### 5. NEVER use `add_set` with non-copyable types
```cpp
// BAD: Using add_set with unique_ptr
std::unique_ptr<int> ptr;
app.add_set("--opt", ptr, {std::make_unique<int>(1)});  // Compilation error

// GOOD: Use add_option with custom conversion
int value;
app.add_option("--opt", value);
```
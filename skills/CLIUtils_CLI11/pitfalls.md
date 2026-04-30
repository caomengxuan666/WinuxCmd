# Pitfalls

### 1. Forgetting to call `required()` on mandatory options
```cpp
// BAD: Option is optional by default
CLI::App app;
std::string name;
app.add_option("--name", name);
CLI11_PARSE(app, argc, argv);
// name may be empty if --name not provided

// GOOD: Mark as required
CLI::App app;
std::string name;
app.add_option("--name", name, "Your name")->required();
CLI11_PARSE(app, argc, argv);
```

### 2. Using `add_set` with enums incorrectly
```cpp
// BAD: Enums don't work directly with add_set
enum Level { High, Medium, Low };
Level level;
app.add_set("-l,--level", level, {High, Medium, Low});  // Compilation error

// GOOD: Use strings and transform, or use add_option with custom conversion
enum Level { High, Medium, Low };
std::string level_str;
app.add_option("-l,--level", level_str, "Level")
   ->check(CLI::IsMember({"High", "Medium", "Low"}));
```

### 3. Not handling parse exceptions
```cpp
// BAD: Unhandled parse errors crash the program
CLI::App app;
int value;
app.add_option("--value", value);
app.parse(argc, argv);  // Throws on invalid input

// GOOD: Wrap in try-catch
CLI::App app;
int value;
app.add_option("--value", value);
try {
    app.parse(argc, argv);
} catch(const CLI::ParseError &e) {
    return app.exit(e);
}
```

### 4. Misunderstanding positional argument ordering
```cpp
// BAD: Positional arguments match in order, not by name
std::string first, second;
app.add_option("first", first);   // Matches first positional
app.add_option("second", second); // Matches second positional
// ./app hello world -> first="hello", second="world"

// GOOD: Use named options for clarity
std::string input, output;
app.add_option("-i,--input", input);
app.add_option("-o,--output", output);
```

### 5. Using `allow_extra_args` incorrectly with vectors
```cpp
// BAD: Vector option consumes all extra args
std::vector<int> values;
app.add_option("--values", values)->allow_extra_args(false);
// This doesn't limit to one value per --values

// GOOD: Use expected() to control count
std::vector<int> values;
app.add_option("--values", values)->expected(1);
// Each --values flag takes exactly one argument
```

### 6. Forgetting to check subcommand presence
```cpp
// BAD: Assuming subcommand was parsed
CLI::App app;
auto* sub = app.add_subcommand("run", "Run command");
std::string file;
sub->add_option("file", file);
CLI11_PARSE(app, argc, argv);
std::cout << file;  // May be uninitialized if "run" not used

// GOOD: Check before using
CLI::App app;
auto* sub = app.add_subcommand("run", "Run command");
std::string file;
sub->add_option("file", file);
CLI11_PARSE(app, argc, argv);
if(app.got_subcommand(sub))
    std::cout << file;
```
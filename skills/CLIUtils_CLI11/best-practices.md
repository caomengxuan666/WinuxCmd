# Best Practices

### Use the `CLI11_PARSE` macro for simplicity
```cpp
// Recommended: Use the macro for clean main()
#include <CLI/CLI.hpp>

int main(int argc, char** argv) {
    CLI::App app{"My Application"};
    // ... add options ...
    CLI11_PARSE(app, argc, argv);
    // ... use values ...
}
```

### Group related options
```cpp
// Group options for better help output
CLI::App app;
app.add_option_group("Input")->add_option("--input-file", input_file);
app.add_option_group("Output")->add_option("--output-file", output_file);
app.add_option_group("Debug")->add_flag("--verbose", verbose);
```

### Use validators for input validation
```cpp
// Validate ranges and formats
int port;
app.add_option("--port", port)
   ->check(CLI::Range(1024, 65535))
   ->required();

std::string email;
app.add_option("--email", email)
   ->check(CLI::Validator([](const std::string& s) {
       if(s.find('@') == std::string::npos)
           return "Invalid email";
       return "";
   }));
```

### Provide sensible defaults
```cpp
// Always provide defaults for optional options
int timeout = 30;
std::string output = "output.txt";
bool debug = false;

app.add_option("--timeout", timeout, "Timeout in seconds")->default_val(30);
app.add_option("--output", output, "Output file")->default_val("output.txt");
app.add_flag("--debug", debug, "Enable debug mode");
```

### Use subcommands for complex interfaces
```cpp
// Organize complex CLIs with subcommands
CLI::App app{"Database tool"};
app.require_subcommand(1);

auto* query = app.add_subcommand("query", "Query database");
auto* insert = app.add_subcommand("insert", "Insert record");

std::string sql;
query->add_option("sql", sql, "SQL query")->required();
```
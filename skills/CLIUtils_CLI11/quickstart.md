# Quickstart

```cpp
// Basic option with required flag
#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc, char** argv) {
    CLI::App app{"My Program"};
    std::string name;
    int count = 1;
    
    app.add_option("-n,--name", name, "Your name")->required();
    app.add_option("-c,--count", count, "Number of times");
    
    CLI11_PARSE(app, argc, argv);
    
    for(int i = 0; i < count; ++i)
        std::cout << "Hello, " << name << "!\n";
}
```

```cpp
// Boolean flags and positional arguments
#include <CLI/CLI.hpp>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    CLI::App app{"File processor"};
    bool verbose = false;
    std::vector<std::string> files;
    
    app.add_flag("-v,--verbose", verbose, "Enable verbose output");
    app.add_option("files", files, "Input files")->expected(-1);
    
    CLI11_PARSE(app, argc, argv);
    
    if(verbose) std::cout << "Processing " << files.size() << " files\n";
    for(const auto& f : files)
        std::cout << "File: " << f << "\n";
}
```

```cpp
// Subcommands
#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc, char** argv) {
    CLI::App app{"Git-like tool"};
    app.require_subcommand(1);
    
    std::string message;
    auto* add = app.add_subcommand("add", "Add files");
    auto* commit = app.add_subcommand("commit", "Commit changes");
    commit->add_option("-m,--message", message, "Commit message")->required();
    
    CLI11_PARSE(app, argc, argv);
    
    if(app.got_subcommand(add))
        std::cout << "Adding files...\n";
    if(app.got_subcommand(commit))
        std::cout << "Committing: " << message << "\n";
}
```

```cpp
// Validators and defaults
#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc, char** argv) {
    CLI::App app{"Calculator"};
    int value = 0;
    
    app.add_option("-n,--number", value, "A positive number")
       ->check(CLI::PositiveNumber)
       ->default_val(42);
    
    CLI11_PARSE(app, argc, argv);
    std::cout << "Value: " << value << "\n";
}
```

```cpp
// Configuration file support
#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc, char** argv) {
    CLI::App app{"Configurable app"};
    std::string config_file;
    int port = 8080;
    bool debug = false;
    
    app.set_config("--config", config_file, "Config file (TOML/INI)");
    app.add_option("--port", port, "Server port");
    app.add_flag("--debug", debug, "Debug mode");
    
    CLI11_PARSE(app, argc, argv);
    
    std::cout << "Port: " << port << ", Debug: " << std::boolalpha << debug << "\n";
}
```
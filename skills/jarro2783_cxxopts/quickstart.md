# Quickstart

```cpp
#include <cxxopts.hpp>
#include <iostream>

// Pattern 1: Basic boolean options
int main(int argc, char* argv[]) {
    cxxopts::Options options("MyProgram", "A brief description");
    options.add_options()
        ("d,debug", "Enable debugging")
        ("h,help", "Print usage");
    
    auto result = options.parse(argc, argv);
    
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    bool debug = result["debug"].as<bool>();
    std::cout << "Debug mode: " << (debug ? "on" : "off") << std::endl;
    return 0;
}
```

```cpp
// Pattern 2: Typed options with default values
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("Server", "Server configuration");
    options.add_options()
        ("p,port", "Port number", cxxopts::value<int>()->default_value("8080"))
        ("h,host", "Host address", cxxopts::value<std::string>()->default_value("localhost"));
    
    auto result = options.parse(argc, argv);
    
    int port = result["port"].as<int>();
    std::string host = result["host"].as<std::string>();
    
    std::cout << "Starting server on " << host << ":" << port << std::endl;
    return 0;
}
```

```cpp
// Pattern 3: Positional arguments
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("FileProcessor", "Process files");
    options.add_options()
        ("input", "Input file", cxxopts::value<std::string>())
        ("output", "Output file", cxxopts::value<std::string>());
    
    options.parse_positional({"input", "output"});
    auto result = options.parse(argc, argv);
    
    std::string input = result["input"].as<std::string>();
    std::string output = result["output"].as<std::string>();
    
    std::cout << "Processing " << input << " -> " << output << std::endl;
    return 0;
}
```

```cpp
// Pattern 4: Multiple positional arguments with vector
#include <cxxopts.hpp>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    cxxopts::Options options("BulkProcessor", "Process multiple files");
    options.add_options()
        ("files", "Files to process", cxxopts::value<std::vector<std::string>>());
    
    options.parse_positional({"files"});
    auto result = options.parse(argc, argv);
    
    auto files = result["files"].as<std::vector<std::string>>();
    for (const auto& file : files) {
        std::cout << "Processing: " << file << std::endl;
    }
    return 0;
}
```

```cpp
// Pattern 5: Implicit values
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("Logger", "Logging configuration");
    options.add_options()
        ("v,verbose", "Verbose output", cxxopts::value<int>()->implicit_value("3")->default_value("0"));
    
    auto result = options.parse(argc, argv);
    int verbose = result["verbose"].as<int>();
    
    std::cout << "Verbosity level: " << verbose << std::endl;
    return 0;
}
```

```cpp
// Pattern 6: Help groups
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("MyApp", "Application with groups");
    options.add_options("Network")
        ("p,port", "Port", cxxopts::value<int>()->default_value("80"))
        ("h,host", "Host", cxxopts::value<std::string>()->default_value("localhost"));
    
    options.add_options("Security")
        ("u,user", "Username", cxxopts::value<std::string>())
        ("k,key", "API key", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.count("help")) {
        std::cout << options.help({"Network", "Security"}) << std::endl;
        return 0;
    }
    return 0;
}
```

```cpp
// Pattern 7: Unrecognized options
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("FlexibleApp", "Accepts unknown options");
    options.allow_unrecognised_options();
    options.add_options()
        ("v,verbose", "Verbose mode");
    
    auto result = options.parse(argc, argv);
    
    auto unmatched = result.unmatched();
    for (const auto& arg : unmatched) {
        std::cout << "Unrecognized: " << arg << std::endl;
    }
    return 0;
}
```

```cpp
// Pattern 8: Boolean with explicit implicit value
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("ToggleApp", "Toggle features");
    options.add_options()
        ("f,feature", "Enable feature", cxxopts::value<bool>()->implicit_value("true"));
    
    auto result = options.parse(argc, argv);
    
    bool feature = result["feature"].as<bool>();
    std::cout << "Feature enabled: " << (feature ? "yes" : "no") << std::endl;
    return 0;
}
```
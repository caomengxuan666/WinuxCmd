# Lifecycle

### Construction and initialization
```cpp
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    // Construction with program name and description
    cxxopts::Options options("MyProgram", "Description of my program");
    
    // Add options during construction phase
    options.add_options()
        ("v,verbose", "Verbose output")
        ("o,output", "Output file", cxxopts::value<std::string>());
    
    // Parse is the "activation" step
    auto result = options.parse(argc, argv);
    
    // Use the result
    if (result.count("verbose")) {
        std::cout << "Verbose mode" << std::endl;
    }
    
    return 0;
}
```

### Move semantics with Options
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <memory>

// Options can be moved, but not copied
cxxopts::Options createOptions() {
    cxxopts::Options options("TempApp", "Temporary");
    options.add_options()
        ("d,debug", "Debug mode");
    return options;  // Move is supported
}

int main(int argc, char* argv[]) {
    // Move construction
    cxxopts::Options opts = createOptions();
    
    // Move assignment
    cxxopts::Options other("Other", "Other app");
    other = std::move(opts);
    
    auto result = other.parse(argc, argv);
    if (result.count("debug")) {
        std::cout << "Debug mode" << std::endl;
    }
    
    return 0;
}
```

### ParseResult lifecycle
```cpp
#include <cxxopts.hpp>
#include <iostream>

cxxopts::ParseResult parseArgs(int argc, char* argv[]) {
    cxxopts::Options options("App", "Description");
    options.add_options()
        ("n,name", "Name", cxxopts::value<std::string>());
    
    // ParseResult can be returned from function (v3 feature)
    return options.parse(argc, argv);
}

int main(int argc, char* argv[]) {
    // ParseResult lives independently of the parser
    auto result = parseArgs(argc, argv);
    
    // Can still access values after parser is destroyed
    if (result.count("name")) {
        std::string name = result["name"].as<std::string>();
        std::cout << "Name: " << name << std::endl;
    }
    
    return 0;
}
```

### Resource management and cleanup
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <memory>

class Application {
private:
    cxxopts::Options options;
    cxxopts::ParseResult result;
    
public:
    Application(int argc, char* argv[]) 
        : options("App", "Application") {
        // Setup options
        options.add_options()
            ("c,config", "Config file", cxxopts::value<std::string>())
            ("v,verbose", "Verbose mode");
        
        // Parse immediately
        result = options.parse(argc, argv);
    }
    
    // Default destructor is fine - no manual cleanup needed
    ~Application() = default;
    
    // Move constructor
    Application(Application&& other) noexcept 
        : options(std::move(other.options))
        , result(std::move(other.result)) {}
    
    // Move assignment
    Application& operator=(Application&& other) noexcept {
        if (this != &other) {
            options = std::move(other.options);
            result = std::move(other.result);
        }
        return *this;
    }
    
    // Delete copy operations
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    void run() {
        if (result.count("config")) {
            std::string config = result["config"].as<std::string>();
            std::cout << "Using config: " << config << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    Application app(argc, argv);
    app.run();
    return 0;
}
```
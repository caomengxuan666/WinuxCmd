# Best Practices

### Use consistent option naming and grouping
```cpp
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("MyApp", "Production application");
    
    // Group related options together
    options.add_options("General")
        ("h,help", "Print help")
        ("v,version", "Print version");
    
    options.add_options("Network")
        ("p,port", "Port number", cxxopts::value<int>()->default_value("8080"))
        ("h,host", "Host address", cxxopts::value<std::string>()->default_value("localhost"));
    
    options.add_options("Security")
        ("u,user", "Username", cxxopts::value<std::string>())
        ("k,key", "API key", cxxopts::value<std::string>());
    
    try {
        auto result = options.parse(argc, argv);
        
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }
        
        // Validate required options
        if (!result.count("user") || !result.count("key")) {
            std::cerr << "Error: --user and --key are required" << std::endl;
            return 1;
        }
        
    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Use default values for optional parameters
```cpp
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("ConfigApp", "Configuration manager");
    
    options.add_options()
        ("c,config", "Config file path", 
         cxxopts::value<std::string>()->default_value("/etc/myapp/config.json"))
        ("l,log-level", "Log level (0-5)", 
         cxxopts::value<int>()->default_value("3"))
        ("t,timeout", "Connection timeout in seconds", 
         cxxopts::value<int>()->default_value("30"));
    
    try {
        auto result = options.parse(argc, argv);
        
        // All options have values due to defaults
        std::string config = result["config"].as<std::string>();
        int logLevel = result["log-level"].as<int>();
        int timeout = result["timeout"].as<int>();
        
        std::cout << "Config: " << config << std::endl;
        std::cout << "Log level: " << logLevel << std::endl;
        std::cout << "Timeout: " << timeout << "s" << std::endl;
        
    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Handle both short and long options
```cpp
#include <cxxopts.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    cxxopts::Options options("FlexibleApp", "Supports both short and long options");
    
    options.add_options()
        ("v,verbose", "Enable verbose output")
        ("o,output", "Output file", cxxopts::value<std::string>())
        ("n,count", "Number of iterations", cxxopts::value<int>()->default_value("1"));
    
    try {
        auto result = options.parse(argc, argv);
        
        // Both -v and --verbose work
        if (result.count("verbose")) {
            std::cout << "Verbose mode enabled" << std::endl;
        }
        
        // Both -o file.txt and --output=file.txt work
        if (result.count("output")) {
            std::string output = result["output"].as<std::string>();
            std::cout << "Output: " << output << std::endl;
        }
        
        int count = result["count"].as<int>();
        std::cout << "Count: " << count << std::endl;
        
    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Validate option values after parsing
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    cxxopts::Options options("ValidatorApp", "Validates option values");
    
    options.add_options()
        ("p,port", "Port number (1024-65535)", cxxopts::value<int>()->default_value("8080"))
        ("l,log-level", "Log level (0-5)", cxxopts::value<int>()->default_value("3"));
    
    try {
        auto result = options.parse(argc, argv);
        
        int port = result["port"].as<int>();
        if (port < 1024 || port > 65535) {
            throw std::runtime_error("Port must be between 1024 and 65535");
        }
        
        int logLevel = result["log-level"].as<int>();
        if (logLevel < 0 || logLevel > 5) {
            throw std::runtime_error("Log level must be between 0 and 5");
        }
        
        std::cout << "Port: " << port << std::endl;
        std::cout << "Log level: " << logLevel << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```
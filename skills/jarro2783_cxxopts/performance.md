# Performance

### Parsing overhead characteristics
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    
    cxxopts::Options options("PerfTest", "Performance test");
    options.add_options()
        ("a,alpha", "Alpha option", cxxopts::value<int>())
        ("b,beta", "Beta option", cxxopts::value<std::string>())
        ("g,gamma", "Gamma option", cxxopts::value<double>());
    
    auto result = options.parse(argc, argv);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Parsing took " << duration.count() << " microseconds" << std::endl;
    
    // Access values
    if (result.count("alpha")) {
        int alpha = result["alpha"].as<int>();
        std::cout << "Alpha: " << alpha << std::endl;
    }
    
    return 0;
}
```

### Minimize option definitions for better performance
```cpp
#include <cxxopts.hpp>
#include <iostream>

// BAD: Defining many options that aren't used
void badPerformance(int argc, char* argv[]) {
    cxxopts::Options options("Bad", "Too many options");
    options.add_options()
        ("a,alpha", "Alpha", cxxopts::value<int>())
        ("b,beta", "Beta", cxxopts::value<int>())
        ("g,gamma", "Gamma", cxxopts::value<int>())
        ("d,delta", "Delta", cxxopts::value<int>())
        ("e,epsilon", "Epsilon", cxxopts::value<int>())
        ("z,zeta", "Zeta", cxxopts::value<int>());
    // ... many more options
    auto result = options.parse(argc, argv);
}

// GOOD: Define only needed options
void goodPerformance(int argc, char* argv[]) {
    cxxopts::Options options("Good", "Minimal options");
    options.add_options()
        ("c,config", "Config file", cxxopts::value<std::string>())
        ("v,verbose", "Verbose mode");
    auto result = options.parse(argc, argv);
}
```

### Avoid repeated parsing
```cpp
#include <cxxopts.hpp>
#include <iostream>

// BAD: Parsing multiple times
void badPractice(int argc, char* argv[]) {
    cxxopts::Options options("Bad", "Repeated parsing");
    options.add_options()
        ("v,verbose", "Verbose");
    
    auto result1 = options.parse(argc, argv);  // First parse
    bool verbose1 = result1["verbose"].as<bool>();
    
    auto result2 = options.parse(argc, argv);  // Second parse - wasteful
    bool verbose2 = result2["verbose"].as<bool>();
}

// GOOD: Parse once and reuse
void goodPractice(int argc, char* argv[]) {
    cxxopts::Options options("Good", "Single parse");
    options.add_options()
        ("v,verbose", "Verbose");
    
    auto result = options.parse(argc, argv);  // Parse once
    bool verbose = result["verbose"].as<bool>();
    
    // Use the result throughout the program
    if (verbose) {
        std::cout << "Verbose mode" << std::endl;
    }
}
```

### Use simple types for faster parsing
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    
    cxxopts::Options options("PerfTest", "Type performance");
    
    // BAD: Complex types are slower to parse
    options.add_options()
        ("c,complex", "Complex value", cxxopts::value<std::vector<int>>());
    
    // GOOD: Simple types are faster
    options.add_options()
        ("s,simple", "Simple value", cxxopts::value<int>());
    
    auto result = options.parse(argc, argv);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Parsing took " << duration.count() << " microseconds" << std::endl;
    
    return 0;
}
```
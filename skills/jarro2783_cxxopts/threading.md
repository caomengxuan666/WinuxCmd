# Threading

### cxxopts is NOT thread-safe
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <thread>
#include <mutex>

// BAD: Thread-unsafe access
void badThreading(int argc, char* argv[]) {
    cxxopts::Options options("Bad", "Thread-unsafe");
    options.add_options()
        ("v,verbose", "Verbose");
    
    std::thread t1([&]() {
        auto result = options.parse(argc, argv);  // Unsafe concurrent access
    });
    
    std::thread t2([&]() {
        options.add_options()
            ("d,debug", "Debug");  // Unsafe concurrent modification
    });
    
    t1.join();
    t2.join();
}

// GOOD: Use mutex for thread safety
void goodThreading(int argc, char* argv[]) {
    cxxopts::Options options("Good", "Thread-safe with mutex");
    std::mutex mtx;
    
    // Setup options in main thread
    options.add_options()
        ("v,verbose", "Verbose");
    
    std::thread t1([&]() {
        std::lock_guard<std::mutex> lock(mtx);
        auto result = options.parse(argc, argv);  // Safe with mutex
    });
    
    t1.join();
}
```

### Parse in main thread, use results in worker threads
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char* argv[]) {
    cxxopts::Options options("App", "Thread-safe usage");
    options.add_options()
        ("n,num-threads", "Number of threads", 
         cxxopts::value<int>()->default_value("4"))
        ("v,verbose", "Verbose output");
    
    // Parse in main thread only
    auto result = options.parse(argc, argv);
    
    int numThreads = result["num-threads"].as<int>();
    bool verbose = result["verbose"].as<bool>();
    
    // Use parsed values in worker threads (safe - no parsing)
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([verbose, i]() {
            if (verbose) {
                std::cout << "Thread " << i << " running" << std::endl;
            }
            // Do work...
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

### Copy ParseResult for thread-local access
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char* argv[]) {
    cxxopts::Options options("App", "Thread-local results");
    options.add_options()
        ("c,config", "Config file", cxxopts::value<std::string>())
        ("v,verbose", "Verbose");
    
    // Parse once in main thread
    auto mainResult = options.parse(argc, argv);
    
    // Copy result for each thread (safe - ParseResult is copyable)
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([mainResult, i]() {
            // Each thread has its own copy
            if (mainResult.count("verbose")) {
                std::cout << "Thread " << i << " verbose" << std::endl;
            }
            if (mainResult.count("config")) {
                std::string config = mainResult["config"].as<std::string>();
                std::cout << "Thread " << i << " config: " << config << std::endl;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

### Use atomic flags for thread-safe option checking
```cpp
#include <cxxopts.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

int main(int argc, char* argv[]) {
    cxxopts::Options options("App", "Atomic flags");
    options.add_options()
        ("v,verbose", "Verbose")
        ("d,debug", "Debug mode");
    
    auto result = options.parse(argc, argv);
    
    // Convert to atomic flags for thread-safe access
    std::atomic<bool> verbose{result["verbose"].as<bool>()};
    std::atomic<bool> debug{result["debug"].as<bool>()};
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&verbose, &debug, i]() {
            // Thread-safe access to atomic flags
            if (verbose.load()) {
                std::cout << "Thread " << i << " verbose" << std::endl;
            }
            if (debug.load()) {
                std::cout << "Thread " << i << " debug" << std::endl;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```
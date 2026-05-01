# Lifecycle

### Construction and initialization
```cpp
#include <loguru.hpp>

int main(int argc, char* argv[]) {
    // Loguru is ready to use after including the header
    // but must be initialized before most operations
    
    // Optional: Configure before init
    loguru::g_flush_interval_ms = 0;  // Unbuffered mode
    
    // Required: Initialize the library
    loguru::init(argc, argv);  // Parses -v flag for verbosity
    
    // Now logging is fully functional
    LOG_F(INFO, "Loguru initialized");
    
    return 0;
}
```

### File output lifecycle
```cpp
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    // Add file outputs - they persist until shutdown
    loguru::add_file("debug.log", loguru::Truncate, loguru::Verbosity_MAX);
    loguru::add_file("errors.log", loguru::Append, loguru::Verbosity_ERROR);
    
    LOG_F(INFO, "Logging to files");
    
    // Files are automatically closed during shutdown
    // No manual cleanup needed
    
    return 0;
}
```

### Scope lifecycle
```cpp
#include <loguru.hpp>

void outer_function() {
    LOG_SCOPE_F(INFO, "Outer scope");
    LOG_F(INFO, "Indented by 1 level");
    
    {
        LOG_SCOPE_F(2, "Inner scope");
        LOG_F(INFO, "Indented by 2 levels");
        // Inner scope ends here
    }
    
    LOG_F(INFO, "Back to 1 level indentation");
    // Outer scope ends here
}

int main() {
    loguru::init(0, nullptr);
    outer_function();
    return 0;
}
```

### Resource management
```cpp
#include <loguru.hpp>

class Application {
public:
    Application(int argc, char* argv[]) {
        loguru::init(argc, argv);
        loguru::add_file("app.log", loguru::Truncate, loguru::Verbosity_INFO);
        LOG_F(INFO, "Application created");
    }
    
    ~Application() {
        LOG_F(INFO, "Application shutting down");
        // Loguru handles cleanup automatically
        // No need to call shutdown() explicitly
    }
    
    // Move semantics - Loguru state is global, not per-instance
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;
    
    // Copy semantics
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};
```

### Shutdown sequence
```cpp
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    // Normal operation
    LOG_F(INFO, "Running");
    
    // Optional: Explicit shutdown (usually not needed)
    // loguru::shutdown();
    
    // After shutdown, logging is undefined behavior
    // LOG_F(INFO, "This is unsafe after shutdown");
    
    return 0;
    // Automatic shutdown happens here
}
```
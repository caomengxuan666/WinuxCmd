# Safety

### Red Line 1: Never pass user-controlled format strings directly
```cpp
// BAD: User input as format string
void log_user_input(const std::string& user_input) {
    LOG_F(INFO, user_input.c_str());  // Format string injection!
}

// GOOD: Use %s specifier
void log_user_input(const std::string& user_input) {
    LOG_F(INFO, "User input: %s", user_input.c_str());
}
```

### Red Line 2: Never call `LOG_F` or `CHECK_F` before `loguru::init()`
```cpp
// BAD: Logging before initialization
#include <loguru.hpp>

void early_log() {
    LOG_F(INFO, "Too early!");  // Undefined behavior
}

// GOOD: Initialize first
int main(int argc, char* argv[]) {
    loguru::init(argc, argv);
    LOG_F(INFO, "Safe to log now");
    return 0;
}
```

### Red Line 3: Never use `ABORT_F` in library code that might be used by others
```cpp
// BAD: Library function aborts the entire program
namespace my_lib {
    void process(int value) {
        ABORT_F("Invalid value: %d", value);  // Kills the calling program!
    }
}

// GOOD: Use exceptions or error codes
namespace my_lib {
    void process(int value) {
        if (value < 0) {
            throw std::invalid_argument("Invalid value");
        }
    }
}
```

### Red Line 4: Never modify `loguru::g_flush_interval_ms` while logging is active
```cpp
// BAD: Race condition on flush interval
void configure_logging() {
    loguru::g_flush_interval_ms = 100;
    // Start logging...
    loguru::g_flush_interval_ms = 0;  // Race with background thread!
}

// GOOD: Set before any logging starts
int main(int argc, char* argv[]) {
    loguru::g_flush_interval_ms = 100;  // Set before init
    loguru::init(argc, argv);
    // Safe to log now
    return 0;
}
```

### Red Line 5: Never call `loguru::shutdown()` while other threads are logging
```cpp
// BAD: Shutdown during concurrent logging
std::thread worker([]() {
    while (running) {
        LOG_F(INFO, "Working...");
    }
});
loguru::shutdown();  // Race condition with worker thread!

// GOOD: Synchronize shutdown
std::atomic<bool> running{true};
std::thread worker([&running]() {
    while (running) {
        LOG_F(INFO, "Working...");
    }
});
running = false;
worker.join();
loguru::shutdown();
```
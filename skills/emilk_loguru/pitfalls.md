# Pitfalls

### Pitfall 1: Forgetting to call `loguru::init()`
```cpp
// BAD: No initialization
#include <loguru.hpp>

int main() {
    LOG_F(INFO, "This might not work correctly");
    // Output may be unpredictable, verbosity flags ignored
    return 0;
}

// GOOD: Always initialize
#include <loguru.hpp>

int main(int argc, char* argv[]) {
    loguru::init(argc, argv);  // Parse command line for -v
    LOG_F(INFO, "Application started");
    return 0;
}
```

### Pitfall 2: Using `CHECK_F` with side effects
```cpp
// BAD: Side effect in CHECK_F
int counter = 0;
CHECK_F(++counter > 0, "Counter should be positive");  // Counter incremented in release too

// GOOD: Separate side effects
int counter = 0;
++counter;
CHECK_F(counter > 0, "Counter should be positive");
```

### Pitfall 3: Incorrect verbosity level usage
```cpp
// BAD: Verbosity levels are inverted from expectations
LOG_F(INFO, "This is INFO");  // INFO is verbosity level
LOG_F(0, "This is ERROR");    // 0 is FATAL level
LOG_F(9, "This is verbose");  // 9 is very verbose

// GOOD: Use named constants
LOG_F(INFO, "Information message");
LOG_F(WARNING, "Warning message");
LOG_F(ERROR, "Error message");
LOG_F(FATAL, "Fatal message");
LOG_F(1, "Verbose debug");  // Higher numbers = more verbose
```

### Pitfall 4: Not handling `CHECK_F` failures properly
```cpp
// BAD: CHECK_F aborts by default, no recovery possible
void process_data(int* data) {
    CHECK_F(data != nullptr, "Data is null");
    // If data is null, program aborts here
}

// GOOD: Use conditional logging for recoverable errors
void process_data(int* data) {
    if (data == nullptr) {
        LOG_F(ERROR, "Data is null, skipping processing");
        return;
    }
    // Process data safely
}
```

### Pitfall 5: Memory leaks with `add_file`
```cpp
// BAD: Not closing file handles
void setup_logging() {
    loguru::add_file("log.txt", loguru::Truncate, loguru::Verbosity_INFO);
    // File handle remains open until program exit
}

// GOOD: Loguru handles cleanup automatically
void setup_logging() {
    loguru::add_file("log.txt", loguru::Truncate, loguru::Verbosity_INFO);
    // File handles are closed during loguru::shutdown()
}
```

### Pitfall 6: Thread safety with `set_thread_name`
```cpp
// BAD: Setting thread name after logging from that thread
void worker() {
    LOG_F(INFO, "Starting work");  // Thread ID logged, not name
    loguru::set_thread_name("worker");  // Too late
}

// GOOD: Set thread name before logging
void worker() {
    loguru::set_thread_name("worker");
    LOG_F(INFO, "Starting work");  // Shows "worker" in log
}
```

### Pitfall 7: Using `DCHECK_F` in performance-critical code
```cpp
// BAD: DCHECK_F still evaluates the expression in debug
void process_large_data(const std::vector<int>& data) {
    DCHECK_F(data.size() > 0, "Empty data");  // Still checks in debug
    // Expensive operation follows
}

// GOOD: Use conditional for expensive checks
void process_large_data(const std::vector<int>& data) {
    if (data.empty()) {
        LOG_F(ERROR, "Empty data, skipping");
        return;
    }
    // Expensive operation
}
```

### Pitfall 8: Format string mismatch
```cpp
// BAD: Format string doesn't match arguments
LOG_F(INFO, "Value: %d", "not_an_integer");  // Undefined behavior
LOG_F(INFO, "Value: %s", 42);  // Will crash or print garbage

// GOOD: Match format specifiers to arguments
LOG_F(INFO, "Value: %d", 42);
LOG_F(INFO, "Value: %s", "string");
LOG_F(INFO, "Value: %.2f", 3.14159);
```
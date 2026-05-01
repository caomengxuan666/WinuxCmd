# Quickstart

```cpp
// Basic initialization and logging
#include <loguru.hpp>

int main(int argc, char* argv[]) {
    // Initialize loguru (parses -v verbosity flag)
    loguru::init(argc, argv);
    
    // Add file output
    loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file("latest.log", loguru::Truncate, loguru::Verbosity_INFO);
    
    // Basic logging
    LOG_F(INFO, "Application started with %d arguments", argc);
    LOG_F(WARNING, "This is a warning message");
    LOG_F(ERROR, "An error occurred: code=%d", 42);
    
    // Verbosity levels (0-9, higher = more verbose)
    LOG_F(1, "Verbose debug message");
    LOG_F(9, "Very verbose trace");
    
    // Conditional logging
    int error_code = -1;
    LOG_IF_F(ERROR, error_code < 0, "Negative error code: %d", error_code);
    
    // Dynamic verbosity
    int level = 3;
    VLOG_F(level, "Dynamic verbosity message at level %d", level);
    
    // Assertions
    FILE* fp = fopen("test.txt", "r");
    CHECK_F(fp != nullptr, "Failed to open test.txt");
    
    // Comparison assertions
    int x = 5, y = 10;
    CHECK_LT_F(x, y, "x should be less than y");
    CHECK_GT_F(y, x, "y should be greater than x");
    
    // Scope-based indentation
    LOG_SCOPE_F(INFO, "Processing data");
    LOG_F(INFO, "Inside scope - indented automatically");
    
    return 0;
}
```

```cpp
// Stream-style logging (requires #define before include)
#define LOGURU_WITH_STREAMS 1
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    // Stream-style logging
    LOG_S(INFO) << "Stream-style message: " << 42 << " and " << 3.14;
    
    // Stream-style assertions
    int value = 5;
    CHECK_EQ_S(value, 5) << "Value should be 5";
    CHECK_NE_S(value, 10) << "Value should not be 10";
    
    return 0;
}
```

```cpp
// Error context for crash debugging
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    int important_value = 42;
    std::string context = "critical operation";
    
    // Capture context that prints on crash
    loguru::set_error_context("Important value: %d", important_value);
    loguru::set_error_context("Context: %s", context.c_str());
    
    // This will print the error context on failure
    CHECK_F(important_value > 100, "Value too small");
    
    return 0;
}
```

```cpp
// Custom fatal handler
#include <loguru.hpp>
#include <stdexcept>

int main() {
    loguru::init(0, nullptr);
    
    // Throw exception instead of aborting
    loguru::set_fatal_handler([](const loguru::Message& message) {
        throw std::runtime_error(
            std::string(message.prefix) + message.message
        );
    });
    
    try {
        CHECK_F(false, "This will throw instead of abort");
    } catch (const std::runtime_error& e) {
        LOG_F(ERROR, "Caught: %s", e.what());
    }
    
    return 0;
}
```

```cpp
// Thread naming and multi-threaded logging
#include <loguru.hpp>
#include <thread>

void worker_thread(int id) {
    loguru::set_thread_name(("worker_" + std::to_string(id)).c_str());
    LOG_F(INFO, "Worker thread %d started", id);
    
    for (int i = 0; i < 5; ++i) {
        LOG_F(2, "Worker %d iteration %d", id, i);
    }
}

int main() {
    loguru::init(0, nullptr);
    loguru::set_thread_name("main");
    
    std::thread t1(worker_thread, 1);
    std::thread t2(worker_thread, 2);
    
    LOG_F(INFO, "Main thread waiting for workers");
    
    t1.join();
    t2.join();
    
    return 0;
}
```

```cpp
// Debug-only logging and assertions
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    // These only execute in debug builds
    DLOG_F(INFO, "Debug-only message");
    
    int expensive_result = perform_expensive_calculation();
    DCHECK_F(expensive_result > 0, "Expensive check failed: %d", expensive_result);
    
    // These execute in all builds
    LOG_F(INFO, "Release message");
    CHECK_F(true, "Always checked");
    
    return 0;
}
```

```cpp
// File output configuration
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    // Append mode - grows the file
    loguru::add_file("full_log.txt", loguru::Append, loguru::Verbosity_MAX);
    
    // Truncate mode - overwrites each run
    loguru::add_file("recent_log.txt", loguru::Truncate, loguru::Verbosity_INFO);
    
    // Control stderr output
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;  // Only warnings+
    loguru::g_colorlogtostderr = false;  // Disable colors
    
    // Flush control
    loguru::g_flush_interval_ms = 100;  // Flush every 100ms
    
    LOG_F(INFO, "Logging configured");
    
    return 0;
}
```

```cpp
// Signal handling and stack traces
#include <loguru.hpp>

int main() {
    loguru::init(0, nullptr);
    
    // Loguru automatically installs signal handlers
    // that print stack traces on crash signals
    
    // Test stack trace on abort
    // ABORT_F("This demonstrates stack trace: value=%d", 42);
    
    // Or use CHECK_F for assertions with context
    int* ptr = nullptr;
    CHECK_F(ptr != nullptr, "Null pointer detected");
    
    return 0;
}
```
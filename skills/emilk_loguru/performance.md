# Performance

### Performance characteristics
Loguru is designed for speed, typically taking 3-8 microseconds per log call depending on configuration. The library uses a simple, synchronous logging model where each log call formats the message and writes to all outputs immediately (or buffers for periodic flush).

```cpp
#include <loguru.hpp>
#include <chrono>

void benchmark_logging() {
    loguru::init(0, nullptr);
    loguru::add_file("bench.log", loguru::Truncate, loguru::Verbosity_MAX);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        LOG_F(INFO, "Benchmark iteration %d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    LOG_F(INFO, "Average: %d us per call", duration.count() / 10000);
}
```

### Flush interval optimization
```cpp
#include <loguru.hpp>

// Unbuffered mode (fastest for crash safety)
void configure_unbuffered() {
    loguru::g_flush_interval_ms = 0;  // Flush every call
    // ~6-8 us per log call
}

// Buffered mode (fastest for throughput)
void configure_buffered() {
    loguru::g_flush_interval_ms = 100;  // Flush every 100ms
    // ~3-5 us per log call
}
```

### Allocation patterns
```cpp
#include <loguru.hpp>
#include <string>

// BAD: Creates temporary strings
void log_with_string_concat(const std::string& name, int value) {
    LOG_F(INFO, ("Name: " + name + ", Value: " + std::to_string(value)).c_str());
    // Multiple allocations for string concatenation
}

// GOOD: Use printf-style formatting (no extra allocations)
void log_with_printf(const std::string& name, int value) {
    LOG_F(INFO, "Name: %s, Value: %d", name.c_str(), value);
    // Single format operation, no temporary strings
}
```

### Verbosity level optimization
```cpp
#include <loguru.hpp>

// BAD: Always evaluates expensive expressions
void expensive_log(const std::vector<int>& data) {
    LOG_F(2, "Data: %s", compute_expensive_string(data).c_str());
    // compute_expensive_string() called even if verbosity < 2
}

// GOOD: Check verbosity first
void expensive_log(const std::vector<int>& data) {
    if (loguru::g_stderr_verbosity >= 2) {
        LOG_F(2, "Data: %s", compute_expensive_string(data).c_str());
    }
}
```

### File output optimization
```cpp
#include <loguru.hpp>

// BAD: Multiple file outputs with high verbosity
void configure_many_files() {
    loguru::add_file("debug1.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file("debug2.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file("debug3.log", loguru::Append, loguru::Verbosity_MAX);
    // Each log call writes to all 3 files - 3x slower
}

// GOOD: Minimize file outputs for performance
void configure_efficient() {
    loguru::add_file("full.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file("errors.log", loguru::Truncate, loguru::Verbosity_ERROR);
    // Only 2 file writes per log call
}
```
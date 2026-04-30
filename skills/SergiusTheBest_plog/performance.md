# Performance


Performance characteristics and optimization tips for Plog.

**Lazy stream evaluation**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int expensiveFunction() {
    // Simulate expensive computation
    for (volatile int i = 0; i < 1000000; ++i);
    return 42;
}

int main() {
    plog::init(plog::warning, "app.log"); // Only warnings and errors
    
    // BAD: expensiveFunction() is called even though debug is disabled
    PLOGD << "Value: " << expensiveFunction(); // Still computes!
    
    // GOOD: use severity check to avoid computation
    if (plog::get()->checkSeverity(plog::debug)) {
        PLOGD << "Value: " << expensiveFunction(); // Only computes when debug is enabled
    }
    
    return 0;
}
```

**Performance benchmarks**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <chrono>
#include <iostream>

int main() {
    plog::init(plog::debug, "benchmark.log");
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100000; ++i) {
        PLOGD << "Simple message " << i;
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "100000 logs in " << duration << "ms ("
              << (100000.0 / duration) << " logs/ms)" << std::endl;
    // Typical output: ~5000-10000 logs/ms on modern hardware
    return 0;
}
```

**Optimization: Disable source file capture**
```cpp
// Define before including plog headers to skip __FILE__ capture
#define PLOG_CAPTURE_FILE 0

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    plog::init(plog::debug, "app.log");
    PLOGD << "No file info captured"; // Slightly faster
    return 0;
}
```

**Optimization: Use message-only formatter**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>

int main() {
    // MessageOnlyFormatter skips timestamp, severity, etc.
    static plog::RollingFileAppender<plog::MessageOnlyFormatter> appender("fast.log");
    plog::init(plog::debug, &appender);
    
    PLOGD << "Just the message"; // Output: "Just the message"
    // Faster than TxtFormatter which adds timestamp and severity
    return 0;
}
```

**Memory allocation patterns**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    plog::init(plog::debug, "app.log");
    
    // Plog allocates a fixed-size buffer per log message (32KB max)
    // Long messages cause reallocation
    std::string longString(50000, 'A');
    PLOGD << longString; // May cause heap allocation for large messages
    
    // Short messages use stack buffer (no heap allocation)
    PLOGD << "Short message"; // Fast, no allocation
    
    return 0;
}
```
# Best Practices


Recommended patterns for production use of Plog.

**Pattern 1: Singleton logger with multiple appenders**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ColorConsoleAppender.h>

class Logger {
public:
    static void init() {
        static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("production.log", 5000000, 10);
        static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
        plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);
    }
};

int main() {
    Logger::init();
    PLOGI << "Application started";
    return 0;
}
```

**Pattern 2: Conditional logging for performance**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

// Expensive computation only when debug is enabled
void logComplexData(const std::vector<int>& data) {
    if (plog::get()->checkSeverity(plog::debug)) {
        std::ostringstream oss;
        for (int v : data) oss << v << ",";
        PLOGD << "Vector data: " << oss.str();
    }
}

int main() {
    plog::init(plog::warning, "app.log"); // Only warnings and errors
    std::vector<int> bigData(1000000);
    logComplexData(bigData); // No overhead from string formatting
    return 0;
}
```

**Pattern 3: Logging in libraries with separate loggers**
```cpp
// mylibrary.h
#include <plog/Log.h>

namespace mylib {
    // Use a separate logger instance to avoid conflicts
    void initLogging() {
        static plog::RollingFileAppender<plog::TxtFormatter> appender("mylib.log");
        plog::init<1>(plog::debug, &appender); // Logger instance 1
    }
    
    inline void doWork() {
        PLOGD_(1) << "Library operation"; // Use logger 1
    }
}

// main.cpp
int main() {
    plog::init(plog::info, "app.log"); // Logger instance 0
    mylib::initLogging();
    PLOGI << "App started";
    mylib::doWork();
    return 0;
}
```

**Pattern 4: Using CSV format for automated log analysis**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/CsvFormatter.h>

int main() {
    // CSV format is ideal for parsing with scripts or importing to databases
    plog::init<plog::CsvFormatter>(plog::debug, "audit.csv", 10000000, 20);
    
    PLOGI << "User login: admin";
    PLOGE << "Database connection failed: timeout";
    // Output: 2024/01/15;10:30:45.123;INFO;1234;main@12;"User login: admin"
    return 0;
}
```

**Pattern 5: Runtime severity adjustment for debugging**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <csignal>

void handleSignal(int) {
    // Increase logging verbosity on signal
    plog::get()->setMaxSeverity(plog::debug);
    PLOGI << "Log level increased to debug due to signal";
}

int main() {
    plog::init(plog::warning, "app.log");
    std::signal(SIGUSR1, handleSignal);
    
    // Normal operation - only warnings and errors
    PLOGD << "Hidden debug info";
    PLOGW << "Visible warning";
    return 0;
}
```

**Pattern 6: Disable logging in release builds**
```cpp
// Define before including plog headers to strip all logging
#define PLOG_DISABLE_LOGGING

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    plog::init(plog::debug, "app.log"); // No-op in release
    PLOGD << "This is compiled out"; // Macro expands to nothing
    PLOGI << "Also removed";
    return 0;
}
```
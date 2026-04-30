# Quickstart


Here are the most common usage patterns for Plog. Copy-paste these into your project to get started quickly.

```cpp
// Pattern 1: Basic file logging
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    plog::init(plog::debug, "app.log"); // Log all severities to app.log
    PLOGD << "This is a debug message";
    PLOGI << "Info message";
    PLOGW << "Warning message";
    PLOGE << "Error message";
    return 0;
}
```

```cpp
// Pattern 2: Console logging with colors
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

int main() {
    static plog::ColorConsoleAppender<plog::TxtFormatter> appender;
    plog::init(plog::debug, &appender);
    PLOGD << "Debug message (gray)";
    PLOGW << "Warning (yellow)";
    PLOGE << "Error (red)";
    return 0;
}
```

```cpp
// Pattern 3: Rolling file with size limit
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    // Max file size: 1MB, keep 5 rotated files
    plog::init(plog::info, "app.log", 1000000, 5);
    for (int i = 0; i < 1000; ++i) {
        PLOGI << "Iteration " << i;
    }
    return 0;
}
```

```cpp
// Pattern 4: CSV format for analysis
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/CsvFormatter.h>

int main() {
    plog::init<plog::CsvFormatter>(plog::debug, "log.csv");
    PLOGD << "Easy to parse in Excel";
    PLOGE << "Error with timestamp";
    return 0;
}
```

```cpp
// Pattern 5: Conditional logging
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    plog::init(plog::debug, "app.log");
    int errorCode = -1;
    PLOGE_IF(errorCode != 0) << "Operation failed with code " << errorCode;
    // Only logs if condition is true
    return 0;
}
```

```cpp
// Pattern 6: Multiple appenders (file + console)
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>

int main() {
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("app.log");
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);
    PLOGI << "Logged to both file and console";
    return 0;
}
```

```cpp
// Pattern 7: Severity level at runtime
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    plog::init(plog::debug, "app.log");
    PLOGD << "This is visible";
    plog::get()->setMaxSeverity(plog::warning); // Suppress debug/info
    PLOGD << "This is NOT visible";
    PLOGW << "This is still visible";
    return 0;
}
```
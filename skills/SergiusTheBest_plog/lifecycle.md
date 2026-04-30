# Lifecycle


Understanding the lifecycle of Plog components: construction, initialization, and cleanup.

**Logger initialization and release**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main() {
    // Construction: init() creates the logger and attaches appenders
    plog::init(plog::debug, "app.log"); // Logger is now active
    
    PLOGD << "First message"; // Logger writes to file
    
    // Destruction: release() cleans up the logger
    plog::release(); // Logger is destroyed, file is flushed
    
    // After release, logging is undefined behavior
    // PLOGD << "This would crash";
    
    return 0;
}
```

**Appender lifetime management**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ConsoleAppender.h>

// Appenders must have static storage duration
static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("persistent.log");
static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

void setupLogger() {
    // init() stores a pointer to the appender, does not copy it
    plog::init(plog::debug, &fileAppender).addAppender(&consoleAppender);
    // fileAppender and consoleAppender must outlive all logging
}

int main() {
    setupLogger();
    PLOGI << "Logging with persistent appenders";
    // Appenders are destroyed after main() returns, in reverse order of declaration
    return 0;
}
```

**Multiple logger instances**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>

int main() {
    // Logger instance 0 (default)
    static plog::RollingFileAppender<plog::TxtFormatter> appender0("main.log");
    plog::init(plog::debug, &appender0);
    
    // Logger instance 1
    static plog::RollingFileAppender<plog::TxtFormatter> appender1("network.log");
    plog::init<1>(plog::info, &appender1);
    
    PLOGD << "To main.log"; // Uses instance 0
    PLOGD_(1) << "To network.log"; // Uses instance 1
    
    // Release specific instances
    plog::release<1>(); // Release network logger
    // plog::release(); // Release all loggers
    
    return 0;
}
```

**Move semantics and resource management**
```cpp
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>

// Plog appenders are not movable (they hold file handles)
// BAD: attempting to move an appender
// auto appender = plog::RollingFileAppender<plog::TxtFormatter>("log.txt"); // OK
// auto moved = std::move(appender); // Compile error: deleted copy constructor

// GOOD: use pointers or references
static plog::RollingFileAppender<plog::TxtFormatter> appender("log.txt");

void reconfigure() {
    // To change appender, release and reinitialize
    plog::release();
    static plog::RollingFileAppender<plog::TxtFormatter> newAppender("new_log.txt");
    plog::init(plog::debug, &newAppender);
}
```

**Resource cleanup on shutdown**
```cpp
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <cstdlib>

void cleanup() {
    plog::release(); // Flush and close all log files
}

int main() {
    std::atexit(cleanup); // Register cleanup handler
    
    plog::init(plog::debug, "app.log");
    PLOGI << "Application running";
    
    // On exit, cleanup() is called automatically
    return 0;
}
```
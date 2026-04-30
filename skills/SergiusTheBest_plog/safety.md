# Safety


Five red-line conditions that must NEVER occur when using Plog.

**Condition 1: Never log before initialization**
```cpp
// BAD: logging before plog::init() causes undefined behavior
int main() {
    PLOGD << "Hello"; // CRASH: no logger instance
    plog::init(plog::debug, "app.log");
    return 0;
}
```

```cpp
// GOOD: always initialize at the start of main()
int main() {
    plog::init(plog::debug, "app.log");
    PLOGD << "Hello";
    return 0;
}
```

**Condition 2: Never destroy appenders while logger is active**
```cpp
// BAD: appender destroyed before program ends
void setup() {
    plog::RollingFileAppender<plog::TxtFormatter> appender("log.txt");
    plog::init(plog::debug, &appender);
} // appender destroyed, logger has dangling pointer

int main() {
    setup();
    PLOGD << "This will crash"; // Undefined behavior
    return 0;
}
```

```cpp
// GOOD: appender must outlive all logging
static plog::RollingFileAppender<plog::TxtFormatter> appender("log.txt");

void setup() {
    plog::init(plog::debug, &appender);
}

int main() {
    setup();
    PLOGD << "Safe";
    return 0;
}
```

**Condition 3: Never use LOG_XXX macros in code that may be included by other libraries**
```cpp
// BAD: LOGD macro may be redefined by another library
// mylib.h
#include <plog/Log.h>
void myFunc() {
    LOGD << "test"; // If another library also defines LOGD, this is ambiguous
}
```

```cpp
// GOOD: always use PLOG-prefixed macros in library code
// mylib.h
#include <plog/Log.h>
void myFunc() {
    PLOGD << "test"; // Unique to plog
}
```

**Condition 4: Never call plog::init() multiple times without releasing the logger**
```cpp
// BAD: double initialization leaks resources
int main() {
    plog::init(plog::debug, "log1.txt");
    plog::init(plog::info, "log2.txt"); // First logger is leaked
    PLOGI << "test";
    return 0;
}
```

```cpp
// GOOD: release first logger before reinitializing
int main() {
    plog::init(plog::debug, "log1.txt");
    plog::release(); // Release first logger
    plog::init(plog::info, "log2.txt");
    PLOGI << "test";
    return 0;
}
```

**Condition 5: Never pass a null pointer as appender**
```cpp
// BAD: null appender causes crash
int main() {
    plog::init(plog::debug, static_cast<plog::IAppender*>(nullptr)); // CRASH
    PLOGD << "test";
    return 0;
}
```

```cpp
// GOOD: always provide a valid appender
int main() {
    static plog::ConsoleAppender<plog::TxtFormatter> appender;
    plog::init(plog::debug, &appender);
    PLOGD << "test";
    return 0;
}
```
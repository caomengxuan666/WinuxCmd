# Pitfalls


Common mistakes when using Plog and how to avoid them.

**Pitfall 1: Appender lifetime too short**
```cpp
// BAD: appender destroyed before logger uses it
void setupLogging() {
    plog::RollingFileAppender<plog::TxtFormatter> appender("log.txt");
    plog::init(plog::debug, &appender);
} // appender destroyed here, logger has dangling pointer

int main() {
    setupLogging();
    PLOGD << "This may crash!"; // Undefined behavior
    return 0;
}
```

```cpp
// GOOD: static lifetime for appender
void setupLogging() {
    static plog::RollingFileAppender<plog::TxtFormatter> appender("log.txt");
    plog::init(plog::debug, &appender);
}

int main() {
    setupLogging();
    PLOGD << "Safe logging";
    return 0;
}
```

**Pitfall 2: Forgetting to initialize**
```cpp
// BAD: logging before init
int main() {
    PLOGD << "Before init"; // Undefined behavior, may crash
    plog::init(plog::debug, "app.log");
    return 0;
}
```

```cpp
// GOOD: always init first
int main() {
    plog::init(plog::debug, "app.log");
    PLOGD << "After init";
    return 0;
}
```

**Pitfall 3: Macro name clashes with other libraries**
```cpp
// BAD: using short macros that may clash
#include <plog/Log.h>
// Some other library also defines LOGD
LOGD << "This might not be plog!";
```

```cpp
// GOOD: use PLOG-prefixed macros to avoid clashes
#include <plog/Log.h>
PLOGD << "This is definitely plog";
```

**Pitfall 4: Ignoring thread safety of custom appenders**
```cpp
// BAD: custom appender without mutex
class UnsafeAppender : public plog::IAppender {
    std::vector<std::string> messages; // Not thread-safe!
public:
    void write(const plog::Record& record) override {
        messages.push_back(record.getMessage()); // Data race!
    }
};
```

```cpp
// GOOD: use built-in appenders or add mutex
#include <mutex>
class SafeAppender : public plog::IAppender {
    std::vector<std::string> messages;
    std::mutex mutex;
public:
    void write(const plog::Record& record) override {
        std::lock_guard<std::mutex> lock(mutex);
        messages.push_back(record.getMessage());
    }
};
```

**Pitfall 5: Setting max file size too small**
```cpp
// BAD: minimum file size is 1000 bytes
plog::init(plog::debug, "app.log", 100, 3); // 100 bytes < 1000 minimum
// Plog will use 1000 internally, but behavior is undefined
```

```cpp
// GOOD: use reasonable sizes
plog::init(plog::debug, "app.log", 1000000, 3); // 1MB files, 3 rotations
```

**Pitfall 6: Using LOG_XXX macros in header files**
```cpp
// BAD: macros in headers cause multiple definitions
// myheader.h
void doSomething() {
    LOGD << "doing something"; // LOGD expands to plog::get<0>()...
}
```

```cpp
// GOOD: use PLOG_XXX or wrap in functions
// myheader.h
inline void doSomething() {
    PLOGD << "doing something"; // PLOG-prefixed macros are safe
}
```

**Pitfall 7: Not checking severity before expensive computation**
```cpp
// BAD: expensive computation even when logging is disabled
plog::init(plog::warning, "app.log"); // Only warning and above
PLOGD << "Complex data: " << computeExpensiveString(); // computeExpensiveString() called even though debug is disabled
```

```cpp
// GOOD: use severity check or conditional macro
plog::init(plog::warning, "app.log");
if (plog::get()->checkSeverity(plog::debug)) {
    PLOGD << "Complex data: " << computeExpensiveString();
}
// Or use PLOG_DEBUG_IF with a condition that's always true
```
# Overview


Plog is a header-only, portable C++ logging library designed for simplicity and minimal overhead. It provides a clean macro-based API for writing log messages with severity levels, timestamps, and source location information.

**When to use Plog:**
- You need a lightweight logging library with no external dependencies
- You want simple file rotation or console output with colors
- You need cross-platform support (Windows, Linux, macOS, Android, embedded)
- You prefer header-only libraries for easy integration
- You want thread-safe logging without complex setup

**When NOT to use Plog:**
- You need structured JSON logging (Plog only supports TXT and CSV)
- You require asynchronous logging with non-blocking writes
- You need network/remote logging (syslog, socket appenders)
- You want runtime-configurable log levels per module (Plog uses global severity)
- You need C++20 modules support (Plog uses traditional headers)

**Key design principles:**
- **Header-only**: No compilation or linking required
- **Macro-based**: Uses `PLOGD`, `PLOGI`, `PLOGW`, `PLOGE` macros for concise logging
- **Lazy evaluation**: Log arguments are only evaluated if the severity level is enabled
- **Pluggable architecture**: Separate formatters (TXT, CSV) and appenders (file, console, color)
- **Thread-safe**: All appenders use mutex protection internally
- **Severity levels**: `none`, `fatal`, `error`, `warning`, `info`, `debug`, `verbose`

```cpp
// Core architecture: Logger -> Appender(s) with Formatter
#include <plog/Log.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/CsvFormatter.h>

int main() {
    // RollingFileAppender uses TxtFormatter by default
    static plog::RollingFileAppender<plog::CsvFormatter> appender("log.csv");
    plog::init(plog::debug, &appender);
    PLOGI << "CSV formatted log entry";
    return 0;
}
```
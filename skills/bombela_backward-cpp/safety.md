# Safety

**Condition 1: NEVER call Printer::print() from a signal handler**
```cpp
// BAD: Unsafe signal handler usage
#include "backward.hpp"
#include <signal.h>

backward::SignalHandling sh;

extern "C" void unsafe_handler(int) {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st); // CRASH: malloc() not async-signal-safe
}
```

```cpp
// GOOD: Safe signal handler - only capture, don't print
#include "backward.hpp"
#include <signal.h>
#include <cstring>

backward::StackTrace g_st;

extern "C" void safe_handler(int) {
    g_st.load_here(32); // Safe: uses pre-allocated buffer
    // Write to file descriptor using write() (async-signal-safe)
    const char* msg = "Crash detected\n";
    write(STDERR_FILENO, msg, strlen(msg));
    _exit(1);
}
```

**Condition 2: NEVER use backward.hpp without debug symbols in production**
```cpp
// BAD: Release build without debug info
// Compiled with: g++ -O2 -DNDEBUG main.cpp
#include "backward.hpp"
// Stack traces show only hex addresses - useless for debugging
```

```cpp
// GOOD: Always compile with -g even in optimized builds
// Compiled with: g++ -g -O2 main.cpp
#include "backward.hpp"
// Stack traces show function names and line numbers
```

**Condition 3: NEVER assume SignalHandling works without proper linking**
```cpp
// BAD: Missing library linkage
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"

backward::SignalHandling sh; // May crash at construction if libbfd not linked
```

```cpp
// GOOD: Verify library availability at compile time
#include "backward.hpp"

// Use feature test macros
#if defined(BACKWARD_HAS_BFD) || defined(BACKWARD_HAS_DW) || defined(BACKWARD_HAS_DWARF)
backward::SignalHandling sh;
#else
#warning "No supported debug info library found, stack traces will be limited"
#endif
```

**Condition 4: NEVER use load_here() with a frame count that exceeds available stack**
```cpp
// BAD: Requesting too many frames
#include "backward.hpp"

void deep_recursion(int depth) {
    backward::StackTrace st;
    st.load_here(10000); // May overflow internal buffer
    if (depth > 0) deep_recursion(depth - 1);
}
```

```cpp
// GOOD: Use reasonable frame limits
#include "backward.hpp"

void deep_recursion(int depth) {
    backward::StackTrace st;
    st.load_here(64); // 64 frames is usually sufficient
    if (depth > 0) deep_recursion(depth - 1);
}
```

**Condition 5: NEVER share a Printer object across threads without synchronization**
```cpp
// BAD: Unsynchronized Printer access
#include "backward.hpp"
#include <thread>

backward::Printer shared_printer;

void thread_func() {
    backward::StackTrace st;
    st.load_here(32);
    shared_printer.print(st); // Data race on Printer's internal state
}

int main() {
    std::thread t1(thread_func);
    std::thread t2(thread_func);
    t1.join(); t2.join();
}
```

```cpp
// GOOD: Each thread has its own Printer
#include "backward.hpp"
#include <thread>

void thread_func() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer local_printer; // Thread-local instance
    local_printer.print(st);
}

int main() {
    std::thread t1(thread_func);
    std::thread t2(thread_func);
    t1.join(); t2.join();
}
```
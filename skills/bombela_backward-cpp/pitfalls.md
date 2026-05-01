# Pitfalls

```cpp
// BAD: Forgetting to compile with debug symbols
// Compiled with: g++ -O2 main.cpp
#include "backward.hpp"

void func() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st); // Output shows only addresses, no function names
}
```

```cpp
// GOOD: Compile with debug symbols
// Compiled with: g++ -g -O2 main.cpp
#include "backward.hpp"

void func() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st); // Shows function names, file names, line numbers
}
```

```cpp
// BAD: Using Printer in a signal handler (not async-signal-safe)
#include "backward.hpp"
#include <signal.h>

backward::SignalHandling sh;

void my_handler(int sig) {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st); // DANGER: Printer allocates memory, not safe in signal handler
}
```

```cpp
// GOOD: Capture stack trace in signal handler, print later
#include "backward.hpp"
#include <signal.h>
#include <atomic>

std::atomic<bool> crash_occurred{false};
backward::StackTrace crash_st;

extern "C" void my_handler(int sig) {
    crash_st.load_here(32); // This is safe
    crash_occurred.store(true);
    _exit(1); // Must exit, can't return to corrupted state
}
```

```cpp
// BAD: Not linking required libraries
// Compiled with: g++ -g main.cpp -o main
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
// Linker error: undefined reference to 'bfd_openr'
```

```cpp
// GOOD: Link with required libraries
// Compiled with: g++ -g main.cpp -o main -lbfd -ldl
#define BACKWARD_HAS_BFD 1
#include "backward.hpp"
// Works correctly
```

```cpp
// BAD: Using default constructed Printer without configuration
#include "backward.hpp"

void func() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    // p is not configured - uses default settings
    p.print(st); // May not show source code snippets
}
```

```cpp
// GOOD: Configure Printer for optimal output
#include "backward.hpp"

void func() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.address = true;     // Show addresses
    p.snippet = true;     // Show source code snippets
    p.color_mode = backward::ColorMode::always; // Force color output
    p.print(st);
}
```
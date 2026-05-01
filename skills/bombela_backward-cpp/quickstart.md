# Quickstart

```cpp
// Quickstart: Basic stack trace printing
#include "backward.hpp"
#include <iostream>

void dangerous_function() {
    backward::StackTrace st;
    st.load_here(32); // Capture up to 32 frames
    backward::Printer p;
    p.print(st); // Print to stderr
}

int main() {
    dangerous_function();
    return 0;
}
```

```cpp
// Quickstart: Signal handler for crashes
#include "backward.hpp"

// This registers signal handlers for SIGSEGV, SIGABRT, etc.
backward::SignalHandling sh;

void cause_segfault() {
    int* p = nullptr;
    *p = 42; // Will trigger signal handler
}

int main() {
    cause_segfault();
    return 0;
}
```

```cpp
// Quickstart: Custom output stream
#include "backward.hpp"
#include <sstream>
#include <iostream>

void log_stack_trace() {
    backward::StackTrace st;
    st.load_here(32);
    
    backward::Printer p;
    std::stringstream ss;
    p.print(st, ss); // Print to stringstream instead of stderr
    
    std::cout << "Stack trace:\n" << ss.str();
}
```

```cpp
// Quickstart: Resolver for symbol names
#include "backward.hpp"
#include <iostream>

void print_resolved_trace() {
    backward::StackTrace st;
    st.load_here(32);
    
    backward::TraceResolver tr;
    tr.load_stacktrace(st);
    
    for (size_t i = 0; i < st.size(); ++i) {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        std::cout << "#" << i << " " << trace.object_function 
                  << " in " << trace.source.filename 
                  << ":" << trace.source.line << std::endl;
    }
}
```

```cpp
// Quickstart: Skipping frames
#include "backward.hpp"

void inner_function() {
    backward::StackTrace st;
    st.load_here(32);
    // Skip the first 2 frames (this function and its caller)
    st.skip_n_firsts(2);
    backward::Printer p;
    p.print(st);
}

void middle_function() {
    inner_function();
}

int main() {
    middle_function();
    return 0;
}
```

```cpp
// Quickstart: Thread-safe stack trace capture
#include "backward.hpp"
#include <thread>

void worker_thread() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st);
}

int main() {
    std::thread t1(worker_thread);
    std::thread t2(worker_thread);
    t1.join();
    t2.join();
    return 0;
}
```
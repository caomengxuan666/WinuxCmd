# Lifecycle

```cpp
// Construction: Default construction of StackTrace
#include "backward.hpp"

void example_construction() {
    // Default constructor - empty stack trace
    backward::StackTrace st;
    assert(st.size() == 0);
    
    // Load current stack
    st.load_here(32);
    assert(st.size() > 0);
    
    // Copy constructor
    backward::StackTrace st2(st);
    assert(st2.size() == st.size());
    
    // Move constructor
    backward::StackTrace st3(std::move(st));
    assert(st.size() == 0); // Moved-from state is empty
    assert(st3.size() > 0);
}
```

```cpp
// Destruction and resource management
#include "backward.hpp"

class StackTraceGuard {
    backward::StackTrace st_;
public:
    StackTraceGuard() {
        st_.load_here(32);
    }
    
    ~StackTraceGuard() {
        // Printer may allocate memory, but that's okay in destructor
        // (not called from signal handler)
        backward::Printer p;
        p.print(st_);
    }
    
    // Move semantics
    StackTraceGuard(StackTraceGuard&& other) noexcept 
        : st_(std::move(other.st_)) {}
    
    StackTraceGuard& operator=(StackTraceGuard&& other) noexcept {
        if (this != &other) {
            st_ = std::move(other.st_);
        }
        return *this;
    }
    
    // Delete copy (or implement if needed)
    StackTraceGuard(const StackTraceGuard&) = delete;
    StackTraceGuard& operator=(const StackTraceGuard&) = delete;
};
```

```cpp
// Resource management with SignalHandling
#include "backward.hpp"

void lifecycle_example() {
    // SignalHandling registers signal handlers in constructor
    backward::SignalHandling sh;
    
    // Now SIGSEGV, SIGABRT, etc. will print stack traces
    
    // When sh goes out of scope, signal handlers are restored
    // to their previous state
} // sh destructor called here

// WARNING: SignalHandling should be a singleton in most cases
// Creating multiple instances may cause issues
```

```cpp
// Printer configuration and reuse
#include "backward.hpp"

void printer_lifecycle() {
    backward::Printer p;
    
    // Configure once
    p.address = true;
    p.snippet = true;
    p.color_mode = backward::ColorMode::automatic;
    
    // Reuse for multiple traces
    backward::StackTrace st1, st2;
    st1.load_here(32);
    st2.load_here(32);
    
    p.print(st1);
    p.print(st2); // Printer state is preserved between calls
    
    // Printer can be reset
    p = backward::Printer(); // Reset to defaults
}
```
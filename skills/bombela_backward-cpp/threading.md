# Threading

```cpp
// Thread safety: StackTrace capture is thread-safe
#include "backward.hpp"
#include <thread>
#include <vector>

void thread_safe_capture() {
    backward::StackTrace st;
    st.load_here(32); // Safe to call from multiple threads simultaneously
    // Each thread has its own StackTrace object
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(thread_safe_capture);
    }
    for (auto& t : threads) t.join();
    return 0;
}
```

```cpp
// Thread safety: Printer is NOT thread-safe
#include "backward.hpp"
#include <thread>
#include <mutex>

backward::Printer global_printer;
std::mutex printer_mutex;

void thread_safe_print() {
    backward::StackTrace st;
    st.load_here(32);
    
    // Must synchronize access to shared Printer
    std::lock_guard<std::mutex> lock(printer_mutex);
    global_printer.print(st);
}

// Better: Use thread-local Printer
void thread_local_print() {
    thread_local backward::Printer local_printer;
    backward::StackTrace st;
    st.load_here(32);
    local_printer.print(st); // No synchronization needed
}
```

```cpp
// Thread safety: SignalHandling and threads
#include "backward.hpp"
#include <thread>
#include <signal.h>

backward::SignalHandling sh; // Must be created before any threads

void worker() {
    // Signal handlers are process-wide, so all threads benefit
    // If a crash occurs in this thread, the handler will print stack trace
    volatile int* p = nullptr;
    // *p = 42; // Would trigger handler in this thread
}

int main() {
    std::thread t(worker);
    t.join();
    return 0;
}
```

```cpp
// Thread safety: TraceResolver is NOT thread-safe
#include "backward.hpp"
#include <thread>
#include <mutex>

backward::TraceResolver global_resolver;
std::mutex resolver_mutex;

void safe_resolve(backward::StackTrace::Frame frame) {
    std::lock_guard<std::mutex> lock(resolver_mutex);
    backward::ResolvedTrace rt;
    rt.addr = frame.addr;
    global_resolver.resolve(rt);
}

// Better: Thread-local resolver
void thread_local_resolve(backward::StackTrace::Frame frame) {
    thread_local backward::TraceResolver local_resolver;
    backward::ResolvedTrace rt;
    rt.addr = frame.addr;
    local_resolver.resolve(rt);
}
```
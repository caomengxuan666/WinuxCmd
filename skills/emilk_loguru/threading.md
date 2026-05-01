# Threading

### Thread safety guarantees
Loguru is fully thread-safe for all logging operations. Multiple threads can call `LOG_F`, `CHECK_F`, and other logging functions simultaneously without data races.

```cpp
#include <loguru.hpp>
#include <thread>
#include <vector>

void worker(int id) {
    loguru::set_thread_name(("worker_" + std::to_string(id)).c_str());
    
    for (int i = 0; i < 100; ++i) {
        LOG_F(INFO, "Worker %d iteration %d", id, i);
        // Safe to call from multiple threads concurrently
    }
}

int main() {
    loguru::init(0, nullptr);
    loguru::set_thread_name("main");
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker, i);
    }
    
    LOG_F(INFO, "All workers started");
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

### Thread naming
```cpp
#include <loguru.hpp>
#include <thread>

void thread_function(const std::string& name) {
    // Set thread name before any logging
    loguru::set_thread_name(name.c_str());
    
    LOG_F(INFO, "This will show thread name: %s", name.c_str());
}

int main() {
    loguru::init(0, nullptr);
    
    std::thread t1(thread_function, "network");
    std::thread t2(thread_function, "database");
    
    t1.join();
    t2.join();
    
    return 0;
}
```

### Concurrent file access
```cpp
#include <loguru.hpp>
#include <thread>

// Loguru handles concurrent writes to the same file safely
void concurrent_logging() {
    loguru::init(0, nullptr);
    loguru::add_file("shared.log", loguru::Append, loguru::Verbosity_MAX);
    
    auto writer = [](int id) {
        loguru::set_thread_name(("writer_" + std::to_string(id)).c_str());
        for (int i = 0; i < 1000; ++i) {
            LOG_F(INFO, "Message from writer %d", id);
            // All writers safely write to shared.log
        }
    };
    
    std::thread t1(writer, 1);
    std::thread t2(writer, 2);
    
    t1.join();
    t2.join();
}
```

### Thread-safe configuration
```cpp
#include <loguru.hpp>
#include <thread>
#include <atomic>

// BAD: Modifying global state from multiple threads
void unsafe_config() {
    std::thread t1([]() {
        loguru::g_stderr_verbosity = 1;  // Race condition!
    });
    std::thread t2([]() {
        loguru::g_stderr_verbosity = 2;  // Race condition!
    });
    t1.join();
    t2.join();
}

// GOOD: Configure before spawning threads
void safe_config() {
    loguru::g_stderr_verbosity = 1;  // Set before threads start
    loguru::init(0, nullptr);
    
    std::thread t1([]() {
        LOG_F(INFO, "Thread 1");  // Safe - config is stable
    });
    std::thread t2([]() {
        LOG_F(INFO, "Thread 2");  // Safe - config is stable
    });
    t1.join();
    t2.join();
}
```

### Thread-local error context
```cpp
#include <loguru.hpp>
#include <thread>

void thread_safe_context() {
    // Error context is thread-local
    loguru::set_error_context("Thread-specific data: %d", 42);
    
    // This context only applies to the current thread
    CHECK_F(false, "This will show thread-specific context");
}

int main() {
    loguru::init(0, nullptr);
    
    std::thread t1(thread_safe_context);
    std::thread t2(thread_safe_context);
    
    t1.join();
    t2.join();
    
    return 0;
}
```
# Best Practices

### Consistent initialization pattern
```cpp
#include <loguru.hpp>

int main(int argc, char* argv[]) {
    // Configure before init
    loguru::g_flush_interval_ms = 100;
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
    
    // Initialize
    loguru::init(argc, argv);
    
    // Add file outputs
    loguru::add_file("full.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file("errors.log", loguru::Truncate, loguru::Verbosity_ERROR);
    
    LOG_F(INFO, "Application initialized");
    return 0;
}
```

### Structured logging with scopes
```cpp
#include <loguru.hpp>

void process_order(int order_id) {
    LOG_SCOPE_F(INFO, "Processing order %d", order_id);
    
    LOG_F(INFO, "Validating order");
    validate_order(order_id);
    
    {
        LOG_SCOPE_F(2, "Payment processing");
        LOG_F(INFO, "Charging customer");
        process_payment(order_id);
    }  // Scope ends, indentation decreases
    
    LOG_F(INFO, "Order %d processed", order_id);
}
```

### Error context for debugging
```cpp
#include <loguru.hpp>

void critical_operation(int id, const std::string& data) {
    // Capture context that prints only on crash
    loguru::set_error_context("Operation ID: %d", id);
    loguru::set_error_context("Data size: %zu", data.size());
    
    CHECK_F(id > 0, "Invalid ID");
    CHECK_F(!data.empty(), "Empty data");
    
    // Clear context after operation
    loguru::set_error_context("");  // Reset
}
```

### Custom fatal handler for graceful recovery
```cpp
#include <loguru.hpp>
#include <exception>

int main() {
    loguru::init(0, nullptr);
    
    // Install custom handler
    loguru::set_fatal_handler([](const loguru::Message& message) {
        // Log to system error log
        std::cerr << "FATAL: " << message.message << std::endl;
        
        // Save crash state
        save_crash_state();
        
        // Throw instead of abort
        throw std::runtime_error(message.message);
    });
    
    try {
        // Application code
        run_application();
    } catch (const std::exception& e) {
        LOG_F(ERROR, "Application crashed: %s", e.what());
        return 1;
    }
    
    return 0;
}
```

### Thread naming convention
```cpp
#include <loguru.hpp>
#include <thread>
#include <vector>

void worker_thread(int id, const std::string& type) {
    // Use consistent naming: type_id
    loguru::set_thread_name((type + "_" + std::to_string(id)).c_str());
    
    LOG_F(INFO, "Worker started");
    // ... work ...
}

int main() {
    loguru::init(0, nullptr);
    loguru::set_thread_name("main");
    
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back(worker_thread, i, "worker");
    }
    
    for (auto& t : workers) {
        t.join();
    }
    
    return 0;
}
```

### Conditional logging for performance
```cpp
#include <loguru.hpp>

void process_data(const std::vector<int>& data) {
    // Avoid expensive string formatting when not needed
    if (loguru::g_stderr_verbosity >= loguru::Verbosity_INFO) {
        LOG_F(INFO, "Processing %zu elements", data.size());
    }
    
    // For very expensive checks
    if (loguru::g_stderr_verbosity >= 2) {
        auto expensive_info = compute_expensive_debug_info();
        LOG_F(2, "Debug: %s", expensive_info.c_str());
    }
}
```
# Best Practices

```cpp
// Best Practice: Create a reusable stack trace utility
#include "backward.hpp"
#include <string>
#include <sstream>

class StackTraceUtil {
public:
    static std::string get_trace(size_t max_frames = 32, size_t skip = 0) {
        backward::StackTrace st;
        st.load_here(max_frames + skip);
        st.skip_n_firsts(skip);
        
        backward::Printer p;
        p.address = true;
        p.snippet = true;
        p.color_mode = backward::ColorMode::never; // For logging
        
        std::stringstream ss;
        p.print(st, ss);
        return ss.str();
    }
    
    static void log_on_crash() {
        // Only register once
        static backward::SignalHandling sh;
    }
};

// Usage
int main() {
    StackTraceUtil::log_on_crash();
    std::cout << StackTraceUtil::get_trace(64, 1) << std::endl;
    return 0;
}
```

```cpp
// Best Practice: Conditional compilation for debug/release
#include "backward.hpp"

class DebugHelper {
public:
    static void print_stack_trace() {
#ifdef BACKWARD_DEBUG_ENABLED
        backward::StackTrace st;
        st.load_here(32);
        backward::Printer p;
        p.print(st);
#endif
    }
};

// In CMakeLists.txt:
// target_compile_definitions(mytarget PRIVATE BACKWARD_DEBUG_ENABLED=1)
```

```cpp
// Best Practice: Integrate with logging framework
#include "backward.hpp"
#include <spdlog/spdlog.h>
#include <memory>

class BackwardSink : public spdlog::sinks::sink {
    void log(const spdlog::details::log_msg& msg) override {
        if (msg.level >= spdlog::level::err) {
            backward::StackTrace st;
            st.load_here(32);
            backward::Printer p;
            p.address = true;
            p.snippet = false; // Don't read source files in production
            
            std::stringstream ss;
            p.print(st, ss);
            spdlog::default_logger()->log(msg.level, "Stack trace:\n{}", ss.str());
        }
    }
};
```

```cpp
// Best Practice: Cache resolved symbols for repeated use
#include "backward.hpp"
#include <unordered_map>
#include <mutex>

class SymbolCache {
    std::mutex mtx;
    std::unordered_map<uintptr_t, std::string> cache;
    
public:
    std::string resolve(void* addr) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = cache.find(reinterpret_cast<uintptr_t>(addr));
        if (it != cache.end()) return it->second;
        
        backward::ResolveTrace rt;
        rt.addr = addr;
        backward::TraceResolver tr;
        tr.resolve(rt);
        
        std::string result = rt.object_function;
        cache[reinterpret_cast<uintptr_t>(addr)] = result;
        return result;
    }
};
```
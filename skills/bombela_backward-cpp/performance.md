# Performance

```cpp
// Performance: Stack trace capture is expensive
#include "backward.hpp"
#include <chrono>
#include <iostream>

void measure_overhead() {
    auto start = std::chrono::high_resolution_clock::now();
    
    backward::StackTrace st;
    st.load_here(32);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Stack trace capture took " << duration.count() << " microseconds" << std::endl;
    // Typically 10-100 microseconds on modern hardware
}
```

```cpp
// Performance: Avoid repeated resolution of same addresses
#include "backward.hpp"
#include <unordered_map>

class EfficientResolver {
    backward::TraceResolver resolver_;
    std::unordered_map<uintptr_t, backward::ResolvedTrace> cache_;
    
public:
    backward::ResolvedTrace resolve(const backward::StackTrace::Frame& frame) {
        auto addr = reinterpret_cast<uintptr_t>(frame.addr);
        auto it = cache_.find(addr);
        if (it != cache_.end()) return it->second;
        
        backward::ResolvedTrace rt;
        rt.addr = frame.addr;
        resolver_.resolve(rt);
        
        cache_[addr] = rt;
        return rt;
    }
};
```

```cpp
// Performance: Limit frame count for speed
#include "backward.hpp"

void fast_trace() {
    backward::StackTrace st;
    st.load_here(16); // Fewer frames = faster capture
    // 16 frames is usually enough for debugging
}

void detailed_trace() {
    backward::StackTrace st;
    st.load_here(128); // More frames = slower but more complete
    // Use only when you need deep call chains
}
```

```cpp
// Performance: Memory allocation patterns
#include "backward.hpp"
#include <vector>

void allocation_aware() {
    // StackTrace allocates memory internally
    backward::StackTrace st;
    st.load_here(32); // Allocates for 32 frames
    
    // Printer allocates for formatting
    backward::Printer p;
    
    // For hot paths, pre-allocate and reuse
    static backward::StackTrace cached_st;
    static bool initialized = false;
    
    if (!initialized) {
        cached_st.load_here(32);
        initialized = true;
    }
    
    // But be careful: cached stack trace is from a different call site!
    // This is only useful for static analysis, not runtime debugging
}
```
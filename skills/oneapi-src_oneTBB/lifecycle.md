# Lifecycle

### Construction and initialization
```cpp
#include <oneapi/tbb.h>

// Default construction (uses global thread pool)
oneapi::tbb::task_arena arena;

// Construction with specific thread count
oneapi::tbb::task_arena arena_with_threads(4);

// Construction with priority
oneapi::tbb::task_arena arena_with_priority(4, 1);  // 4 threads, priority 1

// Global control initialization
oneapi::tbb::global_control gc(
    oneapi::tbb::global_control::max_allowed_parallelism, 4
);
```

### Destruction and cleanup
```cpp
// task_arena destructor waits for all tasks
{
    oneapi::tbb::task_arena arena(4);
    arena.execute([] {
        oneapi::tbb::parallel_for(0, 100, [](int i) {
            process(i);
        });
    });
}  // Arena destroyed, all tasks complete

// task_group must be waited on
{
    oneapi::tbb::task_group tg;
    tg.run([] { do_work(); });
    tg.wait();  // Required before destruction
}  // Safe destruction
```

### Move semantics
```cpp
// task_arena is movable
oneapi::tbb::task_arena arena1(4);
oneapi::tbb::task_arena arena2 = std::move(arena1);  // Move constructor
arena1 = std::move(arena2);  // Move assignment

// concurrent containers are movable
oneapi::tbb::concurrent_vector<int> vec1 = {1, 2, 3};
oneapi::tbb::concurrent_vector<int> vec2 = std::move(vec1);

// task_group is NOT movable
oneapi::tbb::task_group tg;
// oneapi::tbb::task_group tg2 = std::move(tg);  // Compile error
```

### Resource management
```cpp
// Use RAII for thread pool control
class ParallelProcessor {
    oneapi::tbb::task_arena arena;
    oneapi::tbb::global_control gc;
public:
    ParallelProcessor(int num_threads)
        : arena(num_threads)
        , gc(oneapi::tbb::global_control::max_allowed_parallelism, num_threads)
    {}
    
    void process(std::function<void()> task) {
        arena.execute(task);
    }
};

// Cleanup is automatic via destructors
{
    ParallelProcessor processor(4);
    processor.process([] { do_work(); });
}  // Resources released
```
# Pitfalls

```cpp
// PITFALL 1: Using _per_thread() data across translation units
// BAD: Creates executor in one TU, uses in another
// file1.cpp
#include <taskflow/taskflow.hpp>
tf::Executor& get_executor() {
    static tf::Executor exec(4);
    return exec;
}

// file2.cpp (different translation unit)
#include "file1.h"
void bad_example() {
    auto& exec = get_executor();
    // This may crash due to invalid per-thread data
    exec.run(taskflow).wait();
}

// GOOD: Keep executor usage within same translation unit
void good_example() {
    tf::Executor executor(4);
    tf::Taskflow taskflow;
    taskflow.emplace([](){});
    executor.run(taskflow).wait();
}

// PITFALL 2: Modifying task graph while it's running
// BAD: Modifying taskflow during execution
tf::Taskflow taskflow;
tf::Executor executor;
auto task = taskflow.emplace([](){});
executor.run(taskflow);
taskflow.emplace([](){});  // Undefined behavior!

// GOOD: Wait for completion before modification
executor.run(taskflow).wait();
taskflow.emplace([](){});  // Safe now

// PITFALL 3: Capturing references to destroyed objects
// BAD: Lambda captures dangling reference
void bad_capture() {
    tf::Taskflow taskflow;
    std::vector<int>* data = new std::vector<int>(100);
    taskflow.emplace([data]() { 
        data->push_back(42);  // data may be deleted
    });
    delete data;
}

// GOOD: Use shared ownership or ensure lifetime
void good_capture() {
    tf::Taskflow taskflow;
    auto data = std::make_shared<std::vector<int>>(100);
    taskflow.emplace([data]() { 
        data->push_back(42);  // Safe: shared_ptr keeps alive
    });
}

// PITFALL 4: Not calling wait() on async tasks
// BAD: Async tasks may not complete
void bad_async() {
    tf::Executor executor;
    executor.async([]() { /* critical work */ });
    // Program may exit before task completes
}

// GOOD: Wait for all async tasks
void good_async() {
    tf::Executor executor;
    executor.async([]() { /* critical work */ });
    executor.wait_for_all();  // Ensures completion
}

// PITFALL 5: Creating cycles in task graph
// BAD: Creates infinite loop
void bad_cycle() {
    tf::Taskflow taskflow;
    auto A = taskflow.emplace([](){});
    auto B = taskflow.emplace([](){});
    A.precede(B);
    B.precede(A);  // Cycle! Undefined behavior
}

// GOOD: Ensure DAG structure
void good_dag() {
    tf::Taskflow taskflow;
    auto A = taskflow.emplace([](){});
    auto B = taskflow.emplace([](){});
    auto C = taskflow.emplace([](){});
    A.precede(B);
    B.precede(C);  // Linear chain, no cycle
}

// PITFALL 6: Using condition tasks incorrectly
// BAD: Condition task returns non-integer
void bad_condition() {
    tf::Taskflow taskflow;
    auto cond = taskflow.emplace([]() -> double { 
        return 3.14;  // Must return size_t!
    }).name("cond");
}

// GOOD: Condition task returns size_t
void good_condition() {
    tf::Taskflow taskflow;
    auto cond = taskflow.emplace([]() -> size_t { 
        return std::rand() % 2; 
    }).name("cond");
}
```
# Quickstart

```cpp
// Quickstart: Common Taskflow Usage Patterns
#include <taskflow/taskflow.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

// Pattern 1: Basic Task Graph with Dependencies
void basic_task_graph() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    auto [A, B, C, D] = taskflow.emplace(
        []() { std::cout << "Task A\n"; },
        []() { std::cout << "Task B\n"; },
        []() { std::cout << "Task C\n"; },
        []() { std::cout << "Task D\n"; }
    );
    
    A.precede(B, C);  // A runs before B and C
    D.succeed(B, C);  // D runs after B and C
    
    executor.run(taskflow).wait();
}

// Pattern 2: Subflow Graph (Recursive Tasking)
void subflow_example() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    tf::Task A = taskflow.emplace([](){}).name("A");
    tf::Task B = taskflow.emplace([](tf::Subflow& subflow) {
        tf::Task B1 = subflow.emplace([](){}).name("B1");
        tf::Task B2 = subflow.emplace([](){}).name("B2");
        B1.precede(B2);
    }).name("B");
    
    A.precede(B);
    executor.run(taskflow).wait();
}

// Pattern 3: Conditional Tasking
void conditional_example() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    tf::Task init = taskflow.emplace([](){}).name("init");
    tf::Task cond = taskflow.emplace([]() { 
        return std::rand() % 2; 
    }).name("cond");
    tf::Task stop = taskflow.emplace([](){}).name("stop");
    
    init.precede(cond);
    cond.precede(cond, stop);  // Loop back or stop
}

// Pattern 4: Parallel Algorithms
void parallel_algorithms() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    std::vector<int> data(1000, 0);
    
    // Parallel for_each
    tf::Task for_each_task = taskflow.for_each(
        data.begin(), data.end(), 
        [](int& i) { i = 42; }
    );
    
    // Parallel reduce
    tf::Task reduce_task = taskflow.reduce(
        data.begin(), data.end(), 
        0, std::plus<int>{}
    );
    
    for_each_task.precede(reduce_task);
    executor.run(taskflow).wait();
}

// Pattern 5: Asynchronous Tasks
void async_example() {
    tf::Executor executor;
    
    // Simple async task
    std::future<int> future = executor.async([]() { 
        return 42; 
    });
    
    // Dependent async tasks
    tf::AsyncTask A = executor.silent_dependent_async([](){});
    tf::AsyncTask B = executor.silent_dependent_async([](){}, A);
    
    executor.wait_for_all();
}

// Pattern 6: Task Graph Composition
void composition_example() {
    tf::Taskflow f1, f2;
    
    // Create reusable module
    f1.emplace([](){}).name("f1A");
    f1.emplace([](){}).name("f1B");
    
    // Compose f1 into f2
    tf::Task module = f2.composed_of(f1).name("module");
    tf::Task f2A = f2.emplace([](){}).name("f2A");
    
    module.succeed(f2A);
}

int main() {
    basic_task_graph();
    subflow_example();
    conditional_example();
    parallel_algorithms();
    async_example();
    composition_example();
    return 0;
}
```
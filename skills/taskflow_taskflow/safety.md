# Safety

```cpp
// SAFETY RULE 1: NEVER access task graph from multiple threads without synchronization
// BAD: Concurrent modification
void bad_concurrent_modification() {
    tf::Taskflow taskflow;
    tf::Executor executor;
    
    std::thread t1([&]() { taskflow.emplace([](){}); });
    std::thread t2([&]() { taskflow.emplace([](){}); });
    // Race condition!
}

// GOOD: Serialize access or use separate taskflows
void good_serialized_access() {
    tf::Taskflow taskflow;
    tf::Executor executor;
    
    std::mutex mtx;
    std::thread t1([&]() { 
        std::lock_guard<std::mutex> lock(mtx);
        taskflow.emplace([](){}); 
    });
    std::thread t2([&]() { 
        std::lock_guard<std::mutex> lock(mtx);
        taskflow.emplace([](){}); 
    });
}

// SAFETY RULE 2: NEVER destroy executor while tasks are running
// BAD: Executor destroyed prematurely
void bad_executor_lifetime() {
    tf::Taskflow taskflow;
    taskflow.emplace([]() { std::this_thread::sleep_for(1s); });
    
    {
        tf::Executor executor;
        executor.run(taskflow);
    }  // Executor destroyed, tasks may still run!
}

// GOOD: Ensure executor outlives all tasks
void good_executor_lifetime() {
    tf::Taskflow taskflow;
    taskflow.emplace([]() { std::this_thread::sleep_for(1s); });
    
    tf::Executor executor;
    executor.run(taskflow).wait();  // Wait for completion
}  // Safe to destroy now

// SAFETY RULE 3: NEVER create subflows outside of task execution
// BAD: Creating subflow outside task
void bad_subflow_creation() {
    tf::Taskflow taskflow;
    tf::Subflow subflow;  // Cannot create Subflow directly!
}

// GOOD: Subflows only created within task lambdas
void good_subflow_creation() {
    tf::Taskflow taskflow;
    taskflow.emplace([](tf::Subflow& subflow) {
        subflow.emplace([](){});
    });
}

// SAFETY RULE 4: NEVER use moved-from task objects
// BAD: Using task after move
void bad_moved_task() {
    tf::Taskflow taskflow;
    auto A = taskflow.emplace([](){});
    auto B = std::move(A);
    A.precede(B);  // A is in moved-from state!
}

// GOOD: Use the moved-to object
void good_moved_task() {
    tf::Taskflow taskflow;
    auto A = taskflow.emplace([](){});
    auto B = std::move(A);
    B.precede(taskflow.emplace([](){}));  // Use B, not A
}

// SAFETY RULE 5: NEVER assume task execution order without explicit dependencies
// BAD: Assuming sequential execution
void bad_order_assumption() {
    tf::Taskflow taskflow;
    int shared_var = 0;
    taskflow.emplace([&]() { shared_var = 1; });
    taskflow.emplace([&]() { 
        // May execute before or after first task!
        assert(shared_var == 1);  // May fail!
    });
}

// GOOD: Explicit dependencies
void good_order_control() {
    tf::Taskflow taskflow;
    int shared_var = 0;
    auto A = taskflow.emplace([&]() { shared_var = 1; });
    auto B = taskflow.emplace([&]() { 
        assert(shared_var == 1);  // Guaranteed after A
    });
    A.precede(B);  // Explicit dependency
}
```
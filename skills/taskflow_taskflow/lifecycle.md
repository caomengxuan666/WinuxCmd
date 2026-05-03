# Lifecycle

```cpp
// LIFECYCLE: Construction, Destruction, Move Semantics, Resource Management
#include <taskflow/taskflow.hpp>
#include <iostream>
#include <cassert>

// 1. Construction
void construction_examples() {
    // Default constructor - creates empty taskflow
    tf::Taskflow empty_flow;
    assert(empty_flow.num_tasks() == 0);
    
    // Copy constructor (deleted - Taskflow is move-only)
    // tf::Taskflow copy = empty_flow;  // Compile error!
    
    // Move constructor
    tf::Taskflow source;
    source.emplace([](){});
    tf::Taskflow dest = std::move(source);
    assert(dest.num_tasks() == 1);
    assert(source.num_tasks() == 0);  // Source is empty after move
}

// 2. Task Lifecycle
void task_lifecycle() {
    tf::Taskflow taskflow;
    
    // Task creation
    auto task = taskflow.emplace([]() { 
        std::cout << "Task executing\n"; 
    }).name("my_task");
    
    // Task is valid until taskflow is destroyed or moved
    assert(task.name() == "my_task");
    
    // Task can be renamed
    task.name("renamed_task");
    assert(task.name() == "renamed_task");
    
    // Task handles remain valid after adding more tasks
    auto task2 = taskflow.emplace([](){});
    assert(task.name() == "renamed_task");  // Still valid
}

// 3. Move Semantics
void move_semantics() {
    tf::Taskflow flow1, flow2;
    
    // Create tasks in flow1
    auto a = flow1.emplace([](){}).name("A");
    auto b = flow1.emplace([](){}).name("B");
    a.precede(b);
    
    // Move flow1 to flow2
    flow2 = std::move(flow1);
    
    // flow1 is now empty, flow2 has the tasks
    assert(flow1.num_tasks() == 0);
    assert(flow2.num_tasks() == 2);
    
    // Task handles from flow1 are now associated with flow2
    // a and b are still valid and point to tasks in flow2
}

// 4. Executor Lifecycle
void executor_lifecycle() {
    // Executor creates worker threads on construction
    tf::Executor executor(4);  // 4 worker threads
    
    // Executor can be moved
    tf::Executor executor2 = std::move(executor);
    
    // Original executor is now empty
    // executor.run(taskflow);  // Would crash!
    
    // Executor destructor waits for running tasks
    {
        tf::Executor temp_exec(2);
        tf::Taskflow taskflow;
        taskflow.emplace([]() { 
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        });
        temp_exec.run(taskflow);
    }  // temp_exec destructor waits for completion
}

// 5. Resource Management with Taskflow
class TaskflowManager {
    tf::Taskflow taskflow;
    tf::Executor executor;
    std::vector<std::shared_ptr<int>> shared_resources;
    
public:
    TaskflowManager(size_t num_threads) 
        : executor(num_threads) {}
    
    void add_task_with_resource() {
        auto resource = std::make_shared<int>(42);
        shared_resources.push_back(resource);
        
        taskflow.emplace([resource]() {
            // Safe: shared_ptr keeps resource alive
            *resource *= 2;
        });
    }
    
    void execute() {
        executor.run(taskflow).wait();
    }
    
    // Move constructor
    TaskflowManager(TaskflowManager&& other) noexcept
        : taskflow(std::move(other.taskflow))
        , executor(std::move(other.executor))
        , shared_resources(std::move(other.shared_resources)) {}
};

// 6. Dump and Restore (for debugging)
void dump_and_restore() {
    tf::Taskflow taskflow;
    
    auto a = taskflow.emplace([](){}).name("A");
    auto b = taskflow.emplace([](){}).name("B");
    a.precede(b);
    
    // Dump taskflow to DOT format
    std::cout << taskflow.dump() << std::endl;
    // Output:
    // digraph Taskflow {
    //   "A" -> "B";
    // }
}

int main() {
    construction_examples();
    task_lifecycle();
    move_semantics();
    executor_lifecycle();
    
    TaskflowManager manager(4);
    manager.add_task_with_resource();
    manager.execute();
    
    dump_and_restore();
    
    return 0;
}
```
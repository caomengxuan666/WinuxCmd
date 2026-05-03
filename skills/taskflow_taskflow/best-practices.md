# Best Practices

```cpp
// BEST PRACTICE 1: Name tasks for debugging
#include <taskflow/taskflow.hpp>
#include <iostream>

void named_tasks() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    auto task = taskflow.emplace([]() { 
        std::cout << "Processing data\n"; 
    }).name("data_processing");
    
    executor.run(taskflow).wait();
}

// BEST PRACTICE 2: Use RAII for resource management
class ResourceManager {
    std::vector<int> data;
public:
    ResourceManager(size_t size) : data(size) {}
    
    tf::Taskflow create_taskflow() {
        tf::Taskflow taskflow;
        taskflow.emplace([this]() { 
            process_data(); 
        }).name("process");
        return taskflow;
    }
    
private:
    void process_data() {
        std::for_each(data.begin(), data.end(), [](int& x) { x *= 2; });
    }
};

// BEST PRACTICE 3: Batch task creation for performance
void batch_creation() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    constexpr size_t NUM_TASKS = 1000;
    std::vector<tf::Task> tasks;
    tasks.reserve(NUM_TASKS);
    
    // Batch create tasks
    for (size_t i = 0; i < NUM_TASKS; ++i) {
        tasks.push_back(taskflow.emplace([i]() {
            // Task work
        }).name("task_" + std::to_string(i)));
    }
    
    // Add dependencies in bulk
    for (size_t i = 1; i < NUM_TASKS; ++i) {
        tasks[i-1].precede(tasks[i]);
    }
    
    executor.run(taskflow).wait();
}

// BEST PRACTICE 4: Use modules for reusable components
class DataPipeline {
    tf::Taskflow pipeline;
public:
    DataPipeline() {
        auto read = pipeline.emplace([](){}).name("read");
        auto process = pipeline.emplace([](){}).name("process");
        auto write = pipeline.emplace([](){}).name("write");
        read.precede(process);
        process.precede(write);
    }
    
    tf::Taskflow& get() { return pipeline; }
};

void use_pipeline() {
    tf::Executor executor;
    tf::Taskflow main_flow;
    
    DataPipeline pipeline;
    auto module_task = main_flow.composed_of(pipeline.get()).name("pipeline");
    
    auto init = main_flow.emplace([](){}).name("init");
    auto cleanup = main_flow.emplace([](){}).name("cleanup");
    
    init.precede(module_task);
    module_task.precede(cleanup);
    
    executor.run(main_flow).wait();
}

// BEST PRACTICE 5: Error handling in tasks
void error_handling() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    std::atomic<bool> has_error{false};
    
    taskflow.emplace([&has_error]() {
        try {
            // Risky operation
            throw std::runtime_error("Task failed");
        } catch (const std::exception& e) {
            has_error = true;
            std::cerr << "Error: " << e.what() << "\n";
        }
    }).name("risky_task");
    
    executor.run(taskflow).wait();
    
    if (has_error) {
        std::cout << "Pipeline completed with errors\n";
    }
}

// BEST PRACTICE 6: Profile with TFProf
void profiling_example() {
    // Run with: TF_ENABLE_PROFILER=output.tfp ./program
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    // Your task graph here
    executor.run(taskflow).wait();
    // Visualize output.tfp at https://taskflow.github.io/tfprof/
}

int main() {
    named_tasks();
    
    ResourceManager manager(1000);
    auto taskflow = manager.create_taskflow();
    tf::Executor executor;
    executor.run(taskflow).wait();
    
    batch_creation();
    use_pipeline();
    error_handling();
    
    return 0;
}
```
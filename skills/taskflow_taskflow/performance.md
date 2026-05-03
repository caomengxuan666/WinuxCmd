# Performance

```cpp
// PERFORMANCE: Characteristics, Allocation Patterns, Optimization Tips
#include <taskflow/taskflow.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>

// 1. Task Creation Overhead
void task_creation_overhead() {
    tf::Executor executor;
    
    // BAD: Creating tasks one by one
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        tf::Taskflow taskflow;
        taskflow.emplace([](){});
        executor.run(taskflow).wait();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Individual creation: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
    
    // GOOD: Batch task creation
    start = std::chrono::high_resolution_clock::now();
    tf::Taskflow batch_flow;
    for (int i = 0; i < 10000; ++i) {
        batch_flow.emplace([](){});
    }
    executor.run(batch_flow).wait();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Batch creation: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}

// 2. Work Stealing Performance
void work_stealing_benchmark() {
    tf::Executor executor(std::thread::hardware_concurrency());
    tf::Taskflow taskflow;
    
    constexpr size_t NUM_TASKS = 100000;
    std::vector<int> results(NUM_TASKS, 0);
    
    // Create many small tasks for work stealing
    for (size_t i = 0; i < NUM_TASKS; ++i) {
        taskflow.emplace([&results, i]() {
            results[i] = i * i;
        });
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    executor.run(taskflow).wait();
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "Work stealing throughput: "
              << NUM_TASKS * 1000.0 / 
                 std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " tasks/ms\n";
}

// 3. Memory Allocation Patterns
void allocation_patterns() {
    // BAD: Frequent allocations in tasks
    tf::Taskflow bad_flow;
    for (int i = 0; i < 1000; ++i) {
        bad_flow.emplace([]() {
            std::vector<int> data(1000);  // Allocated on each execution
            std::iota(data.begin(), data.end(), 0);
        });
    }
    
    // GOOD: Pre-allocate and reuse
    tf::Taskflow good_flow;
    auto shared_data = std::make_shared<std::vector<int>>(1000);
    for (int i = 0; i < 1000; ++i) {
        good_flow.emplace([shared_data]() {
            std::iota(shared_data->begin(), shared_data->end(), 0);
        });
    }
}

// 4. Optimizing with Task Priorities
void task_priorities() {
    tf::Executor executor(4);
    tf::Taskflow taskflow;
    
    // High priority tasks execute first
    auto critical = taskflow.emplace([]() {
        // Critical work
    }).name("critical");
    
    auto normal = taskflow.emplace([]() {
        // Normal work
    }).name("normal");
    
    // Note: Taskflow doesn't have explicit priority API
    // Use dependency ordering instead
    critical.precede(normal);  // Critical runs first
}

// 5. Reducing Synchronization Overhead
void reduce_synchronization() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    // BAD: Too many dependencies
    std::vector<tf::Task> tasks;
    for (int i = 0; i < 100; ++i) {
        tasks.push_back(taskflow.emplace([](){}));
    }
    // O(n^2) dependencies
    for (size_t i = 0; i < tasks.size(); ++i) {
        for (size_t j = i + 1; j < tasks.size(); ++j) {
            tasks[i].precede(tasks[j]);
        }
    }
    
    // GOOD: Linear dependencies
    tf::Taskflow good_flow;
    std::vector<tf::Task> good_tasks;
    for (int i = 0; i < 100; ++i) {
        good_tasks.push_back(good_flow.emplace([](){}));
    }
    // O(n) dependencies
    for (size_t i = 1; i < good_tasks.size(); ++i) {
        good_tasks[i-1].precede(good_tasks[i]);
    }
}

// 6. Using Parallel Algorithms
void parallel_algorithms_performance() {
    tf::Executor executor;
    tf::Taskflow taskflow;
    
    std::vector<int> data(1000000);
    std::iota(data.begin(), data.end(), 0);
    
    // Taskflow's parallel algorithms are optimized
    auto start = std::chrono::high_resolution_clock::now();
    
    tf::Task sort_task = taskflow.sort(data.begin(), data.end());
    tf::Task reduce_task = taskflow.reduce(
        data.begin(), data.end(), 
        0, std::plus<int>{}
    );
    
    sort_task.precede(reduce_task);
    executor.run(taskflow).wait();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Parallel sort+reduce: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
}

int main() {
    task_creation_overhead();
    work_stealing_benchmark();
    allocation_patterns();
    task_priorities();
    reduce_synchronization();
    parallel_algorithms_performance();
    
    return 0;
}
```
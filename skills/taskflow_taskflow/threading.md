# Threading

```cpp
// THREADING: Thread Safety Guarantees, Concurrent Access Patterns
#include <taskflow/taskflow.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

// 1. Executor Thread Safety
void executor_thread_safety() {
    tf::Executor executor(4);
    tf::Taskflow taskflow;
    
    // Executor::run() is thread-safe
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&executor, &taskflow]() {
            executor.run(taskflow);  // Safe: run() is thread-safe
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    executor.wait_for_all();
}

// 2. Taskflow Thread Safety (NOT thread-safe)
void taskflow_thread_unsafety() {
    tf::Taskflow taskflow;
    
    // BAD: Concurrent modification of taskflow
    std::mutex mtx;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&taskflow, &mtx]() {
            std::lock_guard<std::mutex> lock(mtx);  // Must synchronize!
            taskflow.emplace([](){});
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

// 3. Worker-Specific Data
void worker_local_data() {
    tf::Executor executor(4);
    tf::Taskflow taskflow;
    
    // Each worker thread has its own data
    taskflow.emplace([]() {
        // Get current worker ID (thread-safe within task execution)
        // Note: this_worker_id() only works from within a running task
        // auto worker_id = executor.this_worker_id();  // Not directly available
    });
    
    executor.run(taskflow).wait();
}

// 4. Concurrent Async Tasks
void concurrent_async() {
    tf::Executor executor(4);
    std::atomic<int> counter{0};
    
    // Async tasks are thread-safe
    std::vector<std::future<int>> futures;
    
    for (int i = 0; i < 100; ++i) {
        futures.push_back(executor.async([&counter]() {
            return counter.fetch_add(1);
        }));
    }
    
    // Wait for all async tasks
    for (auto& f : futures) {
        f.get();
    }
    
    std::cout << "Counter: " << counter << " (expected: 100)\n";
}

// 5. Shared State Protection
void shared_state_protection() {
    tf::Executor executor(4);
    tf::Taskflow taskflow;
    
    std::mutex mtx;
    std::vector<int> shared_data;
    
    // Tasks must protect shared state
    for (int i = 0; i < 10; ++i) {
        taskflow.emplace([&mtx, &shared_data, i]() {
            std::lock_guard<std::mutex> lock(mtx);
            shared_data.push_back(i);
        });
    }
    
    executor.run(taskflow).wait();
    
    std::cout << "Shared data size: " << shared_data.size() << "\n";
}

// 6. Thread-Safe Task Graph Building
class ThreadSafeTaskflowBuilder {
    tf::Taskflow taskflow;
    std::mutex mtx;
    
public:
    void add_task(std::function<void()> func, const std::string& name = "") {
        std::lock_guard<std::mutex> lock(mtx);
        auto task = taskflow.emplace(std::move(func));
        if (!name.empty()) {
            task.name(name);
        }
    }
    
    tf::Taskflow finalize() {
        std::lock_guard<std::mutex> lock(mtx);
        return std::move(taskflow);  // Move to caller
    }
};

void use_thread_safe_builder() {
    ThreadSafeTaskflowBuilder builder;
    tf::Executor executor(4);
    
    // Build task graph from multiple threads
    std::vector<std::thread> builders;
    for (int i = 0; i < 4; ++i) {
        builders.emplace_back([&builder, i]() {
            builder.add_task([i]() {
                std::cout << "Task from thread " << i << "\n";
            }, "task_" + std::to_string(i));
        });
    }
    
    for (auto& t : builders) {
        t.join();
    }
    
    // Finalize and execute
    auto taskflow = builder.finalize();
    executor.run(taskflow).wait();
}

// 7. Thread Safety Summary
void thread_safety_summary() {
    std::cout << "Thread Safety Guarantees:\n";
    std::cout << "  tf::Executor::run() - Thread-safe\n";
    std::cout << "  tf::Executor::async() - Thread-safe\n";
    std::cout << "  tf::Executor::wait_for_all() - Thread-safe\n";
    std::cout << "  tf::Taskflow::emplace() - NOT thread-safe\n";
    std::cout << "  tf::Task::precede() - NOT thread-safe\n";
    std::cout << "  tf::Task::succeed() - NOT thread-safe\n";
    std::cout << "  Task execution - Thread-safe within task\n";
}

int main() {
    executor_thread_safety();
    taskflow_thread_unsafety();
    worker_local_data();
    concurrent_async();
    shared_state_protection();
    use_thread_safe_builder();
    thread_safety_summary();
    
    return 0;
}
```
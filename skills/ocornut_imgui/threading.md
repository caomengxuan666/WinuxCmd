# Threading

**Thread safety: ImGui is NOT thread-safe by default**
```cpp
// BAD: Calling ImGui from worker threads
std::thread worker([]() {
    ImGui::Text("From thread"); // CRASH - not thread safe
});
worker.join();

// GOOD: All ImGui calls from main thread
std::thread worker([]() {
    // Do work here
    result = ComputeSomething();
});
worker.join();
ImGui::Text("Result: %d", result); // Safe on main thread
```

**Safe pattern: Queue work for main thread**
```cpp
std::mutex g_mutex;
std::vector<std::function<void()>> g_ui_tasks;

void WorkerThread() {
    // Compute something
    int result = ExpensiveCalculation();
    
    // Queue UI update
    std::lock_guard<std::mutex> lock(g_mutex);
    g_ui_tasks.push_back([result]() {
        ImGui::Text("Result: %d", result);
    });
}

void MainThreadUpdate() {
    std::lock_guard<std::mutex> lock(g_mutex);
    for (auto& task : g_ui_tasks) {
        task(); // Execute on main thread
    }
    g_ui_tasks.clear();
}
```

**Safe pattern: Use atomic flags for thread communication**
```cpp
std::atomic<bool> g_data_ready{false};
std::atomic<int> g_computed_value{0};

void WorkerThread() {
    g_computed_value = ExpensiveCalculation();
    g_data_ready = true;
}

void MainThreadRender() {
    if (g_data_ready.exchange(false)) {
        ImGui::Text("Computed: %d", g_computed_value.load());
    }
}
```

**Limitations:**
- **No concurrent ImGui calls** from multiple threads
- **IO struct** can be read from any thread but NOT written
- **Draw data** must be consumed on the same thread that called Render()
- **Font atlas** creation is single-threaded
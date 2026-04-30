# Threading

### Thread Safety Guarantees
```cpp
// Ceres Solver is NOT thread-safe for Problem modification
// Only Solve() can be called from multiple threads

// Safe: Multiple solves on different problems
std::vector<ceres::Problem> problems(4);
std::vector<ceres::Solver::Summary> summaries(4);
std::vector<std::thread> threads;

for (int i = 0; i < 4; ++i) {
    threads.emplace_back([&problems, &summaries, i]() {
        ceres::Solver::Options options;
        options.num_threads = 1;  // Each thread solves its own problem
        ceres::Solve(options, &problems[i], &summaries[i]);
    });
}

for (auto& t : threads) {
    t.join();
}
```

### Concurrent Access Patterns
```cpp
// BAD: Concurrent modification of same problem
ceres::Problem problem;
std::thread t1([&]() {
    problem.AddResidualBlock(cost1, nullptr, &x);
});
std::thread t2([&]() {
    problem.AddResidualBlock(cost2, nullptr, &x);
});
t1.join();
t2.join();  // Race condition!

// GOOD: Build problem sequentially, then solve with multiple threads
ceres::Problem problem;
// Build in single thread
for (int i = 0; i < 1000; ++i) {
    problem.AddResidualBlock(cost, nullptr, &x);
}

// Solve with multiple threads
ceres::Solver::Options options;
options.num_threads = 4;  // Thread-safe during Solve
ceres::Solve(options, &problem, &summary);
```

### Thread-Safe Cost Functors
```cpp
// Cost functors must be thread-safe (no mutable state)
struct ThreadSafeCostFunctor {
    ThreadSafeCostFunctor(double observed) : observed_(observed) {}
    
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        residual[0] = T(observed_) - x[0];
        return true;
    }
    
private:
    const double observed_;  // Immutable after construction
};

// BAD: Mutable state in cost functor
struct UnsafeCostFunctor {
    mutable int call_count = 0;  // Race condition!
    
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        ++call_count;
        residual[0] = T(10.0) - x[0];
        return true;
    }
};
```

### Parallel Problem Building
```cpp
// Use thread-local problems for parallel construction
std::vector<ceres::Problem> local_problems(4);
std::vector<double> local_params(4, 0.5);

#pragma omp parallel for
for (int i = 0; i < 1000; ++i) {
    int tid = omp_get_thread_num();
    local_problems[tid].AddResidualBlock(
        new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor),
        nullptr, &local_params[tid]);
}

// Merge problems (requires custom implementation)
ceres::Problem merged_problem;
for (auto& lp : local_problems) {
    // Transfer residual blocks from local to merged
    // Note: Ceres doesn't provide direct merge API
}
```
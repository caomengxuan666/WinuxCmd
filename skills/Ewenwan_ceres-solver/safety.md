# Safety

### 1. NEVER pass null pointers to AddResidualBlock
BAD:
```cpp
double* ptr = nullptr;
problem.AddResidualBlock(cost_function, nullptr, ptr);
```
GOOD:
```cpp
double x = 0.0;
problem.AddResidualBlock(cost_function, nullptr, &x);
```

### 2. NEVER use the same parameter block in multiple problems simultaneously
BAD:
```cpp
double x = 0.5;
ceres::Problem problem1, problem2;
problem1.AddResidualBlock(cost1, nullptr, &x);
problem2.AddResidualBlock(cost2, nullptr, &x);  // Undefined behavior
```
GOOD:
```cpp
double x1 = 0.5, x2 = 0.5;
ceres::Problem problem1, problem2;
problem1.AddResidualBlock(cost1, nullptr, &x1);
problem2.AddResidualBlock(cost2, nullptr, &x2);
```

### 3. NEVER call Solve on a problem with zero residual blocks
BAD:
```cpp
ceres::Problem empty_problem;
ceres::Solve(options, &empty_problem, &summary);  // Crash
```
GOOD:
```cpp
if (problem.NumResidualBlocks() > 0) {
    ceres::Solve(options, &problem, &summary);
} else {
    std::cerr << "No residual blocks to optimize\n";
}
```

### 4. NEVER modify parameter values while solver is running
BAD:
```cpp
std::thread solver_thread([&]() {
    ceres::Solve(options, &problem, &summary);
});
x = 10.0;  // Race condition
solver_thread.join();
```
GOOD:
```cpp
ceres::Solve(options, &problem, &summary);
x = 10.0;  // Safe after solver completes
```

### 5. NEVER use AutoDiffCostFunction with non-differentiable operations
BAD:
```cpp
struct BadFunctor {
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        residual[0] = abs(x[0]);  // Not differentiable at 0
        return true;
    }
};
```
GOOD:
```cpp
struct GoodFunctor {
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        residual[0] = x[0] * x[0];  // Smooth and differentiable
        return true;
    }
};
```
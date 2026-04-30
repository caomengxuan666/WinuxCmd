# Pitfalls

### 1. Missing Include for AutoDiff
BAD:
```cpp
struct CostFunctor {
    bool operator()(const double* x, double* residual) const {
        residual[0] = 10.0 - x[0];
        return true;
    }
};
```
GOOD:
```cpp
#include "ceres/ceres.h"
struct CostFunctor {
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        residual[0] = T(10.0) - x[0];
        return true;
    }
};
```

### 2. Incorrect Residual Block Size
BAD:
```cpp
// Residual dimension 2, but functor computes 1
ceres::CostFunction* cost_function =
    new ceres::AutoDiffCostFunction<CostFunctor, 2, 1>(new CostFunctor);
```
GOOD:
```cpp
// Match residual dimension to functor output
ceres::CostFunction* cost_function =
    new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
```

### 3. Forgetting to Set Initial Parameter Values
BAD:
```cpp
double x;  // Uninitialized
problem.AddResidualBlock(cost_function, nullptr, &x);
```
GOOD:
```cpp
double x = 0.5;  // Good initial guess
problem.AddResidualBlock(cost_function, nullptr, &x);
```

### 4. Using Raw Pointers Without Ownership
BAD:
```cpp
auto cost = new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
// Never added to problem - memory leak
```
GOOD:
```cpp
problem.AddResidualBlock(
    new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor),
    nullptr, &x);
// Problem takes ownership
```

### 5. Ignoring Solver Summary
BAD:
```cpp
ceres::Solve(options, &problem, &summary);
// Assume success
```
GOOD:
```cpp
ceres::Solve(options, &problem, &summary);
if (!summary.IsSolutionUsable()) {
    std::cerr << "Solver failed: " << summary.FullReport() << "\n";
    return 1;
}
```

### 6. Incorrect Parameter Block Ordering
BAD:
```cpp
// Functor expects [a, b] but we pass [b, a]
problem.AddResidualBlock(cost_function, nullptr, &b, &a);
```
GOOD:
```cpp
// Match parameter order to functor template arguments
problem.AddResidualBlock(cost_function, nullptr, &a, &b);
```

### 7. Not Using Loss Functions for Outliers
BAD:
```cpp
problem.AddResidualBlock(cost_function, nullptr, &x);
// No robust loss - outliers dominate
```
GOOD:
```cpp
problem.AddResidualBlock(cost_function, 
    new ceres::HuberLoss(1.0), &x);
```

### 8. Modifying Parameters During Solve
BAD:
```cpp
ceres::Solve(options, &problem, &summary);
x = 5.0;  // Modifies parameter while solver might be using it
```
GOOD:
```cpp
ceres::Solve(options, &problem, &summary);
// Only modify after solver completes
double result = x;
```
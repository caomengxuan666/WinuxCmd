# Performance

### Memory Allocation Patterns
```cpp
// Prefer stack allocation for small problems
double parameters[3] = {1.0, 2.0, 3.0};

// Use heap for large parameter blocks
std::vector<double> large_params(1000000, 0.5);
problem.AddParameterBlock(large_params.data(), large_params.size());
```

### Solver Performance Characteristics
```cpp
// For small problems (< 100 parameters)
ceres::Solver::Options options;
options.linear_solver_type = ceres::DENSE_QR;
options.minimizer_type = ceres::TRUST_REGION;

// For large sparse problems
options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
options.sparse_linear_algebra_library_type = ceres::SUITE_SPARSE;

// For very large problems
options.linear_solver_type = ceres::ITERATIVE_SCHUR;
options.preconditioner_type = ceres::SCHUR_JACOBI;
```

### Optimization Tips
```cpp
// 1. Reuse cost functions when possible
auto* cost = new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
for (int i = 0; i < 1000; ++i) {
    problem.AddResidualBlock(cost, nullptr, &x);  // Reuse same cost
}

// 2. Use numeric differentiation for complex functors
struct ComplexFunctor {
    bool operator()(const double* x, double* residual) const {
        // Complex computation that's hard to template
        residual[0] = expensive_function(x[0]);
        return true;
    }
};
problem.AddResidualBlock(
    new ceres::NumericDiffCostFunction<ComplexFunctor, 
        ceres::CENTRAL, 1, 1>(new ComplexFunctor),
    nullptr, &x);

// 3. Set appropriate tolerances
options.function_tolerance = 1e-4;  // Looser for speed
options.gradient_tolerance = 1e-6;
options.parameter_tolerance = 1e-4;
```

### Benchmarking
```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();
ceres::Solve(options, &problem, &summary);
auto end = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Solve time: " << duration.count() << " ms\n";
std::cout << "Iterations: " << summary.num_successful_steps << "\n";
```
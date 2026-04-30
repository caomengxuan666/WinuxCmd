# Lifecycle

### Construction
```cpp
// Default construction
ceres::Problem problem;

// With options
ceres::Problem::Options problem_options;
problem_options.loss_function_ownership = ceres::DO_NOT_TAKE_OWNERSHIP;
ceres::Problem problem(problem_options);

// Move construction
ceres::Problem problem2 = std::move(problem);
```

### Destruction
```cpp
{
    ceres::Problem problem;
    double x = 0.5;
    problem.AddResidualBlock(
        new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor),
        nullptr, &x);
    // Problem destructor cleans up owned cost functions and loss functions
}  // problem destroyed here
```

### Resource Management
```cpp
// Problem takes ownership by default
{
    ceres::Problem problem;
    auto* cost = new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
    problem.AddResidualBlock(cost, nullptr, &x);
    // Do NOT delete cost manually - problem owns it
}

// To manage memory manually
ceres::Problem::Options options;
options.cost_function_ownership = ceres::DO_NOT_TAKE_OWNERSHIP;
ceres::Problem problem(options);
auto* cost = new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
problem.AddResidualBlock(cost, nullptr, &x);
// Must delete cost manually after problem is destroyed
delete cost;
```

### Move Semantics
```cpp
ceres::Problem CreateProblem() {
    ceres::Problem problem;
    double x = 0.5;
    problem.AddResidualBlock(
        new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor),
        nullptr, &x);
    return problem;  // Move return
}

ceres::Problem p = CreateProblem();
ceres::Problem p2 = std::move(p);  // p is now empty
```

### Solver Lifecycle
```cpp
// Solver options are lightweight
ceres::Solver::Options options;
options.max_num_iterations = 100;

// Summary is populated by Solve
ceres::Solver::Summary summary;
ceres::Solve(options, &problem, &summary);

// Summary can be copied
ceres::Solver::Summary summary_copy = summary;
```
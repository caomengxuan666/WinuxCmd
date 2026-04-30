# Quickstart

```cpp
#include "ceres/ceres.h"
#include <iostream>

// 1. Simple cost function: f(x) = 10 - x
struct CostFunctor {
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        residual[0] = T(10.0) - x[0];
        return true;
    }
};

int main() {
    // 2. Problem setup
    double x = 0.5;
    ceres::Problem problem;
    
    // 3. Add residual block
    ceres::CostFunction* cost_function =
        new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
    problem.AddResidualBlock(cost_function, nullptr, &x);
    
    // 4. Configure solver
    ceres::Solver::Options options;
    options.minimizer_progress_to_stdout = true;
    options.max_num_iterations = 100;
    
    // 5. Solve
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);
    
    // 6. Output results
    std::cout << summary.BriefReport() << "\n";
    std::cout << "x = " << x << "\n";
    
    // 7. Check convergence
    if (!summary.IsSolutionUsable()) {
        std::cerr << "Solution not usable\n";
        return 1;
    }
    
    // 8. Access covariance
    ceres::Covariance::Options cov_options;
    ceres::Covariance covariance(cov_options);
    std::vector<std::pair<const double*, const double*>> covariance_blocks;
    covariance_blocks.push_back(std::make_pair(&x, &x));
    covariance.Compute(covariance_blocks, &problem);
    double cov_xx;
    covariance.GetCovarianceBlock(&x, &x, &cov_xx);
    std::cout << "Covariance: " << cov_xx << "\n";
    
    return 0;
}
```
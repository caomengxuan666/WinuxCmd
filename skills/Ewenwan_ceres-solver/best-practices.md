# Best Practices

### 1. Use LocalParameterization for Constrained Parameters
```cpp
#include "ceres/ceres.h"
#include "ceres/rotation.h"

// For quaternion normalization
ceres::LocalParameterization* quaternion_parameterization =
    new ceres::QuaternionParameterization;
problem.AddParameterBlock(quaternion, 4, quaternion_parameterization);
```

### 2. Structure Cost Functors with Clear Naming
```cpp
struct ReprojectionError {
    ReprojectionError(double observed_x, double observed_y)
        : observed_x_(observed_x), observed_y_(observed_y) {}
    
    template <typename T>
    bool operator()(const T* const camera,
                    const T* const point,
                    T* residuals) const {
        // Compute reprojection error
        T predicted_x, predicted_y;
        ComputeProjection(camera, point, &predicted_x, &predicted_y);
        residuals[0] = predicted_x - T(observed_x_);
        residuals[1] = predicted_y - T(observed_y_);
        return true;
    }
    
private:
    double observed_x_, observed_y_;
};
```

### 3. Use Solver Options for Production
```cpp
ceres::Solver::Options options;
options.minimizer_progress_to_stdout = false;  // Disable in production
options.max_num_iterations = 50;
options.function_tolerance = 1e-6;
options.gradient_tolerance = 1e-10;
options.parameter_tolerance = 1e-8;
options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
options.num_threads = 4;  // Use multiple threads
```

### 4. Validate Problem Before Solving
```cpp
bool ValidateProblem(const ceres::Problem& problem) {
    if (problem.NumResidualBlocks() == 0) return false;
    if (problem.NumParameterBlocks() == 0) return false;
    
    // Check for NaN in parameters
    double* parameters;
    problem.GetParameterBlocks(&parameters);
    for (int i = 0; i < problem.NumParameterBlocks(); ++i) {
        if (std::isnan(parameters[i])) return false;
    }
    return true;
}
```

### 5. Use Covariance Estimation for Uncertainty
```cpp
ceres::Covariance::Options cov_options;
cov_options.algorithm_type = ceres::CovarianceAlgorithmType::DENSE_SVD;
ceres::Covariance covariance(cov_options);

std::vector<std::pair<const double*, const double*>> pairs;
pairs.push_back(std::make_pair(&x, &x));
CHECK(covariance.Compute(pairs, &problem));

double covariance_xx;
covariance.GetCovarianceBlock(&x, &x, &covariance_xx);
double std_dev = sqrt(covariance_xx);
```
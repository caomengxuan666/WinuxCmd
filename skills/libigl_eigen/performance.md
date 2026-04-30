# Performance

**Expression templates and lazy evaluation**
```cpp
// Eigen's expression templates defer computation
Eigen::MatrixXd A(1000, 1000), B(1000, 1000), C(1000, 1000);

// BAD: Multiple evaluations
Eigen::MatrixXd D = A * B + C;  // Single fused evaluation (good!)
// But this forces evaluation immediately

// GOOD: Let Eigen optimize the expression
auto expr = A * B + C;  // Expression object, no computation yet
Eigen::MatrixXd D = expr;  // Single fused evaluation

// For repeated use, evaluate once
Eigen::MatrixXd temp = (A * B).eval();  // Force evaluation
```

**Memory layout and access patterns**
```cpp
// Column-major storage (default)
Eigen::MatrixXd m(1000, 1000);

// BAD: Row-major access in column-major matrix
double sum = 0;
for (int i = 0; i < m.rows(); ++i) {
    for (int j = 0; j < m.cols(); ++j) {
        sum += m(i, j);  // Non-contiguous access
    }
}

// GOOD: Column-major access
double sum = 0;
for (int j = 0; j < m.cols(); ++j) {
    for (int i = 0; i < m.rows(); ++i) {
        sum += m(i, j);  // Contiguous access
    }
}

// Or use built-in functions
double sum = m.sum();  // Optimized implementation
```

**Block operations and views**
```cpp
Eigen::MatrixXd big(1000, 1000);

// BAD: Copying blocks unnecessarily
Eigen::MatrixXd block = big.block(100, 100, 200, 200);  // Deep copy

// GOOD: Use block expressions (no copy)
auto block_view = big.block(100, 100, 200, 200);  // View, no copy
double val = block_view(0, 0);  // Accesses original matrix

// For repeated access, consider copying
Eigen::MatrixXd block_copy = big.block(100, 100, 200, 200);  // Copy once
```

**Compiler optimization flags**
```cpp
// Essential compiler flags for Eigen performance
// -O3: Enable all optimizations
// -march=native: Use all CPU features (AVX, AVX2, etc.)
// -DNDEBUG: Disable Eigen's runtime assertions
// -DEIGEN_NO_DEBUG: Additional debug removal

// Example CMake configuration
// target_compile_options(my_target PRIVATE
//     $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
// )
```

**Avoiding temporary objects**
```cpp
// BAD: Creating temporaries
Eigen::MatrixXd result = A * B + C * D;  // Two temporaries

// GOOD: Let Eigen fuse operations
Eigen::MatrixXd result;
result.noalias() = A * B + C * D;  // No temporary for result

// For complex expressions, use eval()
Eigen::MatrixXd temp = (A * B).eval();
temp.noalias() += C * D;
```
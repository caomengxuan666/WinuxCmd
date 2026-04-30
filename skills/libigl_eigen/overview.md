# Overview

Eigen is a high-performance C++ template library for linear algebra operations. It provides comprehensive support for matrices, vectors, numerical solvers, and related algorithms. The library is header-only, meaning no separate compilation is needed - just include the headers and use.

**When to use Eigen:**
- You need matrix/vector operations in C++
- You require high-performance linear algebra without external dependencies
- You need numerical solvers (linear systems, eigenvalues, SVD)
- You want compile-time optimization through template expressions

**When NOT to use Eigen:**
- You need GPU acceleration (consider cuBLAS, cuSolver)
- You require sparse matrix operations at massive scale (consider SuiteSparse)
- You need distributed computing across multiple machines
- You're working with extremely large matrices (>100k x 100k) where memory is critical

**Key design features:**
- Template expressions for lazy evaluation and optimization
- Compile-time matrix sizes for zero overhead
- Explicit vectorization (SSE, AVX, NEON)
- Automatic alignment for SIMD instructions
- Extensive solver suite (LU, QR, Cholesky, eigenvalues, SVD)

```cpp
// Compile-time sized matrices for maximum performance
Eigen::Matrix<double, 3, 3> fixed_size;  // No dynamic allocation
Eigen::MatrixXd dynamic_size(1000, 1000);  // Dynamic allocation

// Expression templates enable optimization
auto expr = (fixed_size * fixed_size).eval();  // Forces evaluation
```
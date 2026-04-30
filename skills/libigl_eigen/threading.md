# Threading

**Thread safety guarantees**
```cpp
// Eigen matrices are NOT thread-safe for concurrent modification
Eigen::MatrixXd shared(100, 100);

// BAD: Concurrent writes to same matrix
#pragma omp parallel for
for (int i = 0; i < shared.rows(); ++i) {
    shared.row(i) = Eigen::VectorXd::Random(shared.cols());  // Data race!
}

// GOOD: Thread-local matrices
#pragma omp parallel
{
    Eigen::MatrixXd local(100, 100);
    #pragma omp for
    for (int i = 0; i < local.rows(); ++i) {
        local.row(i) = Eigen::VectorXd::Random(local.cols());
    }
    // Merge results in critical section
    #pragma omp critical
    {
        shared += local;
    }
}
```

**Read-only concurrent access**
```cpp
// Multiple threads can read the same matrix safely
Eigen::MatrixXd data = Eigen::MatrixXd::Random(1000, 1000);

// SAFE: All threads only read
#pragma omp parallel for
for (int i = 0; i < 100; ++i) {
    double sum = data.col(i).sum();  // Read-only, safe
    // ... process sum
}
```

**Using OpenMP with Eigen**
```cpp
// Enable OpenMP in Eigen
// #define EIGEN_USE_BLAS  // Optional: use BLAS for parallel operations
// #define EIGEN_USE_LAPACK  // Optional: use LAPACK

// Eigen's internal parallelization (when enabled)
Eigen::MatrixXd A(1000, 1000), B(1000, 1000);
Eigen::MatrixXd C = A * B;  // May use multiple threads internally

// Custom parallel operations
#pragma omp parallel for
for (int i = 0; i < A.rows(); ++i) {
    A.row(i) = B.row(i) * C.row(i).transpose();  // Each row independent
}
```

**Thread-local storage for repeated operations**
```cpp
// BAD: Creating matrices in parallel region
#pragma omp parallel for
for (int i = 0; i < 1000; ++i) {
    Eigen::MatrixXd temp(100, 100);  // Each iteration allocates!
    // ... use temp
}

// GOOD: Thread-local storage
thread_local Eigen::MatrixXd temp(100, 100);  // One per thread
#pragma omp parallel for
for (int i = 0; i < 1000; ++i) {
    temp.setRandom();  // Reuse thread-local matrix
    // ... use temp
}
```

**Mutex protection for shared matrices**
```cpp
Eigen::MatrixXd shared(100, 100);
std::mutex mtx;

// Safe concurrent updates with mutex
#pragma omp parallel for
for (int i = 0; i < 100; ++i) {
    Eigen::VectorXd local = Eigen::VectorXd::Random(100);
    std::lock_guard<std::mutex> lock(mtx);
    shared.col(i) = local;  // Protected write
}
```
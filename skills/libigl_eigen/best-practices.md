# Best Practices

**Use fixed-size matrices for small, known sizes**
```cpp
// Prefer this for 3D graphics, small linear algebra
Eigen::Matrix3d rotation;  // 3x3, no dynamic allocation
Eigen::Vector3d position;  // 3x1, no dynamic allocation

// Avoid dynamic allocation for small matrices
Eigen::MatrixXd rotation_dyn(3, 3);  // Heap allocation overhead
```

**Pre-allocate and reuse matrices**
```cpp
// BAD: Repeated allocation
for (int i = 0; i < 1000; ++i) {
    Eigen::MatrixXd temp(100, 100);
    // ... use temp
}

// GOOD: Reuse allocated memory
Eigen::MatrixXd temp(100, 100);
for (int i = 0; i < 1000; ++i) {
    temp.setRandom();  // Reuses memory
    // ... use temp
}
```

**Use conservativeResize for growing matrices**
```cpp
// BAD: Reallocation loses data
Eigen::MatrixXd m(10, 10);
m.resize(20, 20);  // Data lost!

// GOOD: Preserve existing data
Eigen::MatrixXd m(10, 10);
m.conservativeResize(20, 20);  // Existing data preserved
```

**Choose appropriate solvers**
```cpp
// For small, dense systems
Eigen::MatrixXd A = /* ... */;
Eigen::VectorXd b = /* ... */;

// BAD: Using general solver for symmetric positive definite
Eigen::VectorXd x = A.fullPivLu().solve(b);  // Overkill

// GOOD: Use specialized solver
Eigen::VectorXd x = A.llt().solve(b);  // 2x faster for SPD matrices
```

**Use Map for external data**
```cpp
// Efficiently wrap external arrays
double raw_data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
Eigen::Map<Eigen::Matrix3d> mat(raw_data);  // No copy, just view
Eigen::Map<Eigen::VectorXd> vec(raw_data, 9);  // Same data, different view
```

**Enable compiler optimizations**
```cpp
// CMakeLists.txt
// target_compile_options(my_target PRIVATE -O3 -march=native -DNDEBUG)
// This enables Eigen's vectorization and optimization
```
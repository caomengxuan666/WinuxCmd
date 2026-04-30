# Safety

**Red Line 1: NEVER use uninitialized matrices in computations**
```cpp
// BAD: Undefined behavior
Eigen::MatrixXd m(100, 100);
double result = m.norm();  // Garbage in, garbage out

// GOOD: Always initialize
Eigen::MatrixXd m = Eigen::MatrixXd::Random(100, 100);
double result = m.norm();  // Safe
```

**Red Line 2: NEVER ignore aliasing warnings**
```cpp
// BAD: Silent data corruption
Eigen::MatrixXd A = Eigen::MatrixXd::Random(10, 10);
A = A * A;  // Aliasing! Results are undefined

// GOOD: Use eval() or transposeInPlace()
Eigen::MatrixXd A = Eigen::MatrixXd::Random(10, 10);
A = (A * A).eval();  // Safe
```

**Red Line 3: NEVER pass Eigen objects by value to functions**
```cpp
// BAD: Expensive copy and potential alignment issues
void process(Eigen::MatrixXd m) { /* ... */ }

// GOOD: Pass by const reference
void process(const Eigen::MatrixXd& m) { /* ... */ }
```

**Red Line 4: NEVER assume matrix operations are thread-safe**
```cpp
// BAD: Concurrent modification
Eigen::MatrixXd shared(100, 100);
#pragma omp parallel for
for (int i = 0; i < 100; ++i) {
    shared.row(i) = Eigen::VectorXd::Random(100);  // Data race!
}

// GOOD: Use thread-local copies
#pragma omp parallel
{
    Eigen::MatrixXd local(100, 100);
    #pragma omp for
    for (int i = 0; i < 100; ++i) {
        local.row(i) = Eigen::VectorXd::Random(100);
    }
    // Merge results safely
}
```

**Red Line 5: NEVER use auto with Eigen expressions in long-lived contexts**
```cpp
// BAD: Dangling expression
auto expr = A * B;  // Expression references A and B
A.resize(0, 0);     // expr now references invalid data
auto result = expr;  // Undefined behavior

// GOOD: Evaluate immediately
Eigen::MatrixXd result = A * B;  // Safe copy
```
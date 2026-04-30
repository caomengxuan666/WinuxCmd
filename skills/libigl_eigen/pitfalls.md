# Pitfalls

**Pitfall 1: Aliasing issues with matrix operations**
```cpp
// BAD: Aliasing causes incorrect results
Eigen::MatrixXd A(2, 2);
A << 1, 2, 3, 4;
A = A * A;  // Aliasing! A is being read and written simultaneously

// GOOD: Use eval() to break aliasing
Eigen::MatrixXd A(2, 2);
A << 1, 2, 3, 4;
A = (A * A).eval();  // Correct: evaluates product first
```

**Pitfall 2: Transpose aliasing**
```cpp
// BAD: a = a.transpose() causes aliasing
Eigen::MatrixXd a(2, 2);
a << 1, 2, 3, 4;
a = a.transpose();  // Wrong! Aliasing

// GOOD: Use transposeInPlace()
Eigen::MatrixXd a(2, 2);
a << 1, 2, 3, 4;
a.transposeInPlace();  // Correct
```

**Pitfall 3: Mixing fixed-size and dynamic-size matrices**
```cpp
// BAD: Incompatible sizes cause runtime errors
Eigen::Matrix3d fixed;  // 3x3 fixed
Eigen::MatrixXd dyn(4, 4);
fixed = dyn;  // Runtime assertion failure

// GOOD: Ensure size compatibility
Eigen::Matrix3d fixed;
Eigen::MatrixXd dyn(3, 3);
fixed = dyn;  // OK: sizes match
```

**Pitfall 4: Forgetting to initialize matrices**
```cpp
// BAD: Uninitialized matrix contains garbage
Eigen::MatrixXd m(100, 100);
double sum = m.sum();  // Undefined behavior

// GOOD: Always initialize
Eigen::MatrixXd m = Eigen::MatrixXd::Zero(100, 100);
double sum = m.sum();  // Safe: returns 0
```

**Pitfall 5: Incorrect use of auto with Eigen expressions**
```cpp
// BAD: auto captures expression, not result
auto result = A * B;  // This is an expression, not a matrix
// result might be invalid after A or B changes

// GOOD: Use explicit type or eval()
Eigen::MatrixXd result = A * B;  // Forces evaluation
auto result = (A * B).eval();     // Explicit evaluation
```

**Pitfall 6: Stack overflow with large fixed-size matrices**
```cpp
// BAD: Large fixed-size matrix on stack
Eigen::Matrix<double, 1000, 1000> huge;  // 8MB on stack - likely crash

// GOOD: Use dynamic allocation for large matrices
Eigen::MatrixXd huge(1000, 1000);  // On heap
```

**Pitfall 7: Ignoring alignment requirements**
```cpp
// BAD: Unaligned allocation in containers
std::vector<Eigen::Matrix4d> vec(10);  // May crash on some platforms

// GOOD: Use aligned allocator
std::vector<Eigen::Matrix4d, Eigen::aligned_allocator<Eigen::Matrix4d>> vec(10);
```

**Pitfall 8: Modifying matrices through references**
```cpp
// BAD: Modifying through const reference
const Eigen::MatrixXd& ref = matrix;
ref(0, 0) = 42;  // Compiles but violates const correctness

// GOOD: Use const correctly
const Eigen::MatrixXd& ref = matrix;
// ref(0, 0) = 42;  // Compiler error - correct
```
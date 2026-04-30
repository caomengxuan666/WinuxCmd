# Lifecycle

**Construction and initialization**
```cpp
// Default construction (uninitialized)
Eigen::MatrixXd m;  // Empty, size 0x0
Eigen::Matrix3d f;  // Fixed 3x3, uninitialized

// Sized construction
Eigen::MatrixXd m(3, 4);  // 3x4, uninitialized
Eigen::VectorXd v(10);    // 10 elements, uninitialized

// Initialized construction
Eigen::MatrixXd zeros = Eigen::MatrixXd::Zero(3, 3);
Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(3, 3);
Eigen::MatrixXd identity = Eigen::MatrixXd::Identity(3, 3);
Eigen::MatrixXd random = Eigen::MatrixXd::Random(3, 3);
```

**Copy and move semantics**
```cpp
// Copy construction (deep copy)
Eigen::MatrixXd a = Eigen::MatrixXd::Random(100, 100);
Eigen::MatrixXd b = a;  // Deep copy of all elements

// Move construction (C++11)
Eigen::MatrixXd c = std::move(a);  // a is now empty (0x0)

// Copy assignment
Eigen::MatrixXd d(100, 100);
d = b;  // Deep copy

// Move assignment
Eigen::MatrixXd e(100, 100);
e = std::move(c);  // c is now empty
```

**Resource management**
```cpp
// Dynamic matrices manage their own memory
{
    Eigen::MatrixXd m(1000, 1000);  // Allocates ~8MB on heap
    // ... use m
}  // Automatically freed when m goes out of scope

// Fixed-size matrices use stack memory
{
    Eigen::Matrix4d m;  // 128 bytes on stack
    // ... use m
}  // No heap allocation needed
```

**Resizing and reshaping**
```cpp
Eigen::MatrixXd m(10, 10);

// Resize (loses data)
m.resize(5, 20);  // New size, old data lost

// Conservative resize (preserves data)
m.conservativeResize(12, 12);  // Preserves top-left 10x10

// Reshape (same data, different shape)
Eigen::MatrixXd reshaped = Eigen::Map<Eigen::MatrixXd>(m.data(), 4, 25);
```

**Swap and reset**
```cpp
Eigen::MatrixXd a(100, 100);
Eigen::MatrixXd b(200, 200);

// Efficient swap (just swaps pointers)
a.swap(b);  // Now a is 200x200, b is 100x100

// Reset to zero
a.setZero();  // All elements become 0, size unchanged

// Reset to identity
a.setIdentity();  // Diagonal becomes 1, rest 0
```
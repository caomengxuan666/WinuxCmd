# Quickstart

```cpp
#include <iostream>
#include <Eigen/Dense>

int main() {
    // 1. Basic matrix creation and initialization
    Eigen::MatrixXd m(2, 2);
    m << 1, 2,
         3, 4;
    std::cout << "Matrix m:\n" << m << std::endl;

    // 2. Vector operations
    Eigen::VectorXd v(3);
    v << 1, 2, 3;
    Eigen::VectorXd w = v * 2.0;
    std::cout << "Vector v * 2:\n" << w << std::endl;

    // 3. Matrix multiplication
    Eigen::MatrixXd a(2, 3);
    a << 1, 2, 3,
         4, 5, 6;
    Eigen::MatrixXd b(3, 2);
    b << 7, 8,
         9, 10,
         11, 12;
    Eigen::MatrixXd c = a * b;
    std::cout << "Matrix multiplication:\n" << c << std::endl;

    // 4. Solving linear systems
    Eigen::MatrixXd A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 10;
    Eigen::VectorXd b_vec(3);
    b_vec << 1, 2, 3;
    Eigen::VectorXd x = A.colPivHouseholderQr().solve(b_vec);
    std::cout << "Solution to Ax = b:\n" << x << std::endl;

    // 5. Eigenvalues and eigenvectors
    Eigen::MatrixXd sym(2, 2);
    sym << 1, 2,
           2, 1;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(sym);
    if (eigensolver.info() == Eigen::Success) {
        std::cout << "Eigenvalues:\n" << eigensolver.eigenvalues() << std::endl;
        std::cout << "Eigenvectors:\n" << eigensolver.eigenvectors() << std::endl;
    }

    // 6. Element-wise operations
    Eigen::MatrixXd m1(2, 2);
    m1 << 1, 2,
          3, 4;
    Eigen::MatrixXd m2 = m1.array() * m1.array();  // element-wise multiplication
    std::cout << "Element-wise square:\n" << m2 << std::endl;

    // 7. Resizing and reshaping
    Eigen::MatrixXd dyn(3, 4);
    dyn.setRandom();
    dyn.resize(2, 6);  // total elements must match
    std::cout << "Resized matrix:\n" << dyn << std::endl;

    // 8. Block operations
    Eigen::MatrixXd big(4, 4);
    big.setRandom();
    Eigen::MatrixXd block = big.block(1, 1, 2, 2);  // 2x2 block starting at (1,1)
    std::cout << "Block from big matrix:\n" << block << std::endl;

    return 0;
}
```
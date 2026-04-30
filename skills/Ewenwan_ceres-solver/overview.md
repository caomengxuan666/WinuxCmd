# Overview

Ceres Solver is a C++ library for modeling and solving large, complicated optimization problems. It is designed for nonlinear least squares problems with bounds constraints and general unconstrained optimization. The library uses automatic differentiation to compute derivatives, making it accessible for users who don't want to manually derive Jacobians.

Use Ceres when you need to solve:
- Bundle adjustment in computer vision
- Sensor fusion and calibration
- Robot pose estimation (SLAM)
- Curve fitting and data interpolation
- Any problem expressible as minimizing sum of squared residuals

Do NOT use Ceres for:
- Linear programming (use dedicated LP solvers)
- Convex optimization with simple structure (use CVXOPT)
- Problems requiring integer variables
- Real-time control loops (solver overhead may be too high)

Key design features:
- Automatic differentiation via templated functors
- Sparse and dense linear solvers
- Trust region and line search strategies
- Robust loss functions for outlier handling
- Local parameterizations for constrained manifolds (e.g., quaternions)
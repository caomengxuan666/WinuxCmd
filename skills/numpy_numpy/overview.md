# Overview

```cpp
// Overview: numpy/numpy provides C-level array manipulation for Python
// Use when: You need high-performance numerical computation in C++ with Python integration
// Don't use when: Pure C++ computation without Python interop is sufficient

/*
Key Design:
- NumPy arrays are Python objects with C-level access
- Data is stored in contiguous memory buffers
- Supports multiple data types (NPY_DOUBLE, NPY_INT32, etc.)
- Memory management via reference counting
- Strided access for efficient slicing
*/

// Example: When to use numpy
#include <numpy/arrayobject.h>
void process_large_dataset(PyObject* py_array) {
    // Use numpy for large numerical datasets that need Python interop
    if (!PyArray_Check(py_array)) return;
    PyArrayObject* arr = reinterpret_cast<PyArrayObject*>(py_array);
    double* data = static_cast<double*>(PyArray_DATA(arr));
    // Process data...
}

// Example: When NOT to use numpy (pure C++ is better)
#include <vector>
void pure_cpp_processing(const std::vector<double>& data) {
    // No Python dependency needed
    for (auto val : data) { /* process */ }
}
```
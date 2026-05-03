# Quickstart

```cpp
// Quickstart: Common numpy/numpy usage patterns
#include <numpy/arrayobject.h>
#include <iostream>
#include <vector>

// Pattern 1: Create a 1D array from C++ vector
void create_from_vector() {
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
    npy_intp dims[] = {static_cast<npy_intp>(data.size())};
    PyObject* arr = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, data.data());
    if (!arr) { std::cerr << "Failed to create array\n"; return; }
    Py_DECREF(arr);
}

// Pattern 2: Create a 2D array with zeros
void create_zeros_2d() {
    npy_intp dims[] = {3, 4};
    PyObject* arr = PyArray_ZEROS(2, dims, NPY_DOUBLE, 0);
    if (!arr) { std::cerr << "Failed to create zeros\n"; return; }
    Py_DECREF(arr);
}

// Pattern 3: Access array elements
void access_elements(PyObject* arr) {
    if (!PyArray_Check(arr)) return;
    npy_intp idx[] = {0, 1};
    double val = *static_cast<double*>(PyArray_GetPtr(arr, idx));
    std::cout << "Value at [0,1]: " << val << "\n";
}

// Pattern 4: Reshape an array
PyObject* reshape_array(PyObject* arr, npy_intp new_rows, npy_intp new_cols) {
    npy_intp new_dims[] = {new_rows, new_cols};
    return PyArray_Newshape(arr, new_dims, NPY_CORDER);
}

// Pattern 5: Perform element-wise operations
void elementwise_add(PyObject* a, PyObject* b) {
    PyObject* result = PyArray_GenericBinaryFunction(a, b, npy_ops.add);
    if (!result) { PyErr_Print(); return; }
    Py_DECREF(result);
}

// Pattern 6: Slice an array
PyObject* slice_array(PyObject* arr, npy_intp start, npy_intp stop) {
    PyObject* slice = PySlice_New(PyLong_FromSsize_t(start), 
                                  PyLong_FromSsize_t(stop), nullptr);
    PyObject* result = PyObject_GetItem(arr, slice);
    Py_DECREF(slice);
    return result;
}

// Pattern 7: Convert to Python list
PyObject* to_list(PyObject* arr) {
    return PyArray_ToList(arr);
}

// Pattern 8: Check array properties
void check_properties(PyObject* arr) {
    if (!PyArray_Check(arr)) return;
    std::cout << "Dimensions: " << PyArray_NDIM(arr) << "\n";
    std::cout << "Shape: ";
    for (int i = 0; i < PyArray_NDIM(arr); ++i)
        std::cout << PyArray_DIMS(arr)[i] << " ";
    std::cout << "\nTotal size: " << PyArray_SIZE(arr) << "\n";
}
```
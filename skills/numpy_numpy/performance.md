# Performance

```cpp
// Performance: Characteristics, allocation patterns, optimization tips

#include <numpy/arrayobject.h>
#include <chrono>
#include <iostream>

// PERFORMANCE CHARACTERISTICS
/*
- NumPy arrays are stored in contiguous memory (C-order by default)
- Access patterns significantly affect performance
- Creating arrays has overhead (Python object creation)
- Views are cheap (no data copy)
- Copies are expensive (O(n) memory and time)
*/

// ALLOCATION PATTERNS

// BAD: Repeated allocation in loop
void bad_allocation() {
    for (int i = 0; i < 1000; ++i) {
        npy_intp dims[] = {1000};
        PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
        // Use arr...
        Py_DECREF(arr); // Expensive: allocate and free 1000 times
    }
}

// GOOD: Pre-allocate and reuse
void good_allocation() {
    npy_intp dims[] = {1000};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    double* data = static_cast<double*>(PyArray_DATA(arr));
    
    for (int i = 0; i < 1000; ++i) {
        // Reuse same array, just modify data
        for (npy_intp j = 0; j < 1000; ++j) {
            data[j] = i * j;
        }
    }
    Py_DECREF(arr);
}

// OPTIMIZATION TIPS

// Tip 1: Use contiguous arrays for faster access
void ensure_contiguous(PyObject* arr) {
    // Non-contiguous arrays have slower access
    if (!PyArray_ISCONTIGUOUS(arr)) {
        PyObject* contig = PyArray_NewCopy(arr, NPY_CORDER);
        // Use contig for faster access
        Py_DECREF(contig);
    }
}

// Tip 2: Use views instead of copies
void use_views_instead_of_copies(PyObject* arr) {
    // BAD: Creating copy for slicing
    PyObject* bad_slice = PyArray_Copy(arr); // Expensive!
    
    // GOOD: Create view
    PyObject* good_slice = PyArray_Newview(arr, nullptr); // Cheap!
    Py_DECREF(good_slice);
    Py_DECREF(bad_slice);
}

// Tip 3: Batch operations
void batch_operations(PyObject* arr) {
    // BAD: Element-by-element
    double* data = static_cast<double*>(PyArray_DATA(arr));
    npy_intp size = PyArray_SIZE(arr);
    for (npy_intp i = 0; i < size; ++i) {
        data[i] = data[i] * 2.0 + 1.0; // Multiple operations per element
    }
    
    // GOOD: Use NumPy's vectorized operations
    PyObject* result = PyArray_GenericBinaryFunction(arr, 
        PyFloat_FromDouble(2.0), npy_ops.multiply);
    // Then add 1...
    Py_DECREF(result);
}

// Tip 4: Memory layout matters
void memory_layout() {
    npy_intp dims[] = {1000, 1000};
    
    // C-order (row-major) - fast for row access
    PyObject* c_order = PyArray_SimpleNew(2, dims, NPY_DOUBLE);
    
    // F-order (column-major) - fast for column access
    PyObject* f_order = PyArray_New(2, dims, NPY_DOUBLE, nullptr, nullptr, 
                                     nullptr, 0, NPY_ARRAY_F_CONTIGUOUS, nullptr);
    
    Py_DECREF(c_order);
    Py_DECREF(f_order);
}

// PERFORMANCE MEASUREMENT
void measure_performance() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Operation to measure
    npy_intp dims[] = {1000000};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    double* data = static_cast<double*>(PyArray_DATA(arr));
    for (npy_intp i = 0; i < 1000000; ++i) data[i] = i * 2.0;
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Operation took " << duration.count() << " microseconds\n";
    
    Py_DECREF(arr);
}
```
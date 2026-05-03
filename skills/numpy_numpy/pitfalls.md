# Pitfalls

```cpp
// Pitfalls: Common mistakes with numpy/numpy

// PITFALL 1: Forgetting to initialize NumPy
// BAD: Missing initialization
void bad_init() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE); // CRASH!
}

// GOOD: Proper initialization
void good_init() {
    import_array(); // Must call this first
    npy_intp dims[] = {5};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    Py_DECREF(arr);
}

// PITFALL 2: Memory leaks with borrowed references
// BAD: Not decrementing references
void bad_refcount(PyObject* arr) {
    PyObject* shape = PyArray_Shape(arr); // New reference
    // Missing Py_DECREF(shape) - memory leak!
}

// GOOD: Proper reference management
void good_refcount(PyObject* arr) {
    PyObject* shape = PyArray_Shape(arr);
    // Use shape...
    Py_DECREF(shape); // Clean up
}

// PITFALL 3: Using invalidated data pointers
// BAD: Using pointer after array modification
void bad_pointer(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    PyArray_Resize(arr, nullptr, 0, NPY_ANYORDER); // May reallocate!
    data[0] = 1.0; // Undefined behavior!
}

// GOOD: Re-acquire pointer after modification
void good_pointer(PyObject* arr) {
    PyArray_Resize(arr, nullptr, 0, NPY_ANYORDER);
    double* data = static_cast<double*>(PyArray_DATA(arr)); // Fresh pointer
    data[0] = 1.0;
}

// PITFALL 4: Incorrect dimension handling
// BAD: Assuming 1D array
void bad_dims(PyObject* arr) {
    npy_intp size = PyArray_DIMS(arr)[0]; // May not exist for 0D arrays!
}

// GOOD: Check dimensions first
void good_dims(PyObject* arr) {
    if (PyArray_NDIM(arr) < 1) return;
    npy_intp size = PyArray_DIMS(arr)[0];
}

// PITFALL 5: Type mismatch
// BAD: Assuming double type
void bad_type(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    // If array is NPY_INT32, this corrupts memory!
}

// GOOD: Check and convert type
void good_type(PyObject* arr) {
    if (PyArray_TYPE(arr) != NPY_DOUBLE) {
        arr = PyArray_Cast(arr, NPY_DOUBLE);
    }
    double* data = static_cast<double*>(PyArray_DATA(arr));
    Py_DECREF(arr);
}

// PITFALL 6: Not handling NULL returns
// BAD: No NULL check
void bad_null() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE);
    double* data = static_cast<double*>(PyArray_DATA(arr)); // Crash if NULL!
}

// GOOD: Always check for NULL
void good_null() {
    npy_intp dims[] = {5};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    if (!arr) { PyErr_Print(); return; }
    double* data = static_cast<double*>(PyArray_DATA(arr));
    Py_DECREF(arr);
}

// PITFALL 7: Incorrect stride usage
// BAD: Assuming contiguous strides
void bad_strides(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    for (npy_intp i = 0; i < PyArray_SIZE(arr); ++i) {
        data[i] = 0.0; // Wrong for non-contiguous arrays!
    }
}

// GOOD: Use proper iteration
void good_strides(PyObject* arr) {
    PyArrayIterObject* iter = PyArray_IterNew(arr);
    while (iter->index < iter->size) {
        double* ptr = static_cast<double*>(iter->dataptr);
        *ptr = 0.0;
        PyArray_ITER_NEXT(iter);
    }
    Py_DECREF(iter);
}

// PITFALL 8: Mixing Python and C++ exceptions
// BAD: Letting C++ exceptions escape through Python
void bad_exceptions(PyObject* arr) {
    throw std::runtime_error("Error"); // Will crash Python!
}

// GOOD: Handle errors properly
void good_exceptions(PyObject* arr) {
    try {
        // NumPy operations...
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    }
}
```
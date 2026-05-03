# Safety

```cpp
// Safety: 5 red-line conditions that must NEVER occur

// CONDITION 1: NEVER access array data without checking type
// BAD: Direct cast without type check
void unsafe_type_access(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr)); // CRASH if not double!
}

// GOOD: Always verify type
void safe_type_access(PyObject* arr) {
    if (PyArray_TYPE(arr) != NPY_DOUBLE) {
        PyErr_SetString(PyExc_TypeError, "Expected double array");
        return;
    }
    double* data = static_cast<double*>(PyArray_DATA(arr));
}

// CONDITION 2: NEVER use array data after resize/reallocation
// BAD: Stale pointer after resize
void unsafe_resize(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    PyArray_Resize(arr, nullptr, 100, NPY_ANYORDER);
    data[0] = 1.0; // UNDEFINED BEHAVIOR - data may be invalid!
}

// GOOD: Re-acquire pointer
void safe_resize(PyObject* arr) {
    PyArray_Resize(arr, nullptr, 100, NPY_ANYORDER);
    double* data = static_cast<double*>(PyArray_DATA(arr)); // Fresh pointer
    data[0] = 1.0;
}

// CONDITION 3: NEVER ignore NULL returns from NumPy functions
// BAD: No NULL check
void unsafe_null() {
    npy_intp dims[] = {1000000000}; // Huge size
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE); // May return NULL!
    double* data = static_cast<double*>(PyArray_DATA(arr)); // Crash if NULL!
}

// GOOD: Always check
void safe_null() {
    npy_intp dims[] = {1000000000};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    if (!arr) {
        PyErr_Print();
        return;
    }
    double* data = static_cast<double*>(PyArray_DATA(arr));
    Py_DECREF(arr);
}

// CONDITION 4: NEVER forget to call import_array()
// BAD: Missing initialization
void unsafe_init() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE); // CRASH!
}

// GOOD: Proper initialization
void safe_init() {
    import_array(); // REQUIRED before any NumPy operations
    npy_intp dims[] = {5};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    Py_DECREF(arr);
}

// CONDITION 5: NEVER let C++ exceptions propagate through Python callbacks
// BAD: Uncaught exception in Python callback
void unsafe_callback() {
    // Called from Python
    throw std::runtime_error("Error"); // Will crash interpreter!
}

// GOOD: Catch all exceptions
void safe_callback() {
    try {
        // NumPy operations
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
    }
}
```
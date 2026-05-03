# Best Practices

```cpp
// Best Practices: Production-ready patterns for numpy/numpy

// PRACTICE 1: RAII wrapper for NumPy arrays
#include <numpy/arrayobject.h>
#include <memory>

class NumPyArray {
    PyObject* arr_;
public:
    NumPyArray() : arr_(nullptr) {}
    explicit NumPyArray(PyObject* arr) : arr_(arr) {
        if (!PyArray_Check(arr)) throw std::invalid_argument("Not a NumPy array");
    }
    ~NumPyArray() { if (arr_) Py_DECREF(arr_); }
    
    NumPyArray(const NumPyArray&) = delete;
    NumPyArray& operator=(const NumPyArray&) = delete;
    
    NumPyArray(NumPyArray&& other) noexcept : arr_(other.arr_) {
        other.arr_ = nullptr;
    }
    
    PyObject* get() const { return arr_; }
    double* data() const {
        return static_cast<double*>(PyArray_DATA(arr_));
    }
};

// PRACTICE 2: Type-safe array creation
template<typename T>
PyObject* create_array(const std::vector<T>& data) {
    import_array();
    npy_intp dims[] = {static_cast<npy_intp>(data.size())};
    int type = std::is_same<T, double>::value ? NPY_DOUBLE :
               std::is_same<T, int>::value ? NPY_INT32 : NPY_FLOAT;
    
    PyObject* arr = PyArray_SimpleNew(1, dims, type);
    if (!arr) return nullptr;
    
    std::copy(data.begin(), data.end(), 
              static_cast<T*>(PyArray_DATA(arr)));
    return arr;
}

// PRACTICE 3: Safe iteration pattern
void safe_iterate(PyObject* arr) {
    if (!PyArray_Check(arr)) return;
    
    // Ensure contiguous for performance
    PyObject* contig = PyArray_FromAny(arr, nullptr, 0, 0, 
                                        NPY_ARRAY_CARRAY, nullptr);
    if (!contig) { PyErr_Print(); return; }
    
    double* data = static_cast<double*>(PyArray_DATA(contig));
    npy_intp size = PyArray_SIZE(contig);
    
    for (npy_intp i = 0; i < size; ++i) {
        data[i] *= 2.0; // Safe because contiguous
    }
    
    Py_DECREF(contig);
}

// PRACTICE 4: Error handling pattern
PyObject* safe_operation(PyObject* arr) {
    if (!PyArray_Check(arr)) {
        PyErr_SetString(PyExc_TypeError, "Expected NumPy array");
        return nullptr;
    }
    
    PyObject* result = PyArray_SimpleNewLikeArray(arr, nullptr, 0, 0);
    if (!result) return nullptr;
    
    // Perform operation...
    
    return result; // Caller owns reference
}

// PRACTICE 5: Memory-efficient views
void use_views(PyObject* arr) {
    // Create view instead of copy when possible
    PyObject* view = PyArray_Newview(arr, nullptr);
    // Modify view...
    Py_DECREF(view); // Original array unchanged
}
```
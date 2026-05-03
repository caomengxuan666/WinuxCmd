# Lifecycle

```cpp
// Lifecycle: Construction, destruction, move semantics, resource management

#include <numpy/arrayobject.h>
#include <cassert>

// CONSTRUCTION: Different ways to create arrays
void construction_examples() {
    import_array();
    
    // From scratch
    npy_intp dims[] = {3, 4};
    PyObject* arr1 = PyArray_SimpleNew(2, dims, NPY_DOUBLE);
    
    // From existing data (no copy)
    double data[] = {1.0, 2.0, 3.0};
    npy_intp dims1[] = {3};
    PyObject* arr2 = PyArray_SimpleNewFromData(1, dims1, NPY_DOUBLE, data);
    
    // Copy of existing array
    PyObject* arr3 = PyArray_Copy(arr1);
    
    // Zeros
    PyObject* arr4 = PyArray_ZEROS(2, dims, NPY_DOUBLE, 0);
    
    Py_DECREF(arr1);
    Py_DECREF(arr2);
    Py_DECREF(arr3);
    Py_DECREF(arr4);
}

// DESTRUCTION: Proper cleanup
void destruction_example() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE);
    // Use array...
    Py_DECREF(arr); // Decrement reference count
    // arr is now invalid - don't use it!
}

// MOVE SEMANTICS: Transfer ownership
class ArrayOwner {
    PyObject* arr_;
public:
    ArrayOwner() : arr_(nullptr) {}
    
    // Move constructor
    ArrayOwner(ArrayOwner&& other) noexcept : arr_(other.arr_) {
        other.arr_ = nullptr;
    }
    
    // Move assignment
    ArrayOwner& operator=(ArrayOwner&& other) noexcept {
        if (this != &other) {
            if (arr_) Py_DECREF(arr_);
            arr_ = other.arr_;
            other.arr_ = nullptr;
        }
        return *this;
    }
    
    ~ArrayOwner() {
        if (arr_) Py_DECREF(arr_);
    }
    
    void reset(PyObject* arr = nullptr) {
        if (arr_) Py_DECREF(arr_);
        arr_ = arr;
    }
    
    PyObject* release() {
        PyObject* tmp = arr_;
        arr_ = nullptr;
        return tmp;
    }
};

// RESOURCE MANAGEMENT: Reference counting
void reference_counting() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE);
    
    // New reference - need to decrement
    PyObject* shape = PyArray_Shape(arr);
    Py_DECREF(shape);
    
    // Borrowed reference - don't decrement
    PyArrayObject* arr_obj = reinterpret_cast<PyArrayObject*>(arr);
    // arr_obj is borrowed
    
    // Increment reference when storing
    Py_INCREF(arr);
    // ... use arr ...
    Py_DECREF(arr);
    
    Py_DECREF(arr);
}

// LIFECYCLE: Complete example
void complete_lifecycle() {
    import_array();
    
    // 1. Create
    npy_intp dims[] = {100};
    PyObject* arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    assert(arr != nullptr);
    
    // 2. Use
    double* data = static_cast<double*>(PyArray_DATA(arr));
    for (npy_intp i = 0; i < 100; ++i) data[i] = i;
    
    // 3. Transfer ownership
    ArrayOwner owner(arr); // Takes ownership
    
    // 4. Move
    ArrayOwner other = std::move(owner); // Transfer ownership
    
    // 5. Release back to Python
    PyObject* released = other.release();
    
    // 6. Final cleanup
    Py_DECREF(released);
}
```
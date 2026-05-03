# Threading

```cpp
// Threading: Thread safety guarantees and concurrent access patterns

#include <numpy/arrayobject.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

// THREAD SAFETY GUARANTEES
/*
- NumPy arrays are NOT thread-safe by default
- Multiple threads reading is safe
- Any writing requires synchronization
- The GIL (Global Interpreter Lock) protects Python objects
- C-level data access bypasses GIL
*/

// BAD: Unsafe concurrent write
void unsafe_concurrent_write(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    npy_intp size = PyArray_SIZE(arr);
    
    std::thread t1([&]() {
        for (npy_intp i = 0; i < size/2; ++i) data[i] = 1.0; // Race condition!
    });
    
    std::thread t2([&]() {
        for (npy_intp i = size/2; i < size; ++i) data[i] = 2.0; // Race condition!
    });
    
    t1.join();
    t2.join();
}

// GOOD: Use mutex for synchronization
std::mutex array_mutex;

void safe_concurrent_write(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    npy_intp size = PyArray_SIZE(arr);
    
    std::thread t1([&]() {
        std::lock_guard<std::mutex> lock(array_mutex);
        for (npy_intp i = 0; i < size/2; ++i) data[i] = 1.0;
    });
    
    std::thread t2([&]() {
        std::lock_guard<std::mutex> lock(array_mutex);
        for (npy_intp i = size/2; i < size; ++i) data[i] = 2.0;
    });
    
    t1.join();
    t2.join();
}

// GOOD: Thread-local arrays for independent work
void thread_local_arrays() {
    auto worker = [](int thread_id) {
        // Each thread gets its own array
        npy_intp dims[] = {1000};
        PyObject* local_arr = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
        double* data = static_cast<double*>(PyArray_DATA(lowcal_arr));
        
        for (npy_intp i = 0; i < 1000; ++i) {
            data[i] = thread_id * i;
        }
        
        Py_DECREF(local_arr);
    };
    
    std::thread t1(worker, 0);
    std::thread t2(worker, 1);
    
    t1.join();
    t2.join();
}

// GOOD: Read-only concurrent access (safe)
void safe_read_only(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    npy_intp size = PyArray_SIZE(arr);
    std::atomic<double> sum{0.0};
    
    std::thread t1([&]() {
        double local_sum = 0.0;
        for (npy_intp i = 0; i < size/2; ++i) local_sum += data[i];
        sum.fetch_add(local_sum);
    });
    
    std::thread t2([&]() {
        double local_sum = 0.0;
        for (npy_intp i = size/2; i < size; ++i) local_sum += data[i];
        sum.fetch_add(local_sum);
    });
    
    t1.join();
    t2.join();
}

// GOOD: Partitioned writes with atomic flags
void partitioned_writes(PyObject* arr) {
    double* data = static_cast<double*>(PyArray_DATA(arr));
    npy_intp size = PyArray_SIZE(arr);
    std::atomic<bool> done1{false}, done2{false};
    
    std::thread t1([&]() {
        for (npy_intp i = 0; i < size/2; ++i) data[i] = 1.0;
        done1 = true;
    });
    
    std::thread t2([&]() {
        for (npy_intp i = size/2; i < size; ++i) data[i] = 2.0;
        done2 = true;
    });
    
    t1.join();
    t2.join();
}

// BAD: Mixing Python API calls across threads
void bad_python_api_cross_thread() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE);
    
    std::thread t([arr]() {
        // BAD: Python API calls from non-main thread without GIL
        PyObject* shape = PyArray_Shape(arr); // Unsafe!
        Py_DECREF(shape);
    });
    
    t.join();
    Py_DECREF(arr);
}

// GOOD: Use GIL properly
void good_gil_usage() {
    PyObject* arr = PyArray_SimpleNew(1, nullptr, NPY_DOUBLE);
    
    std::thread t([arr]() {
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure(); // Acquire GIL
        
        // Now safe to call Python API
        PyObject* shape = PyArray_Shape(arr);
        Py_DECREF(shape);
        
        PyGILState_Release(gstate); // Release GIL
    });
    
    t.join();
    Py_DECREF(arr);
}
```
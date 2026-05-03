# Lifecycle

### Construction
```cpp
use rayon::ThreadPoolBuilder;

// Create a custom thread pool
let pool = ThreadPoolBuilder::new()
    .num_threads(4)
    .thread_name(|i| format!("worker-{}", i))
    .build()
    .unwrap();

// Use the pool
pool.install(|| {
    // Parallel work here
});
```

### Destruction
```cpp
use rayon::ThreadPoolBuilder;

{
    let pool = ThreadPoolBuilder::new()
        .num_threads(2)
        .build()
        .unwrap();
    
    pool.install(|| {
        // Parallel work
    });
} // Pool is dropped here, threads are joined
```

### Resource Management
```cpp
use rayon::ThreadPoolBuilder;
use std::sync::OnceLock;

// Global pool with lazy initialization
static GLOBAL_POOL: OnceLock<rayon::ThreadPool> = OnceLock::new();

fn get_pool() -> &'rayon::ThreadPool {
    GLOBAL_POOL.get_or_init(|| {
        ThreadPoolBuilder::new()
            .num_threads(num_cpus::get())
            .build()
            .unwrap()
    })
}

fn main() {
    let pool = get_pool();
    pool.install(|| {
        // Parallel work
    });
}
```

### Move Semantics
```cpp
use rayon::prelude::*;

fn process_owned_data(data: Vec<i32>) -> Vec<i32> {
    // Data is moved into the closure
    data.into_par_iter()
        .map(|x| x * 2)
        .collect()
}

fn process_borrowed_data(data: &[i32]) -> Vec<i32> {
    // Data is borrowed
    data.par_iter()
        .map(|x| *x * 2)
        .collect()
}
```
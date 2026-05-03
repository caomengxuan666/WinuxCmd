# Threading

### Thread Safety Guarantees
```cpp
use rayon::prelude::*;
use std::sync::Mutex;

// Rayon ensures thread safety through Send + Sync bounds
fn safe_parallel_access(data: &[i32]) -> Vec<i32> {
    let results = Mutex::new(Vec::new());
    data.par_iter().for_each(|x| {
        let mut guard = results.lock().unwrap();
        guard.push(*x * 2);
    });
    results.into_inner().unwrap()
}
```

### Work-Stealing Thread Pool
```cpp
use rayon::ThreadPoolBuilder;

// Thread pool with work-stealing
let pool = ThreadPoolBuilder::new()
    .num_threads(4)
    .build()
    .unwrap();

pool.install(|| {
    // Work is distributed across threads
    // Idle threads steal work from busy threads
    let data: Vec<i32> = (0..1000).collect();
    let sum: i32 = data.par_iter().sum();
    println!("Sum: {}", sum);
});
```

### Concurrent Access Patterns
```cpp
use rayon::prelude::*;
use std::sync::atomic::{AtomicI32, Ordering};

// Safe concurrent accumulation with atomics
fn parallel_count(data: &[i32]) -> i32 {
    let count = AtomicI32::new(0);
    data.par_iter().for_each(|x| {
        if *x > 0 {
            count.fetch_add(1, Ordering::SeqCst);
        }
    });
    count.load(Ordering::SeqCst)
}

// Safe concurrent accumulation with Mutex
use std::sync::Mutex;

fn parallel_sum_with_mutex(data: &[i32]) -> i32 {
    let sum = Mutex::new(0);
    data.par_iter().for_each(|x| {
        let mut guard = sum.lock().unwrap();
        *guard += x;
    });
    *sum.lock().unwrap()
}
```

### Thread Pool Reuse
```cpp
use rayon::ThreadPoolBuilder;
use std::sync::OnceLock;

// Reuse thread pool across multiple operations
static POOL: OnceLock<rayon::ThreadPool> = OnceLock::new();

fn get_pool() -> &'rayon::ThreadPool {
    POOL.get_or_init(|| {
        ThreadPoolBuilder::new()
            .num_threads(4)
            .build()
            .unwrap()
    })
}

fn main() {
    let pool = get_pool();
    
    // Multiple parallel operations reuse the same pool
    pool.install(|| {
        let data1: Vec<i32> = (0..1000).collect();
        let sum1: i32 = data1.par_iter().sum();
        println!("Sum 1: {}", sum1);
    });
    
    pool.install(|| {
        let data2: Vec<i32> = (0..2000).collect();
        let sum2: i32 = data2.par_iter().sum();
        println!("Sum 2: {}", sum2);
    });
}
```
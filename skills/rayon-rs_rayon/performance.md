# Performance

### Thread Pool Configuration
```cpp
use rayon::ThreadPoolBuilder;

// Match thread count to CPU cores
let pool = ThreadPoolBuilder::new()
    .num_threads(num_cpus::get()) // Use all cores
    .build()
    .unwrap();

// For I/O-bound work, use more threads
let pool = ThreadPoolBuilder::new()
    .num_threads(num_cpus::get() * 2) // More threads for I/O
    .build()
    .unwrap();
```

### Minimize Parallel Overhead
```cpp
use rayon::prelude::*;

// BAD: Parallel overhead for small work
fn small_work(data: &[i32]) -> i32 {
    data.par_iter().sum() // Overhead > benefit for small arrays
}

// GOOD: Use sequential for small, parallel for large
fn adaptive_sum(data: &[i32]) -> i32 {
    if data.len() > 10000 {
        data.par_iter().sum()
    } else {
        data.iter().sum()
    }
}
```

### Use par_sort_unstable for Performance
```cpp
use rayon::prelude::*;

// BAD: Using stable sort when not needed
fn sort_data(mut data: Vec<i32>) -> Vec<i32> {
    data.par_sort(); // Stable sort is slower
    data
}

// GOOD: Use unstable sort for performance
fn sort_data_fast(mut data: Vec<i32>) -> Vec<i32> {
    data.par_sort_unstable(); // Faster, but doesn't preserve order of equal elements
    data
}
```

### Avoid Collecting When Not Needed
```cpp
use rayon::prelude::*;

// BAD: Collecting intermediate results
fn process_and_sum(data: &[i32]) -> i32 {
    let intermediate: Vec<i32> = data.par_iter()
        .map(|x| x * 2)
        .collect(); // Unnecessary allocation
    intermediate.par_iter().sum()
}

// GOOD: Chain operations without intermediate collection
fn process_and_sum_fast(data: &[i32]) -> i32 {
    data.par_iter()
        .map(|x| x * 2)
        .sum() // No intermediate allocation
}
```

### Use par_bridge for Non-Parallel Iterators
```cpp
use rayon::prelude::*;
use std::collections::HashSet;

// BAD: Converting to Vec first
fn process_set_slow(data: HashSet<i32>) -> Vec<i32> {
    let vec: Vec<i32> = data.into_iter().collect(); // Extra allocation
    vec.par_iter().map(|x| x * 2).collect()
}

// GOOD: Use par_bridge directly
fn process_set_fast(data: HashSet<i32>) -> Vec<i32> {
    data.into_iter()
        .par_bridge() // No intermediate allocation
        .map(|x| x * 2)
        .collect()
}
```
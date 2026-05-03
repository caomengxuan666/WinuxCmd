# Pitfalls

### Pitfall 1: Using par_iter on small collections
```cpp
// BAD: Overhead of parallelism exceeds benefit
use rayon::prelude::*;

fn main() {
    let small: Vec<i32> = (0..10).collect();
    let result: Vec<i32> = small.par_iter().map(|x| x * 2).collect(); // Too small for parallelism
}

// GOOD: Use sequential iteration for small collections
fn main() {
    let small: Vec<i32> = (0..10).collect();
    let result: Vec<i32> = small.iter().map(|x| x * 2).collect(); // Sequential is faster
}
```

### Pitfall 2: Mutating shared state without synchronization
```cpp
// BAD: Data race on shared vector
use rayon::prelude::*;

fn main() {
    let mut results = Vec::new();
    let data: Vec<i32> = (0..1000).collect();
    data.par_iter().for_each(|x| {
        results.push(*x * 2); // ERROR: multiple threads mutate results
    });
}

// GOOD: Use parallel collect or Mutex
use std::sync::Mutex;

fn main() {
    let results = Mutex::new(Vec::new());
    let data: Vec<i32> = (0..1000).collect();
    data.par_iter().for_each(|x| {
        let mut guard = results.lock().unwrap();
        guard.push(*x * 2);
    });
}
```

### Pitfall 3: Using non-Send types in parallel closures
```cpp
// BAD: Rc is not Send
use rayon::prelude::*;
use std::rc::Rc;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let shared = Rc::new(42);
    data.par_iter().for_each(|x| {
        let _ = shared; // ERROR: Rc<i32> is not Send
    });
}

// GOOD: Use Arc for shared ownership
use std::sync::Arc;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let shared = Arc::new(42);
    data.par_iter().for_each(|x| {
        let _ = &shared;
    });
}
```

### Pitfall 4: Ignoring panic propagation
```cpp
// BAD: Panic in parallel closure causes undefined behavior
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    data.par_iter().for_each(|x| {
        if *x == 50 {
            panic!("Something went wrong"); // Panic may cause issues
        }
    });
}

// GOOD: Handle errors gracefully
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let results: Vec<Result<i32, String>> = data.par_iter()
        .map(|x| {
            if *x == 50 {
                Err("Something went wrong".to_string())
            } else {
                Ok(*x * 2)
            }
        })
        .collect();
    
    for result in &results {
        if let Err(e) = result {
            println!("Error: {}", e);
        }
    }
}
```

### Pitfall 5: Using par_iter with non-deterministic order
```cpp
// BAD: Assuming parallel iteration preserves order
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..10).collect();
    let result: Vec<i32> = data.par_iter().map(|x| x * 2).collect();
    println!("First element: {}", result[0]); // May not be 0!
}

// GOOD: Use par_iter().enumerate() or collect into ordered structure
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..10).collect();
    let result: Vec<(usize, i32)> = data.par_iter()
        .enumerate()
        .map(|(i, x)| (i, *x * 2))
        .collect();
    // result may still be unordered, but we have original indices
}
```

### Pitfall 6: Creating too many thread pools
```cpp
// BAD: Creating new thread pool for each operation
use rayon::ThreadPoolBuilder;

fn main() {
    for i in 0..100 {
        let pool = ThreadPoolBuilder::new()
            .num_threads(4)
            .build()
            .unwrap();
        pool.install(|| {
            // Some parallel work
        }); // Creating 100 thread pools is expensive
    }
}

// GOOD: Use global pool or reuse a single pool
use rayon::ThreadPoolBuilder;
use std::sync::OnceLock;

static POOL: OnceLock<rayon::ThreadPool> = OnceLock::new();

fn main() {
    let pool = POOL.get_or_init(|| {
        ThreadPoolBuilder::new()
            .num_threads(4)
            .build()
            .unwrap()
    });
    
    for i in 0..100 {
        pool.install(|| {
            // Some parallel work
        });
    }
}
```

### Pitfall 7: Using par_iter with borrowed data that doesn't live long enough
```cpp
// BAD: Borrowed data may be dropped before parallel execution completes
use rayon::prelude::*;

fn process_data(data: &Vec<i32>) {
    data.par_iter().for_each(|x| {
        println!("{}", x);
    });
}

fn main() {
    let data;
    {
        let temp = vec![1, 2, 3];
        data = &temp; // ERROR: temp dropped here
    }
    process_data(data);
}

// GOOD: Ensure data lives long enough
fn main() {
    let data = vec![1, 2, 3];
    data.par_iter().for_each(|x| {
        println!("{}", x);
    });
}
```

### Pitfall 8: Using par_iter with non-deterministic reduction
```cpp
// BAD: Non-associative operation in reduce
use rayon::prelude::*;

fn main() {
    let data: Vec<f64> = vec![1.0, 2.0, 3.0, 4.0];
    let result: f64 = data.par_iter()
        .cloned()
        .reduce(|| 0.0, |a, b| a - b); // Subtraction is not associative!
    println!("Result: {}", result); // Non-deterministic
}

// GOOD: Use associative operations only
fn main() {
    let data: Vec<f64> = vec![1.0, 2.0, 3.0, 4.0];
    let result: f64 = data.par_iter()
        .cloned()
        .reduce(|| 0.0, |a, b| a + b); // Addition is associative
    println!("Result: {}", result);
}
```
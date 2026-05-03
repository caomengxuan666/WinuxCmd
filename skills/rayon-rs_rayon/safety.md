# Safety

### Red Line 1: Never mutate data through shared references
```cpp
// BAD: Unsafe mutation through shared reference
use rayon::prelude::*;

fn main() {
    let mut data = vec![1, 2, 3, 4, 5];
    let ref_data = &data;
    data.par_iter().for_each(|x| {
        // ERROR: Cannot mutate data through shared reference
        // ref_data.push(*x); // This would be UB
    });
}

// GOOD: Use interior mutability with proper synchronization
use std::sync::Mutex;

fn main() {
    let data = vec![1, 2, 3, 4, 5];
    let results = Mutex::new(Vec::new());
    data.par_iter().for_each(|x| {
        let mut guard = results.lock().unwrap();
        guard.push(*x * 2);
    });
}
```

### Red Line 2: Never use non-Send types in parallel closures
```cpp
// BAD: Using Rc (non-Send) in parallel context
use rayon::prelude::*;
use std::rc::Rc;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let shared = Rc::new(42);
    data.par_iter().for_each(|x| {
        let _ = shared.clone(); // ERROR: Rc is not Send
    });
}

// GOOD: Use Arc (Send + Sync) for shared ownership
use std::sync::Arc;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let shared = Arc::new(42);
    data.par_iter().for_each(|x| {
        let _ = &shared;
    });
}
```

### Red Line 3: Never create dangling references in parallel closures
```cpp
// BAD: Returning reference to local data
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let results: Vec<&i32> = data.par_iter()
        .map(|x| {
            let local = *x * 2;
            &local // ERROR: local dropped after closure
        })
        .collect();
}

// GOOD: Return owned values
fn main() {
    let data: Vec<i32> = (0..100).collect();
    let results: Vec<i32> = data.par_iter()
        .map(|x| *x * 2)
        .collect();
}
```

### Red Line 4: Never use unsafe code to bypass Send/Sync requirements
```cpp
// BAD: Using unsafe to bypass Send check
use rayon::prelude::*;
use std::rc::Rc;

struct UnsafeWrapper(Rc<i32>);
unsafe impl Send for UnsafeWrapper {} // DANGEROUS!

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let wrapper = UnsafeWrapper(Rc::new(42));
    data.par_iter().for_each(|x| {
        let _ = &wrapper; // Undefined behavior!
    });
}

// GOOD: Use proper synchronization primitives
use std::sync::Arc;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let wrapper = Arc::new(42);
    data.par_iter().for_each(|x| {
        let _ = &wrapper;
    });
}
```

### Red Line 5: Never assume parallel iteration order is deterministic
```cpp
// BAD: Relying on parallel iteration order
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..10).collect();
    let result: Vec<i32> = data.par_iter()
        .map(|x| *x * 2)
        .collect();
    // result may be [0, 2, 4, ...] or any permutation
    assert_eq!(result[0], 0); // May fail!
}

// GOOD: Use sequential iteration when order matters
fn main() {
    let data: Vec<i32> = (0..10).collect();
    let result: Vec<i32> = data.iter()
        .map(|x| *x * 2)
        .collect();
    assert_eq!(result[0], 0); // Guaranteed to pass
}
```
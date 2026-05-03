# Quickstart

```cpp
// Basic parallel iteration with par_iter
use rayon::prelude::*;

fn main() {
    let numbers: Vec<i32> = (0..1000).collect();
    let sum: i32 = numbers.par_iter().sum();
    println!("Sum: {}", sum);
}

// Parallel map and collect
use rayon::prelude::*;

fn main() {
    let data: Vec<i32> = (0..100).collect();
    let doubled: Vec<i32> = data.par_iter().map(|x| x * 2).collect();
    println!("Doubled: {:?}", doubled);
}

// Parallel sort
use rayon::prelude::*;

fn main() {
    let mut numbers: Vec<i32> = (0..1000).rev().collect();
    numbers.par_sort();
    println!("Sorted: {:?}", &numbers[..10]);
}

// Parallel join for divide-and-conquer
use rayon::join;

fn fib(n: u64) -> u64 {
    if n <= 1 { return n; }
    let (a, b) = join(|| fib(n - 1), || fib(n - 2));
    a + b
}

fn main() {
    println!("fib(20) = {}", fib(20));
}

// Parallel filter and collect
use rayon::prelude::*;

fn main() {
    let numbers: Vec<i32> = (0..1000).collect();
    let evens: Vec<i32> = numbers.par_iter()
        .filter(|&&x| x % 2 == 0)
        .copied()
        .collect();
    println!("Evens count: {}", evens.len());
}

// Parallel any/all checks
use rayon::prelude::*;

fn main() {
    let numbers: Vec<i32> = (0..1000).collect();
    let has_negative = numbers.par_iter().any(|&x| x < 0);
    let all_positive = numbers.par_iter().all(|&x| x >= 0);
    println!("Has negative: {}, All positive: {}", has_negative, all_positive);
}

// Parallel reduce with custom operation
use rayon::prelude::*;

fn main() {
    let numbers: Vec<i32> = (1..=100).collect();
    let product: i32 = numbers.par_iter()
        .cloned()
        .reduce(|| 1, |a, b| a * b);
    println!("Product: {}", product);
}

// Parallel for_each for side effects
use rayon::prelude::*;
use std::sync::Mutex;

fn main() {
    let results = Mutex::new(Vec::new());
    let data: Vec<i32> = (0..100).collect();
    data.par_iter().for_each(|x| {
        let mut guard = results.lock().unwrap();
        guard.push(*x * 2);
    });
    println!("Results: {:?}", results.lock().unwrap().len());
}
```
# Best Practices

### Use par_iter for CPU-bound work on large collections
```cpp
use rayon::prelude::*;

fn process_images(images: &[Image]) -> Vec<ProcessedImage> {
    // Only parallelize if collection is large enough
    if images.len() > 1000 {
        images.par_iter()
            .map(|img| img.process())
            .collect()
    } else {
        images.iter()
            .map(|img| img.process())
            .collect()
    }
}
```

### Use join for divide-and-conquer algorithms
```cpp
use rayon::join;

fn parallel_quicksort<T: Ord + Send>(slice: &mut [T]) {
    if slice.len() <= 1 {
        return;
    }
    let pivot_index = partition(slice);
    let (left, right) = slice.split_at_mut(pivot_index);
    join(|| parallel_quicksort(left), || parallel_quicksort(right));
}
```

### Use par_bridge for non-parallel iterators
```cpp
use rayon::prelude::*;
use std::collections::HashSet;

fn process_set(data: HashSet<i32>) -> Vec<i32> {
    data.par_bridge() // Convert sequential iterator to parallel
        .map(|x| x * 2)
        .collect()
}
```

### Use par_sort for large collections
```cpp
use rayon::prelude::*;

fn sort_large_dataset(mut data: Vec<f64>) -> Vec<f64> {
    if data.len() > 10000 {
        data.par_sort_unstable(); // Faster than stable sort
    } else {
        data.sort_unstable();
    }
    data
}
```

### Use par_chunks for grouped parallel processing
```cpp
use rayon::prelude::*;

fn process_in_batches(data: &[i32], batch_size: usize) -> Vec<i32> {
    data.par_chunks(batch_size)
        .flat_map(|chunk| chunk.iter().map(|x| x * 2))
        .collect()
}
```

### Use try_fold and try_reduce for error handling
```cpp
use rayon::prelude::*;

fn safe_process(data: &[i32]) -> Result<i32, String> {
    data.par_iter()
        .try_fold(
            || 0,
            |acc, &x| {
                if x < 0 {
                    Err("Negative value found".to_string())
                } else {
                    Ok(acc + x)
                }
            },
        )
        .try_reduce(|| 0, |a, b| Ok(a + b))
}
```
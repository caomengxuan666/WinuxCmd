# Performance

**Iteration Overhead**

The range-based for loop (`for (auto _ : state)`) has minimal overhead. The library automatically determines the number of iterations needed for statistical significance.

```cpp
static void BM_IterationOverhead(benchmark::State& state) {
  int counter = 0;
  for (auto _ : state) {
    // The loop itself adds ~1-2 ns overhead on modern CPUs
    counter++;
  }
  benchmark::DoNotOptimize(counter);
}
// Typical output: ~1-2 ns per iteration (the loop overhead)
BENCHMARK(BM_IterationOverhead);
```

**Memory Allocation Patterns**

Avoid allocations in the timed loop. Pre-allocate outside and reuse.

```cpp
static void BM_AllocationPatterns(benchmark::State& state) {
  // BAD: Allocation inside timed loop
  for (auto _ : state) {
    std::vector<int> v(1000);  // 4KB allocation each iteration
    benchmark::DoNotOptimize(v);
  }
}

static void BM_GoodAllocation(benchmark::State& state) {
  // GOOD: Pre-allocate once
  std::vector<int> v(1000);
  for (auto _ : state) {
    std::fill(v.begin(), v.end(), 42);
    benchmark::DoNotOptimize(v);
  }
}
```

**Optimization Tips**

```cpp
// Tip 1: Use constexpr for compile-time constants
static void BM_Constexpr(benchmark::State& state) {
  constexpr int size = 1000;
  std::array<int, size> arr{};
  
  for (auto _ : state) {
    for (int i = 0; i < size; ++i) {
      benchmark::DoNotOptimize(arr[i] = i * 2);
    }
  }
}

// Tip 2: Minimize branching in timed code
static void BM_NoBranching(benchmark::State& state) {
  std::vector<int> data(1000);
  std::iota(data.begin(), data.end(), 0);
  
  for (auto _ : state) {
    // Use branchless operations where possible
    for (auto& x : data) {
      x = (x > 500) ? x : 0;  // Branch
    }
    benchmark::DoNotOptimize(data);
  }
}

// Tip 3: Use appropriate data structures
static void BM_DataStructureChoice(benchmark::State& state) {
  std::vector<int> vec;
  vec.reserve(1000);
  
  for (auto _ : state) {
    vec.clear();
    for (int i = 0; i < 1000; ++i) {
      vec.push_back(i);
    }
    benchmark::DoNotOptimize(vec);
  }
}
```

**Measurement Accuracy**

```cpp
static void BM_MeasurementAccuracy(benchmark::State& state) {
  // The library handles:
  // - CPU frequency scaling (uses CPU cycles when possible)
  // - Context switches (detects and discards outliers)
  // - Cache warming (runs iterations until stable)
  
  volatile int x = 0;  // volatile prevents some optimizations
  for (auto _ : state) {
    x = 42;
  }
}
// Use --benchmark_repetitions=N for multiple runs
// Use --benchmark_min_time=1.0 for longer, more accurate runs
```
# Overview

Google Benchmark is a microbenchmarking library for C++ that measures the performance of small code snippets with statistical rigor. It automatically runs code multiple times, calculates mean/median/min/max times, and handles CPU frequency scaling and other environmental noise.

**When to use:**
- Comparing performance of different algorithms or data structures
- Regression testing for performance-critical code
- Optimizing hot paths in your application
- Validating that optimizations actually improve performance

**When NOT to use:**
- For profiling entire applications (use a profiler instead)
- For measuring I/O-bound operations (network, disk)
- When you need wall-clock time for real-world scenarios
- For benchmarks that take more than a few seconds per iteration

**Key design principles:**
- Statistical approach: runs iterations until statistically significant results
- Automatic iteration count: adjusts based on timing stability
- State management: `benchmark::State` object controls timing, iteration, and reporting
- Registration system: macros like `BENCHMARK()` register functions for execution
- Custom arguments: `Arg()`, `Range()`, `Args()` for parameterized benchmarks
- Output formats: console table, JSON, CSV, and more

```cpp
// Core structure of a benchmark
#include <benchmark/benchmark.h>

static void BM_Example(benchmark::State& state) {
  // Setup code here (not timed)
  for (auto _ : state) {
    // Code to benchmark goes here
    // The loop body is what gets timed
  }
}
BENCHMARK(BM_Example);
BENCHMARK_MAIN();
```
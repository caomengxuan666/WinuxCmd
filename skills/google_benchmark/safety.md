# Safety

**Red Line 1: NEVER call state.PauseTiming()/ResumeTiming() outside the timed loop**

BAD: Pausing/resuming outside the iteration loop causes undefined behavior
```cpp
static void BM_Bad(benchmark::State& state) {
  state.PauseTiming();  // Wrong! Must be inside the loop
  for (auto _ : state) {
    // code
  }
}
```

GOOD: Always inside the loop
```cpp
static void BM_Good(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    // non-timed setup
    state.ResumeTiming();
    // timed code
  }
}
```

**Red Line 2: NEVER modify benchmark::State from multiple threads without synchronization**

BAD: Concurrent access to State methods
```cpp
static void BM_Bad(benchmark::State& state) {
  #pragma omp parallel for
  for (int i = 0; i < 100; ++i) {
    state.SetLabel("thread");  // Data race!
  }
}
```

GOOD: Use thread-local state or synchronization
```cpp
static void BM_Good(benchmark::State& state) {
  std::mutex m;
  #pragma omp parallel for
  for (int i = 0; i < 100; ++i) {
    std::lock_guard<std::mutex> lock(m);
    // Thread-safe operations
  }
}
```

**Red Line 3: NEVER use global or static mutable state in benchmarks without resetting**

BAD: Static state persists across benchmark runs
```cpp
static void BM_Bad(benchmark::State& state) {
  static std::vector<int> data;  // Grows unbounded!
  for (auto _ : state) {
    data.push_back(42);
  }
}
```

GOOD: Local state that's properly scoped
```cpp
static void BM_Good(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> data;
    data.push_back(42);
    benchmark::DoNotOptimize(data);
  }
}
```

**Red Line 4: NEVER return from a benchmark function without completing the iteration loop**

BAD: Early return leaves State in invalid state
```cpp
static void BM_Bad(benchmark::State& state) {
  if (state.range(0) < 0) return;  // Incomplete benchmark!
  for (auto _ : state) {
    // code
  }
}
```

GOOD: Handle conditions before the loop
```cpp
static void BM_Good(benchmark::State& state) {
  if (state.range(0) < 0) {
    state.SkipWithError("Invalid range");
    return;
  }
  for (auto _ : state) {
    // code
  }
}
```

**Red Line 5: NEVER use BENCHMARK_MAIN() alongside a custom main() without proper setup**

BAD: Duplicate main functions cause linker errors
```cpp
// file1.cpp
BENCHMARK_MAIN();

// file2.cpp
int main(int argc, char** argv) {
  // Custom setup
  return 0;
}
```

GOOD: Use one or the other
```cpp
// Use either BENCHMARK_MAIN() or custom main(), not both
int main(int argc, char** argv) {
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  return 0;
}
```
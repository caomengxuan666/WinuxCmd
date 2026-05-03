# Pitfalls

**Pitfall 1: Including setup code in the timed loop**

BAD: Setup runs every iteration, skewing results
```cpp
static void BM_Bad(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> v(1000000);  // Allocation timed!
    v[0] = 42;
  }
}
```

GOOD: Setup outside the timed loop
```cpp
static void BM_Good(benchmark::State& state) {
  std::vector<int> v(1000000);
  for (auto _ : state) {
    v[0] = 42;
    benchmark::DoNotOptimize(v);
  }
}
```

**Pitfall 2: Forgetting DoNotOptimize/ClobberMemory**

BAD: Compiler may optimize away the entire benchmark
```cpp
static void BM_Bad(benchmark::State& state) {
  int x = 0;
  for (auto _ : state) {
    x = 42;  // Compiler may remove this
  }
}
```

GOOD: Prevent optimization
```cpp
static void BM_Good(benchmark::State& state) {
  int x = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(x = 42);
  }
}
```

**Pitfall 3: Using non-const reference to State incorrectly**

BAD: Modifying State in ways that break timing
```cpp
static void BM_Bad(benchmark::State& state) {
  for (auto _ : state) {
    state.SetLabel("processing");  // Called every iteration
  }
}
```

GOOD: Set labels outside the loop
```cpp
static void BM_Good(benchmark::State& state) {
  state.SetLabel("processing");
  for (auto _ : state) {
    // benchmark code
  }
}
```

**Pitfall 4: Not handling thread safety in threaded benchmarks**

BAD: Shared mutable state without synchronization
```cpp
static void BM_Bad(benchmark::State& state) {
  static int counter = 0;  // Data race!
  for (auto _ : state) {
    counter++;
  }
}
```

GOOD: Thread-local or synchronized state
```cpp
static void BM_Good(benchmark::State& state) {
  int local_counter = 0;
  for (auto _ : state) {
    local_counter++;
  }
  benchmark::DoNotOptimize(local_counter);
}
```

**Pitfall 5: Using state.range() without setting arguments**

BAD: Accessing range without defining it
```cpp
static void BM_Bad(benchmark::State& state) {
  for (auto _ : state) {
    int n = state.range(0);  // Undefined behavior if no args set
  }
}
BENCHMARK(BM_Bad);  // No Arg() or Range() specified
```

GOOD: Always set arguments
```cpp
static void BM_Good(benchmark::State& state) {
  for (auto _ : state) {
    int n = state.range(0);
    std::vector<int> v(n);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Good)->Arg(100);
```

**Pitfall 6: Ignoring the KeepRunning() return value**

BAD: Not checking if benchmark should stop
```cpp
static void BM_Bad(benchmark::State& state) {
  int i = 0;
  while (state.KeepRunning()) {  // OK but verbose
    i++;
  }
}
```

GOOD: Use range-based for loop
```cpp
static void BM_Good(benchmark::State& state) {
  int i = 0;
  for (auto _ : state) {
    i++;
  }
  benchmark::DoNotOptimize(i);
}
```
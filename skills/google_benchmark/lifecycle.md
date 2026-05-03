# Lifecycle

**Construction and Registration**

Benchmarks are registered at program startup using macros. The `BENCHMARK()` macro creates a static registration object.

```cpp
#include <benchmark/benchmark.h>

// Registration happens at static initialization time
static void BM_Lifecycle(benchmark::State& state) {
  for (auto _ : state) {
    int x = 42;
    benchmark::DoNotOptimize(x);
  }
}
// This macro creates a static object that registers BM_Lifecycle
BENCHMARK(BM_Lifecycle);

// Fixture registration
class MyFixture : public benchmark::Fixture {
public:
  MyFixture() : data(100) {}  // Constructor called once per thread
  
  void SetUp(const ::benchmark::State&) override {
    // Called before each benchmark run
    std::fill(data.begin(), data.end(), 0);
  }
  
  void TearDown(const ::benchmark::State&) override {
    // Called after each benchmark run
    data.clear();
  }
  
  std::vector<int> data;
};

BENCHMARK_F(MyFixture, BM_Test)(benchmark::State& state) {
  for (auto _ : state) {
    for (auto& x : data) {
      benchmark::DoNotOptimize(x += 1);
    }
  }
}
```

**Resource Management**

Benchmark fixtures follow RAII principles. The fixture is constructed once per thread, and SetUp/TearDown are called for each benchmark run.

```cpp
class ResourceFixture : public benchmark::Fixture {
public:
  // Constructor: allocate resources once
  ResourceFixture() : buffer(new char[1024]) {
    // Heavy allocation happens once
  }
  
  // Destructor: clean up resources
  ~ResourceFixture() override {
    delete[] buffer;
  }
  
  // SetUp: lightweight initialization before each run
  void SetUp(const ::benchmark::State&) override {
    std::fill(buffer, buffer + 1024, 0);
  }
  
  char* buffer;
};

BENCHMARK_F(ResourceFixture, BM_UseBuffer)(benchmark::State& state) {
  for (auto _ : state) {
    for (int i = 0; i < 1024; ++i) {
      benchmark::DoNotOptimize(buffer[i] = i % 256);
    }
  }
}
```

**Move Semantics and State**

The `benchmark::State` object is not movable or copyable. It's passed by reference to benchmark functions.

```cpp
// State is non-copyable, non-movable
static void BM_StateUsage(benchmark::State& state) {
  // State& is the only way to access benchmark state
  // benchmark::State s = state;  // Compile error: deleted copy constructor
  
  for (auto _ : state) {
    // Use state for timing control
    state.PauseTiming();
    // Setup
    state.ResumeTiming();
    // Timed work
  }
}
```
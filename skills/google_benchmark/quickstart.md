# Quickstart

```cpp
// Quickstart: Common usage patterns for google/benchmark

// Pattern 1: Basic benchmark with BENCHMARK_MAIN()
#include <benchmark/benchmark.h>
#include <string>

static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state) {
    std::string empty_string;
  }
}
BENCHMARK(BM_StringCreation);

static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state) {
    std::string copy(x);
  }
}
BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();

// Pattern 2: Benchmark with custom arguments
static void BM_StringCompare(benchmark::State& state) {
  std::string s1(state.range(0), '-');
  std::string s2(state.range(0), '-');
  for (auto _ : state) {
    benchmark::DoNotOptimize(s1.compare(s2));
  }
}
BENCHMARK(BM_StringCompare)->Arg(8)->Arg(64)->Arg(512);

// Pattern 3: Range-based benchmarks
static void BM_VectorPushBack(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<int> v;
    for (int i = 0; i < state.range(0); ++i) {
      v.push_back(i);
    }
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_VectorPushBack)->Range(8, 8<<10);

// Pattern 4: Multi-threaded benchmark
static void BM_ThreadedWork(benchmark::State& state) {
  for (auto _ : state) {
    // Thread-safe work here
    int x = 0;
    for (int i = 0; i < 1000; ++i) {
      benchmark::DoNotOptimize(x += i);
    }
  }
}
BENCHMARK(BM_ThreadedWork)->Threads(4);

// Pattern 5: Manual timing
static void BM_ManualTiming(benchmark::State& state) {
  int64_t item_count = 0;
  for (auto _ : state) {
    state.PauseTiming();
    // Setup work (not timed)
    std::vector<int> data(1000);
    state.ResumeTiming();
    
    // Timed work
    for (auto& x : data) {
      x = 42;
    }
    item_count += data.size();
  }
  state.SetItemsProcessed(item_count);
}
BENCHMARK(BM_ManualTiming);

// Pattern 6: Fixture-based benchmark
class MyFixture : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& state) override {
    data.resize(state.range(0));
  }
  void TearDown(const ::benchmark::State&) override {
    data.clear();
  }
  std::vector<int> data;
};

BENCHMARK_F(MyFixture, BM_ProcessData)(benchmark::State& state) {
  for (auto _ : state) {
    for (auto& x : data) {
      benchmark::DoNotOptimize(x += 1);
    }
  }
}
BENCHMARK_REGISTER_F(MyFixture, BM_ProcessData)->Arg(100)->Arg(1000);
```
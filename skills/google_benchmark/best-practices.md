# Best Practices

**Pattern 1: Use fixtures for complex setup**

```cpp
#include <benchmark/benchmark.h>

class StringBenchmark : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State&) override {
    long_string = std::string(10000, 'a');
    short_string = "hello";
  }
  
  std::string long_string;
  std::string short_string;
};

BENCHMARK_F(StringBenchmark, BM_Compare)(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(long_string.compare(short_string));
  }
}
```

**Pattern 2: Use meaningful argument ranges**

```cpp
static void BM_Sort(benchmark::State& state) {
  std::vector<int> v(state.range(0));
  for (auto _ : state) {
    std::iota(v.begin(), v.end(), 0);
    std::shuffle(v.begin(), v.end(), std::mt19937{42});
    std::sort(v.begin(), v.end());
    benchmark::DoNotOptimize(v);
  }
}
// Test powers of 2 from 1K to 1M
BENCHMARK(BM_Sort)->RangeMultiplier(2)->Range(1<<10, 1<<20);
```

**Pattern 3: Report meaningful metrics**

```cpp
static void BM_ProcessBytes(benchmark::State& state) {
  std::vector<char> buffer(state.range(0));
  int64_t bytes_processed = 0;
  
  for (auto _ : state) {
    for (auto& c : buffer) {
      c = ~c;
    }
    bytes_processed += buffer.size();
  }
  
  state.SetBytesProcessed(bytes_processed);
  state.SetItemsProcessed(bytes_processed / sizeof(char));
  state.SetLabel("byte processing");
}
BENCHMARK(BM_ProcessBytes)->Range(1<<10, 1<<16);
```

**Pattern 4: Use DoNotOptimize and ClobberMemory correctly**

```cpp
static void BM_Algorithm(benchmark::State& state) {
  std::vector<int> input(state.range(0));
  std::iota(input.begin(), input.end(), 0);
  std::vector<int> output(state.range(0));
  
  for (auto _ : state) {
    // Prevent compiler from optimizing away input
    benchmark::DoNotOptimize(input.data());
    benchmark::DoNotOptimize(output.data());
    
    std::transform(input.begin(), input.end(), output.begin(),
                   [](int x) { return x * x; });
    
    // Ensure output is actually used
    benchmark::ClobberMemory();
  }
}
```

**Pattern 5: Use custom main for advanced configuration**

```cpp
#include <benchmark/benchmark.h>

int main(int argc, char** argv) {
  ::benchmark::Initialize(&argc, argv);
  
  // Filter benchmarks
  ::benchmark::AddCustomContext("compiler", "gcc-12");
  ::benchmark::AddCustomContext("optimization", "-O3");
  
  // Run only matching benchmarks
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  return 0;
}
```
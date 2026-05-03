# Threading

**Thread Safety Guarantees**

Google Benchmark provides thread-safe benchmark execution. Each thread gets its own `benchmark::State` instance.

```cpp
#include <benchmark/benchmark.h>
#include <atomic>
#include <mutex>

// Thread-safe: Each thread has its own State
static void BM_ThreadSafe(benchmark::State& state) {
  int thread_local_counter = 0;
  
  for (auto _ : state) {
    thread_local_counter++;
    // No synchronization needed for local state
  }
  
  // Report per-thread results
  state.SetItemsProcessed(thread_local_counter);
}
BENCHMARK(BM_ThreadSafe)->ThreadRange(1, 8);
```

**Concurrent Access Patterns**

```cpp
// Pattern 1: Using thread-local storage
static void BM_ThreadLocal(benchmark::State& state) {
  thread_local std::vector<int> cache;
  cache.clear();
  cache.reserve(1000);
  
  for (auto _ : state) {
    for (int i = 0; i < 1000; ++i) {
      cache.push_back(i);
    }
    benchmark::DoNotOptimize(cache);
    cache.clear();
  }
}
BENCHMARK(BM_ThreadLocal)->Threads(4);

// Pattern 2: Shared state with synchronization
static void BM_SharedState(benchmark::State& state) {
  static std::mutex mtx;
  static std::vector<int> shared_data;
  
  for (auto _ : state) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      shared_data.push_back(state.thread_index());
    }
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_SharedState)->Threads(4);
```

**Thread Pool Management**

The library manages a thread pool. Threads are created once and reused across benchmark runs.

```cpp
static void BM_ThreadPool(benchmark::State& state) {
  // state.threads() gives the number of threads
  // state.thread_index() gives the current thread's index (0-based)
  
  for (auto _ : state) {
    // Each thread processes its portion
    int chunk_size = 1000 / state.threads();
    int start = state.thread_index() * chunk_size;
    int end = start + chunk_size;
    
    for (int i = start; i < end; ++i) {
      benchmark::DoNotOptimize(i * i);
    }
  }
}
BENCHMARK(BM_ThreadPool)->Threads(4);
```

**Thread Safety for Fixtures**

Fixture objects are created per thread, so member variables are thread-local.

```cpp
class ThreadSafeFixture : public benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State&) override {
    // Called once per thread
    local_data.clear();
  }
  
  std::vector<int> local_data;  // Thread-local
};

BENCHMARK_F(ThreadSafeFixture, BM_ThreadWork)(benchmark::State& state) {
  for (auto _ : state) {
    local_data.push_back(state.thread_index());
    benchmark::DoNotOptimize(local_data);
  }
}
BENCHMARK_REGISTER_F(ThreadSafeFixture, BM_ThreadWork)->Threads(4);
```
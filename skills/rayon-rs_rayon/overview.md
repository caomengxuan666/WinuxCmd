# Overview

Rayon is a data-parallelism library for Rust that transforms sequential iterators into parallel ones with minimal code changes. It provides a work-stealing thread pool that automatically distributes work across available CPU cores.

**When to use Rayon:**
- CPU-bound computations that can be parallelized (e.g., image processing, numerical simulations)
- Processing large collections where each element operation is independent
- Divide-and-conquer algorithms (tree traversal, merge sort)
- Any parallel map/reduce/filter pattern

**When NOT to use Rayon:**
- I/O-bound workloads (use async/await instead)
- Very small workloads where parallel overhead dominates
- Real-time systems requiring deterministic execution
- When you need fine-grained control over thread scheduling

**Key design principles:**
- **Work-stealing**: Idle threads steal tasks from busy threads' queues
- **Parallel iterators**: `par_iter()` replaces `iter()` for automatic parallelism
- **Join primitive**: `rayon::join()` for fork-join parallelism
- **Thread pool**: Global pool by default, configurable with `ThreadPoolBuilder`
- **No data races**: Rayon's API is designed to prevent data races at compile time
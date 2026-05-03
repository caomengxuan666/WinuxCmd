# Overview

oneTBB (oneAPI Threading Building Blocks) is a C++ library that provides high-level abstractions for parallel programming. It allows developers to express parallelism at a higher level than raw threads, focusing on tasks rather than threads. The library automatically manages thread pools, work stealing, and load balancing.

**When to use:**
- Data-parallel operations on collections (parallel_for, parallel_reduce)
- Task-parallel algorithms with complex dependencies
- Concurrent data structures for multi-threaded access
- Pipeline processing with multiple stages
- When you need portable, scalable parallelism across platforms

**When NOT to use:**
- Simple sequential code with no performance requirements
- Real-time systems with strict latency guarantees (task scheduling overhead)
- GPU-only workloads (use SYCL or CUDA instead)
- Very short-lived parallel regions (overhead may exceed benefits)

**Key design principles:**
- Task-based parallelism instead of thread-based
- Work stealing for automatic load balancing
- Generic programming with C++ templates
- Composable parallelism (nested parallel algorithms work correctly)
- Scalable from 1 to hundreds of cores
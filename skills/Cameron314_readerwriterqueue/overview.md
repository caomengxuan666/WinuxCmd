# Overview

`Cameron314/readerwriterqueue` is a single-producer, single-consumer (SPSC) lock-free queue for C++. It provides blazing-fast, wait-free enqueue and dequeue operations that are O(1) in the common case. The library is header-only and requires only C++11.

**When to use:**
- You have exactly one producer thread and one consumer thread
- You need maximum throughput with minimal latency
- You want to avoid locks and their associated overhead
- You need predictable, allocation-free operations (with `try_enqueue`)

**When NOT to use:**
- You need multi-producer or multi-consumer support (use `concurrentqueue` instead)
- Thread roles might change (producer becomes consumer)
- You need to destroy the queue while threads are blocked on `wait_dequeue`
- You're using GCC 4.6 or earlier (has atomic fence bugs)

**Key design:**
- Uses a contiguous block of pre-allocated memory
- Employs memory barriers instead of CAS loops for synchronization
- On x86, memory barriers compile to no-ops, making operations extremely fast
- Supports both blocking (`BlockingReaderWriterQueue`) and non-blocking variants
- Also offers a circular buffer variant with fixed capacity
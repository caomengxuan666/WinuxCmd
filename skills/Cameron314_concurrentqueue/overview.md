# Overview

`moodycamel::ConcurrentQueue<T>` is a high-performance, lock-free, multi-producer/multi-consumer (MPMC) queue for C++11 and later. It provides thread-safe FIFO queue semantics without using traditional mutex-based synchronization, achieving significantly better performance under contention.

**When to use:**
- You need a thread-safe queue for producer-consumer patterns
- Performance is critical and you're willing to accept eventual consistency
- You need bulk operations (enqueue/dequeue multiple items efficiently)
- You want a single-header, drop-in solution with no external dependencies

**When NOT to use:**
- You need strict FIFO ordering across all producers (the queue is not linearizable)
- You're on a NUMA architecture (the queue is not NUMA-aware)
- You need sequential consistency guarantees
- You only need single-producer/single-consumer (use the SPSC queue instead)
- Your elements have trivial destructors/assignment operators (Boost's queue may suffice)

**Key design:**
- Elements stored in contiguous blocks (not linked lists) for cache efficiency
- Internal sub-queues: one per producer thread
- Consumers scan sub-queues to find elements
- Lock-free using C++11 atomics (no assembly required)
- Supports both implicit (tokenless) and explicit (token-based) operations
- Memory can be pre-allocated or dynamically grown
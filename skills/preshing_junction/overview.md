# Overview

Junction is a C++ library providing high-performance concurrent hash map implementations. It offers four map variants: `ConcurrentMap_Crude`, `ConcurrentMap_Linear`, `ConcurrentMap_Leapfrog`, and `ConcurrentMap_Grampa`, each with different performance characteristics and trade-offs. The maps support lock-free concurrent access from multiple threads without mutual exclusion.

**When to use:**
- High-throughput concurrent hash map operations
- Read-heavy workloads with occasional writes
- Real-time systems where blocking is unacceptable
- Multi-threaded applications requiring scalable data structures

**When NOT to use:**
- When keys/values are not pointer-sized (must fit in `uintptr_t`)
- When you need non-invertible hash functions
- When you can't reserve null and redirect values
- When you need transactional or multi-key atomic operations

**Key design:**
- All operations are atomic with respect to each other
- `assign` uses release semantics, `get` uses consume semantics (except Crude which uses relaxed)
- Requires periodic `DefaultQSBR.update()` calls from each thread
- Uses Quiescent State Based Reclamation (QSBR) for memory management
- Hash function must be invertible (each key has unique hash)
# Overview

`skarupke/flat_hash_map` is a C++ implementation of an open-addressing hash map that stores all data in a single contiguous array, unlike `std::unordered_map` which uses separate chaining with linked lists. This design provides superior cache locality and typically 2-5x faster performance for most operations.

**When to use:**
- Performance-critical code where hash map operations are a bottleneck
- Applications that benefit from cache-friendly data structures
- Scenarios requiring fast iteration over all elements
- Memory-constrained environments (lower overhead than `std::unordered_map`)

**When NOT to use:**
- When you need stable iterators that survive insertions (iterators invalidate on rehash)
- When you need pointer stability to elements (elements move during rehash)
- When working with very large objects as values (consider storing pointers)
- When you need the full C++ standard library guarantee set

**Key design features:**
- Open addressing with quadratic probing for collision resolution
- All data stored in a single contiguous array (flat structure)
- Automatic rehashing when load factor exceeds threshold
- Support for heterogeneous lookup (C++14 and later)
- Customizable hash and key equality functions
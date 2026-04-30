# Overview


The Parallel Hashmap library provides high-performance hash map and btree implementations that serve as drop-in replacements for `std::unordered_map`, `std::unordered_set`, `std::map`, and `std::set`. It is header-only, requires only C++11 support, and offers significant performance improvements over standard library containers through the use of closed hashing and SSE2 parallel lookup instructions.

**When to use:**
- You need faster unordered associative containers than `std::unordered_map`/`std::unordered_set`
- You want memory-efficient hash tables that can operate at up to 87.5% load factor
- You need concurrent access patterns (use `parallel_flat_hash_map` variants)
- You want ordered containers with better cache locality than `std::map`/`std::set` (use btree variants)
- You need heterogeneous lookup support

**When NOT to use:**
- You require pointer/iterator stability for hash map elements (use `node_hash_map` variants instead of `flat_hash_map`)
- You need C++98 compatibility (requires C++11 minimum)
- You need deterministic behavior across runs (disable by default, enable with `PHMAP_NON_DETERMINISTIC 1`)
- You're working with pointer keys and need forward declaration support (known limitation)

**Key design features:**
- Closed hashing with SSE2/AVX2 parallel slot checking (16 slots at once)
- Flat storage: keys and values stored directly in memory array (no indirection)
- Parallel variants with internal sub-maps for reduced contention
- Btree containers with same interface as `std::map`/`std::set`
- Serialization support for trivially copyable types in flat maps
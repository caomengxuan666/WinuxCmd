# Overview

`tsl::robin_map` is a header-only C++ library that implements fast hash maps and hash sets using open-addressing with Robin Hood hashing and backward shift deletion. It provides four main classes: `tsl::robin_map`, `tsl::robin_set`, `tsl::robin_pg_map`, and `tsl::robin_pg_set`. The first two use a power-of-two growth policy for maximum speed, while the prime growth variants (`_pg`) handle poor hash functions better.

**When to use:** Choose `tsl::robin_map` when you need a high-performance hash map with fast lookups and insertions, especially when you can provide a good hash function. It's ideal for cache-friendly workloads where iterator invalidation on modification is acceptable. Use `tsl::robin_pg_map` when dealing with pointers or identity hash functions where lower bits may repeat.

**When NOT to use:** Avoid this library if you need stable iterators across insertions/erasures (all modifying operations invalidate iterators), or if you require the strong exception guarantee with types that aren't nothrow swappable and nothrow move constructible. Also avoid if you need bucket-based operations like `bucket_size()`.

**Key design:** The library uses Robin Hood hashing with backward shift deletion, which reduces probe sequences by shifting elements to maintain shorter average probe lengths. The power-of-two growth policy enables fast modulo operations using bitwise AND instead of division. The `StoreHash` template parameter can store hash values alongside keys for faster rehashing when hash functions are expensive.
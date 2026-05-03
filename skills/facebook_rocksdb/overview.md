# Overview

RocksDB is a high-performance embedded key-value store developed by Facebook, optimized for flash storage and RAM. It uses a Log-Structured Merge-Tree (LSM-tree) design, which provides excellent write performance by batching writes into sorted files and merging them in the background. The library offers flexible tradeoffs between write amplification, read amplification, and space amplification through configurable compaction strategies.

Use RocksDB when you need:
- Fast writes and efficient storage for large datasets (terabytes)
- Persistent key-value storage with configurable durability guarantees
- Embedded database for applications like caching, time-series data, or message queues
- High throughput on flash storage with multi-threaded compaction

Do NOT use RocksDB when:
- You need complex SQL queries or relational operations
- Your dataset fits entirely in memory and you don't need persistence
- You require strict ACID transactions across multiple keys
- You need a simple key-value store with minimal configuration overhead

Key design features include:
- LSM-tree architecture with leveled or universal compaction
- Pluggable compression (Snappy, Zlib, LZ4, ZSTD)
- Bloom filters for efficient point lookups
- Write-ahead log (WAL) for crash recovery
- Column families for logical data partitioning
- Merge operators for read-modify-write patterns
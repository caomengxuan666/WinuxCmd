# Overview

Zstandard (zstd) is a fast lossless compression library developed by Facebook, providing real-time compression speeds with better ratios than zlib. It's the reference implementation of the RFC 8878 standard, offering a C library with bindings for many languages.

**When to use zstd:**
- Need faster compression/decompression than zlib with comparable ratios
- Real-time compression scenarios requiring low latency
- Applications needing configurable speed/ratio tradeoffs
- Small data compression with dictionary training
- Multi-threaded compression for large datasets

**When NOT to use zstd:**
- Need maximum compression ratio regardless of speed (use LZMA/xz)
- Extremely resource-constrained embedded systems (use LZ4)
- Need hardware-accelerated compression (use specialized libraries)
- Streaming with very small buffers (overhead may dominate)

**Key design features:**
- Multiple compression levels from -5 (fastest) to 22 (best ratio)
- Streaming API for incremental compression/decompression
- Dictionary training for small data compression
- Multi-threaded compression support
- Stable format guaranteed by RFC 8878
- Single-header library option available
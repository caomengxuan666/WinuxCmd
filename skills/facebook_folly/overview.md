# Overview

Folly is Facebook's open-source C++20 library containing production-tested components used extensively at Facebook. It provides high-performance alternatives to standard library types (like `fbstring` and `fbvector`), utility functions for string manipulation, JSON handling, formatting, and concurrency primitives. The library is designed with practicality and efficiency in mind, often prioritizing performance over API simplicity.

**When to use Folly:**
- You need better performance than `std::string` for string-heavy workloads
- You work with JSON data and need efficient parsing/serialization
- You require type-safe string formatting with compile-time checks
- You need cache-friendly containers for performance-critical code
- You want production-tested utilities used at massive scale

**When NOT to use Folly:**
- For simple projects where standard library is sufficient
- When ABI stability is required (Folly provides no ABI guarantees)
- In environments where you cannot control the build system
- When you need minimal dependencies

**Key Design Principles:**
- Performance-first approach, sometimes leading to idiosyncratic APIs
- Complements rather than competes with `std` and Boost
- Flat directory structure mirroring namespace hierarchy
- No ABI compatibility guarantees between commits
- All symbols in `folly` namespace; internal details in `detail`/`internal` namespaces
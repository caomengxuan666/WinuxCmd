# Overview

Diesel is a safe, extensible ORM and query builder for Rust that provides compile-time type checking for database operations. It supports PostgreSQL, MySQL, and SQLite, and is designed to prevent runtime SQL errors by validating queries at compile time.

**When to use:**
- Building Rust applications that require database access
- Projects needing type-safe SQL queries
- Applications where compile-time validation of queries is beneficial
- Systems requiring connection pooling and transaction management

**When NOT to use:**
- Simple key-value storage (use sled or rocksdb instead)
- When you need dynamic query building without type safety
- Prototypes where rapid iteration is more important than safety
- When using NoSQL databases (Diesel only supports relational databases)

**Key design principles:**
- Compile-time query validation prevents SQL injection and type mismatches
- Schema-first approach using migrations
- Zero-cost abstractions - no runtime overhead for type safety
- Connection pooling via r2d2 integration
- Support for raw SQL when needed via `sql_query`
# Overview

libpq is the C-based client library for PostgreSQL, providing the foundational API for all PostgreSQL database interactions from C and C++ applications. It handles connection management, query execution, result processing, and transaction control directly against PostgreSQL servers.

**When to use libpq:**
- Building high-performance database applications that need direct control over PostgreSQL features
- Implementing connection pooling or custom database middleware
- Working with PostgreSQL-specific features like LISTEN/NOTIFY, COPY, or large objects
- When you need minimal overhead and maximum control over database operations

**When NOT to use libpq:**
- For simple CRUD applications where an ORM or higher-level wrapper would suffice
- When you need automatic connection pooling (consider PgBouncer or connection pool libraries)
- For applications requiring complex object-relational mapping
- When you prefer exception-based error handling over return code checking

**Key design principles:**
- libpq uses a synchronous, blocking API by default with asynchronous alternatives
- All functions return status codes that must be checked explicitly
- Memory management is manual - every `PQexec` result must be freed with `PQclear`
- Connections are not thread-safe by default; each connection should be used from one thread
- The library uses C-style strings and requires careful handling of binary data
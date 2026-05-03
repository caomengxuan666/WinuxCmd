# Overview

SQLite is a self-contained, serverless, zero-configuration, transactional SQL database engine. It is the most widely deployed database engine in the world, embedded in billions of devices and applications. The library provides a complete SQL implementation with ACID transactions, triggers, views, and support for most SQL92 features.

**When to use SQLite:**
- Embedded applications that need local data storage
- Mobile apps, desktop applications, and IoT devices
- Prototyping and development where a full database server is overkill
- Applications with low to moderate concurrent write requirements
- Single-user or small-team scenarios

**When NOT to use SQLite:**
- High-concurrency write workloads (many simultaneous writers)
- Client-server architectures with many concurrent users
- Applications requiring fine-grained access control per user
- Very large datasets (terabytes) or high-volume OLTP systems

**Key design characteristics:**
- Serverless: runs in-process, no separate server process
- Zero configuration: no setup or administration needed
- Single file database: entire database stored in one cross-platform file
- ACID compliant: atomic commits with rollback journal or WAL mode
- Small footprint: ~600KB compiled library size
- Public domain: completely free for any use
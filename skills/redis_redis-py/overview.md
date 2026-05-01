# Overview

redis-py is the official Python client library for Redis, providing a comprehensive interface to interact with Redis key-value stores. It supports all Redis commands, connection pooling, pipelines, pub/sub messaging, and both synchronous and asynchronous operations.

**When to use redis-py:**
- Building applications that need a fast, in-memory data store
- Implementing caching layers for web applications
- Managing session state across distributed systems
- Implementing real-time features like pub/sub messaging
- Using Redis data structures (hashes, lists, sets, sorted sets) for application logic

**When NOT to use redis-py:**
- For complex relational data that requires SQL queries
- When you need persistent storage with ACID transactions (Redis is primarily in-memory)
- For very large datasets that exceed available RAM
- When you need complex querying capabilities beyond Redis's key-based access

**Key design features:**
- Connection pooling for efficient resource management
- RESP3 protocol support (default) with RESP2 fallback
- Automatic response parsing with optional hiredis acceleration
- Pipeline support for batching commands
- Thread-safe connection management
- Built-in support for Redis Cluster and Sentinel deployments
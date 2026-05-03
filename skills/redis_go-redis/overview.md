# Overview

go-redis is the official Redis client library for Go, providing a comprehensive interface for interacting with Redis servers. It supports all Redis commands (except QUIT and SYNC), automatic connection pooling, Pub/Sub, pipelines, transactions, Lua scripting, Redis Sentinel, and Redis Cluster. The library is designed to be thread-safe and efficient for production use.

**When to use go-redis:**
- Building Go applications that need to interact with Redis
- Implementing caching layers, session stores, or message queues
- Working with Redis Cluster or Sentinel for high availability
- Using Redis data structures like hashes, lists, sets, sorted sets, and streams
- Implementing rate limiting, distributed locks, or pub/sub messaging

**When NOT to use go-redis:**
- For simple key-value storage where a local cache (like sync.Map) suffices
- When you need a full relational database with SQL queries
- For applications that don't require Redis's in-memory data structures or pub/sub capabilities
- When you need to interact with Redis from non-Go languages (use language-specific clients)

**Key design principles:**
- Thread-safe: all client operations are safe for concurrent use
- Connection pooling: manages a pool of connections to Redis servers
- Context-based: all operations accept context.Context for cancellation and timeouts
- Command chaining: supports method chaining for building complex commands
- Error handling: returns errors for Redis errors and uses redis.Nil for missing keys
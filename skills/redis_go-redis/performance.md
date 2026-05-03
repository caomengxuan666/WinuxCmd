# Performance

### Connection pooling performance
```go
// Optimize pool size based on workload
rdb := redis.NewClient(&redis.Options{
    Addr:         "localhost:6379",
    PoolSize:     20, // Increase for high concurrency
    MinIdleConns: 10, // Keep connections warm
    MaxRetries:   2,  // Reduce retries for latency-sensitive apps
})
```

### Use pipelines for batch operations
```go
// BAD: Individual commands (N round trips)
for i := 0; i < 1000; i++ {
    rdb.Set(ctx, fmt.Sprintf("key:%d", i), "value", 0)
}

// GOOD: Pipeline (1 round trip)
pipe := rdb.Pipeline()
for i := 0; i < 1000; i++ {
    pipe.Set(ctx, fmt.Sprintf("key:%d", i), "value", 0)
}
_, err := pipe.Exec(ctx)
```

### Use MGET/MSET for multiple keys
```go
// BAD: Multiple GET commands
val1, _ := rdb.Get(ctx, "key1").Result()
val2, _ := rdb.Get(ctx, "key2").Result()
val3, _ := rdb.Get(ctx, "key3").Result()

// GOOD: Single MGET command
vals, err := rdb.MGet(ctx, "key1", "key2", "key3").Result()
```

### Optimize data structures
```go
// BAD: Using separate keys for related data
rdb.Set(ctx, "user:1:name", "Alice", 0)
rdb.Set(ctx, "user:1:age", "30", 0)

// GOOD: Use hash for related data
rdb.HSet(ctx, "user:1", "name", "Alice", "age", 30)
```

### Use connection reuse
```go
// BAD: Creating new client for each request
func handler(w http.ResponseWriter, r *http.Request) {
    rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379"})
    defer rdb.Close()
    // Use rdb...
}

// GOOD: Reuse client across requests
var rdb *redis.Client

func init() {
    rdb = redis.NewClient(&redis.Options{Addr: "localhost:6379"})
}

func handler(w http.ResponseWriter, r *http.Request) {
    // Use rdb...
}
```
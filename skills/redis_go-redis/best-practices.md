# Best Practices

### Use connection pooling with optimal settings
```go
rdb := redis.NewClient(&redis.Options{
    Addr:         "localhost:6379",
    PoolSize:     10, // Adjust based on expected concurrency
    MinIdleConns: 3,  // Keep minimum connections ready
    MaxRetries:   3,  // Retry on transient errors
    DialTimeout:  5 * time.Second,
    ReadTimeout:  3 * time.Second,
    WriteTimeout: 3 * time.Second,
})
```

### Implement retry logic with exponential backoff
```go
rdb := redis.NewClient(&redis.Options{
    Addr: "localhost:6379",
    DialerRetries:      5,
    DialerRetryTimeout: 100 * time.Millisecond,
    DialerRetryBackoff: redis.DialRetryBackoffExponential(
        100*time.Millisecond,
        2*time.Second,
    ),
})
```

### Use pipelines for batch operations
```go
func batchSetUsers(ctx context.Context, rdb *redis.Client, users map[string]string) error {
    pipe := rdb.Pipeline()
    for key, value := range users {
        pipe.Set(ctx, key, value, 0)
    }
    _, err := pipe.Exec(ctx)
    return err
}
```

### Use transactions with WATCH for optimistic locking
```go
func transfer(ctx context.Context, rdb *redis.Client, from, to string, amount int64) error {
    tx := rdb.TxPipeline()
    defer tx.Close()

    // Watch the keys
    err := rdb.Watch(ctx, func(tx *redis.Tx) error {
        balance, err := tx.Get(ctx, from).Int64()
        if err != nil {
            return err
        }
        if balance < amount {
            return fmt.Errorf("insufficient funds")
        }
        _, err = tx.TxPipelined(ctx, func(pipe redis.Pipeliner) error {
            pipe.DecrBy(ctx, from, amount)
            pipe.IncrBy(ctx, to, amount)
            return nil
        })
        return err
    }, from, to)
    return err
}
```

### Implement health checks
```go
func healthCheck(rdb *redis.Client) error {
    ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
    defer cancel()
    
    _, err := rdb.Ping(ctx).Result()
    return err
}
```

### Use connection strings for configuration
```go
opt, err := redis.ParseURL("redis://user:password@localhost:6379/0")
if err != nil {
    log.Fatal(err)
}
rdb := redis.NewClient(opt)
```
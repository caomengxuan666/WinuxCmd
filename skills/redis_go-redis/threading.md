# Threading

### Thread safety guarantees
go-redis clients are safe for concurrent use. All operations are thread-safe and can be called from multiple goroutines simultaneously.

```go
// Safe concurrent access
rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379"})

var wg sync.WaitGroup
for i := 0; i < 10; i++ {
    wg.Add(1)
    go func(id int) {
        defer wg.Done()
        err := rdb.Set(ctx, fmt.Sprintf("key:%d", id), id, 0).Err()
        if err != nil {
            log.Printf("error: %v", err)
        }
    }(i)
}
wg.Wait()
```

### Concurrent Pub/Sub
```go
// Multiple goroutines can subscribe to different channels
pubsub := rdb.Subscribe(ctx, "channel1", "channel2")
defer pubsub.Close()

// Receive messages in separate goroutine
go func() {
    for msg := range pubsub.Channel() {
        fmt.Printf("Received: %s\n", msg.Payload)
    }
}()

// Publish from another goroutine
go func() {
    for i := 0; i < 10; i++ {
        rdb.Publish(ctx, "channel1", fmt.Sprintf("msg %d", i))
    }
}()
```

### Pipeline thread safety
```go
// Pipelines are NOT thread-safe
pipe := rdb.Pipeline()

// BAD: Concurrent pipeline operations
go func() {
    pipe.Set(ctx, "key1", "val1", 0)
}()
go func() {
    pipe.Set(ctx, "key2", "val2", 0)
}()

// GOOD: Use separate pipelines or synchronize
var mu sync.Mutex
go func() {
    mu.Lock()
    pipe.Set(ctx, "key1", "val1", 0)
    mu.Unlock()
}()
go func() {
    mu.Lock()
    pipe.Set(ctx, "key2", "val2", 0)
    mu.Unlock()
}()
```

### Connection pool thread safety
```go
// Connection pool is thread-safe
rdb := redis.NewClient(&redis.Options{
    Addr:         "localhost:6379",
    PoolSize:     10,
    MinIdleConns: 5,
})

// Safe to use from multiple goroutines
for i := 0; i < 100; i++ {
    go func(id int) {
        val, err := rdb.Get(ctx, fmt.Sprintf("key:%d", id)).Result()
        if err != nil && err != redis.Nil {
            log.Printf("error: %v", err)
        }
        fmt.Printf("value: %s\n", val)
    }(i)
}
```

### Context cancellation across goroutines
```go
// Use shared context for coordinated cancellation
ctx, cancel := context.WithCancel(context.Background())
defer cancel()

// Multiple goroutines share the same context
for i := 0; i < 5; i++ {
    go func(id int) {
        for {
            select {
            case <-ctx.Done():
                return
            default:
                rdb.Set(ctx, fmt.Sprintf("key:%d", id), id, 0)
                time.Sleep(100 * time.Millisecond)
            }
        }
    }(i)
}

// Cancel all operations
time.Sleep(2 * time.Second)
cancel()
```
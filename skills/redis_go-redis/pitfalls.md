# Pitfalls

### Pitfall 1: Not checking for redis.Nil when getting keys
```go
// BAD: Assumes key always exists
val, _ := rdb.Get(ctx, "nonexistent").Result()
fmt.Println(val) // Prints empty string, no error

// GOOD: Check for redis.Nil
val, err := rdb.Get(ctx, "nonexistent").Result()
if err == redis.Nil {
    fmt.Println("key does not exist")
} else if err != nil {
    panic(err)
} else {
    fmt.Println("key:", val)
}
```

### Pitfall 2: Ignoring context cancellation
```go
// BAD: No context timeout
ctx := context.Background()
val, err := rdb.Get(ctx, "key").Result() // Could hang forever

// GOOD: Use context with timeout
ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
defer cancel()
val, err := rdb.Get(ctx, "key").Result()
if err != nil {
    // Handle timeout or cancellation
}
```

### Pitfall 3: Not closing the client
```go
// BAD: Client never closed
func process() {
    rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379"})
    // Use rdb...
    // rdb.Close() is never called
}

// GOOD: Defer close
func process() {
    rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379"})
    defer rdb.Close()
    // Use rdb...
}
```

### Pitfall 4: Using wrong DB index
```go
// BAD: Different DB indices cause confusion
rdb1 := redis.NewClient(&redis.Options{Addr: "localhost:6379", DB: 0})
rdb2 := redis.NewClient(&redis.Options{Addr: "localhost:6379", DB: 1})
rdb1.Set(ctx, "key", "value", 0)
val, _ := rdb2.Get(ctx, "key").Result() // Returns redis.Nil!

// GOOD: Use consistent DB index
rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379", DB: 0})
rdb.Set(ctx, "key", "value", 0)
val, _ := rdb.Get(ctx, "key").Result() // Works correctly
```

### Pitfall 5: Not handling pipeline errors
```go
// BAD: Ignoring pipeline execution errors
pipe := rdb.Pipeline()
pipe.Set(ctx, "key1", "val1", 0)
pipe.Set(ctx, "key2", "val2", 0)
pipe.Exec(ctx) // Error ignored

// GOOD: Check pipeline errors
pipe := rdb.Pipeline()
pipe.Set(ctx, "key1", "val1", 0)
pipe.Set(ctx, "key2", "val2", 0)
_, err := pipe.Exec(ctx)
if err != nil {
    // Handle error
}
```

### Pitfall 6: Forgetting to close Pub/Sub connections
```go
// BAD: PubSub connection leaked
pubsub := rdb.Subscribe(ctx, "channel")
msg, _ := pubsub.ReceiveMessage(ctx)
// pubsub.Close() never called

// GOOD: Defer close
pubsub := rdb.Subscribe(ctx, "channel")
defer pubsub.Close()
msg, err := pubsub.ReceiveMessage(ctx)
```

### Pitfall 7: Using blocking operations without timeout
```go
// BAD: BLPop blocks indefinitely
val, err := rdb.BLPop(ctx, 0, "queue").Result() // 0 means no timeout

// GOOD: Use reasonable timeout
val, err := rdb.BLPop(ctx, 5*time.Second, "queue").Result()
```
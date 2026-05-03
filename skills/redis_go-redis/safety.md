# Safety

### Condition 1: NEVER ignore connection errors
```go
// BAD: Ignoring connection error
rdb := redis.NewClient(&redis.Options{Addr: "invalid:6379"})
// No ping or error check

// GOOD: Verify connection
rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379"})
_, err := rdb.Ping(ctx).Result()
if err != nil {
    log.Fatal("cannot connect to Redis:", err)
}
```

### Condition 2: NEVER use empty context
```go
// BAD: Using context.Background() without timeout
ctx := context.Background()
rdb.Set(ctx, "key", "value", 0)

// GOOD: Always use context with timeout
ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
defer cancel()
rdb.Set(ctx, "key", "value", 0)
```

### Condition 3: NEVER share clients across different Redis instances
```go
// BAD: Using same client for different Redis servers
rdb := redis.NewClient(&redis.Options{Addr: "server1:6379"})
rdb.Set(ctx, "key", "value", 0) // Goes to server1
// Later, same client used for server2

// GOOD: Create separate clients
rdb1 := redis.NewClient(&redis.Options{Addr: "server1:6379"})
rdb2 := redis.NewClient(&redis.Options{Addr: "server2:6379"})
```

### Condition 4: NEVER use unsafe string conversions for binary data
```go
// BAD: Using string for binary data
data := []byte{0x00, 0x01, 0x02}
rdb.Set(ctx, "key", string(data), 0) // May lose null bytes

// GOOD: Use proper encoding
data := []byte{0x00, 0x01, 0x02}
encoded := base64.StdEncoding.EncodeToString(data)
rdb.Set(ctx, "key", encoded, 0)
```

### Condition 5: NEVER ignore redis.Nil errors in production
```go
// BAD: Silently ignoring all errors
val, _ := rdb.Get(ctx, "key").Result()
if val == "" {
    // Wrong: empty string could be valid value
}

// GOOD: Check for redis.Nil specifically
val, err := rdb.Get(ctx, "key").Result()
if err == redis.Nil {
    // Key doesn't exist
} else if err != nil {
    // Real error
} else {
    // Use val
}
```
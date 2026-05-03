# Quickstart

```go
package main

import (
    "context"
    "fmt"
    "time"

    "github.com/redis/go-redis/v9"
)

var ctx = context.Background()

func main() {
    // Pattern 1: Basic connection and SET/GET
    rdb := redis.NewClient(&redis.Options{
        Addr:     "localhost:6379",
        Password: "",
        DB:       0,
    })
    defer rdb.Close()

    err := rdb.Set(ctx, "key", "value", 0).Err()
    if err != nil {
        panic(err)
    }

    val, err := rdb.Get(ctx, "key").Result()
    if err != nil {
        panic(err)
    }
    fmt.Println("key:", val)

    // Pattern 2: Check if key exists
    exists, err := rdb.Exists(ctx, "key").Result()
    if err != nil {
        panic(err)
    }
    fmt.Println("key exists:", exists > 0)

    // Pattern 3: Working with hashes
    err = rdb.HSet(ctx, "user:1", "name", "Alice", "age", 30).Err()
    if err != nil {
        panic(err)
    }

    name, err := rdb.HGet(ctx, "user:1", "name").Result()
    if err != nil {
        panic(err)
    }
    fmt.Println("user name:", name)

    // Pattern 4: Lists
    err = rdb.LPush(ctx, "queue", "task1", "task2", "task3").Err()
    if err != nil {
        panic(err)
    }

    task, err := rdb.LPop(ctx, "queue").Result()
    if err != nil {
        panic(err)
    }
    fmt.Println("popped task:", task)

    // Pattern 5: Expiration
    err = rdb.Set(ctx, "temp", "expires soon", 5*time.Second).Err()
    if err != nil {
        panic(err)
    }

    ttl, err := rdb.TTL(ctx, "temp").Result()
    if err != nil {
        panic(err)
    }
    fmt.Println("TTL:", ttl)

    // Pattern 6: Pipeline for batch operations
    pipe := rdb.Pipeline()
    incr := pipe.Incr(ctx, "counter")
    pipe.Expire(ctx, "counter", time.Hour)
    _, err = pipe.Exec(ctx)
    if err != nil {
        panic(err)
    }
    fmt.Println("counter:", incr.Val())

    // Pattern 7: Scan keys
    var cursor uint64
    var keys []string
    for {
        var err error
        keys, cursor, err = rdb.Scan(ctx, cursor, "user:*", 10).Result()
        if err != nil {
            panic(err)
        }
        fmt.Println("found keys:", keys)
        if cursor == 0 {
            break
        }
    }

    // Pattern 8: Pub/Sub
    pubsub := rdb.Subscribe(ctx, "channel")
    defer pubsub.Close()

    // Publish in goroutine
    go func() {
        err := rdb.Publish(ctx, "channel", "hello").Err()
        if err != nil {
            panic(err)
        }
    }()

    msg, err := pubsub.ReceiveMessage(ctx)
    if err != nil {
        panic(err)
    }
    fmt.Println("received:", msg.Payload)
}
```
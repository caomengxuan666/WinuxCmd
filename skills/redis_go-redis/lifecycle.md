# Lifecycle

### Construction
```go
// Basic construction
rdb := redis.NewClient(&redis.Options{
    Addr:     "localhost:6379",
    Password: "",
    DB:       0,
})

// From URL
opt, _ := redis.ParseURL("redis://localhost:6379/0")
rdb := redis.NewClient(opt)

// Cluster client
clusterClient := redis.NewClusterClient(&redis.ClusterOptions{
    Addrs: []string{"localhost:7000", "localhost:7001", "localhost:7002"},
})

// Sentinel client
sentinelClient := redis.NewFailoverClient(&redis.FailoverOptions{
    MasterName:    "mymaster",
    SentinelAddrs: []string{"localhost:26379"},
})
```

### Destruction
```go
// Always close the client when done
rdb := redis.NewClient(&redis.Options{Addr: "localhost:6379"})
defer rdb.Close()

// Close with error handling
if err := rdb.Close(); err != nil {
    log.Printf("error closing Redis client: %v", err)
}
```

### Resource management
```go
// Use connection pool settings
rdb := redis.NewClient(&redis.Options{
    Addr:         "localhost:6379",
    PoolSize:     10,
    MinIdleConns: 5,
    MaxRetries:   3,
    PoolTimeout:  30 * time.Second,
})

// Monitor pool stats
stats := rdb.PoolStats()
fmt.Printf("Total connections: %d\n", stats.TotalConns)
fmt.Printf("Idle connections: %d\n", stats.IdleConns)
fmt.Printf("Stale connections: %d\n", stats.StaleConns)
```

### Move semantics (Go doesn't have move semantics, but use pointers)
```go
// Pass client by pointer
func processWithRedis(rdb *redis.Client) {
    // Use rdb...
}

// Store client in struct
type Service struct {
    rdb *redis.Client
}

func NewService(rdb *redis.Client) *Service {
    return &Service{rdb: rdb}
}

func (s *Service) Close() error {
    return s.rdb.Close()
}
```
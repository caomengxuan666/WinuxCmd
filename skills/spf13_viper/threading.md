# Threading

```go
// Thread safety: Viper is NOT thread-safe by default
// BAD: Concurrent access without synchronization
go func() { viper.Set("key", "value1") }()
go func() { viper.Get("key") }() // Race condition!

// GOOD: Use mutex for concurrent access
var mu sync.RWMutex

go func() {
    mu.Lock()
    viper.Set("key", "value1")
    mu.Unlock()
}()

go func() {
    mu.RLock()
    value := viper.Get("key")
    mu.RUnlock()
}()
```

```go
// Thread safety: Use separate instances for different goroutines
// GOOD: Each goroutine gets its own Viper instance
func worker(id int) {
    v := viper.New()
    v.SetConfigName("config")
    v.AddConfigPath(".")
    v.ReadInConfig()
    
    // Safe to use v in this goroutine
    fmt.Printf("Worker %d: %s\n", id, v.GetString("worker.name"))
}

for i := 0; i < 5; i++ {
    go worker(i)
}
```

```go
// Thread safety: WatchConfig is not thread-safe
// BAD: Calling WatchConfig from multiple goroutines
go func() { viper.WatchConfig() }()
go func() { viper.WatchConfig() }() // Undefined behavior!

// GOOD: Call WatchConfig once during initialization
func init() {
    viper.SetConfigName("config")
    viper.AddConfigPath(".")
    viper.ReadInConfig()
    viper.WatchConfig() // Single call
}
```

```go
// Thread safety: OnConfigChange callback synchronization
var config Config
var mu sync.RWMutex

viper.OnConfigChange(func(e fsnotify.Event) {
    mu.Lock()
    defer mu.Unlock()
    
    var newConfig Config
    if err := viper.Unmarshal(&newConfig); err != nil {
        log.Printf("Error reloading config: %v", err)
        return
    }
    config = newConfig
})

// Safe concurrent access to config
func getConfig() Config {
    mu.RLock()
    defer mu.RUnlock()
    return config
}
```
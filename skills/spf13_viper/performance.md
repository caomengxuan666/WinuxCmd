# Performance

```go
// Performance: Get operations are O(1) after initialization
v := viper.New()
v.Set("key", "value")

// Fast: Direct key lookup
start := time.Now()
for i := 0; i < 1000000; i++ {
    _ = v.Get("key")
}
fmt.Println("1M gets:", time.Since(start))
```

```go
// Avoid repeated Unmarshal calls
// BAD: Unmarshal on every request
func handler(w http.ResponseWriter, r *http.Request) {
    var config Config
    viper.Unmarshal(&config) // Expensive operation
    // Use config...
}

// GOOD: Unmarshal once and cache
var config Config
viper.Unmarshal(&config)

func handler(w http.ResponseWriter, r *http.Request) {
    // Use cached config
    fmt.Fprintf(w, "Port: %d", config.Server.Port)
}
```

```go
// Memory allocation: Use type-specific getters
// BAD: Using Get and type assertion
value := viper.Get("port").(int) // Extra allocation

// GOOD: Use type-specific getter
port := viper.GetInt("port") // No allocation for type assertion
```

```go
// Optimization: Pre-bind environment variables
// BAD: AutomaticEnv scans all env vars
viper.AutomaticEnv() // O(n) where n = number of env vars

// GOOD: Bind only needed env vars
viper.BindEnv("database.host")
viper.BindEnv("database.port")
viper.BindEnv("server.host")
```
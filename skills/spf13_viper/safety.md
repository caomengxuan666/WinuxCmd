# Safety

### RED LINE 1: Never use Viper without setting at least one config source
```go
// BAD: No defaults, no config file, no env vars
viper.GetString("database.password") // Returns "" silently

// GOOD: Always provide defaults or validate
viper.SetDefault("database.password", "")
password := viper.GetString("database.password")
if password == "" {
    log.Fatal("database.password must be configured")
}
```

### RED LINE 2: Never read config before calling ReadInConfig
```go
// BAD: Reading before config is loaded
viper.SetConfigName("config")
viper.AddConfigPath(".")
fmt.Println(viper.GetString("host")) // Returns "" before ReadInConfig

// GOOD: Ensure config is loaded
viper.SetConfigName("config")
viper.AddConfigPath(".")
if err := viper.ReadInConfig(); err != nil {
    log.Fatal(err)
}
fmt.Println(viper.GetString("host"))
```

### RED LINE 3: Never use the same Viper instance concurrently without synchronization
```go
// BAD: Concurrent reads and writes
go func() { viper.Set("key", "value1") }()
go func() { viper.Get("key") }() // Race condition!

// GOOD: Use mutex or separate instances
var mu sync.RWMutex
go func() { mu.Lock(); viper.Set("key", "value1"); mu.Unlock() }()
go func() { mu.RLock(); viper.Get("key"); mu.RUnlock() }()
```

### RED LINE 4: Never assume environment variables are case-insensitive
```go
// BAD: Expecting case-insensitive env vars
os.Setenv("MYAPP_DATABASE_HOST", "localhost")
viper.SetEnvPrefix("MYAPP")
viper.BindEnv("database.host")
// Works because Viper uppercases, but don't rely on it

// GOOD: Use consistent casing
os.Setenv("MYAPP_DATABASE_HOST", "localhost")
viper.SetEnvPrefix("MYAPP")
viper.BindEnv("database.host")
```

### RED LINE 5: Never ignore errors from WriteConfig
```go
// BAD: Ignoring write errors
viper.WriteConfig() // Silently fails if file is read-only

// GOOD: Handle write errors
if err := viper.WriteConfig(); err != nil {
    log.Printf("Failed to write config: %v", err)
}
```
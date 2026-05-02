# Pitfalls

### BAD: Ignoring config file not found errors
```go
viper.SetConfigName("config")
viper.AddConfigPath(".")
viper.ReadInConfig() // Silently ignores missing config
// Later: viper.GetString("key") returns empty string
```

### GOOD: Properly handling missing config files
```go
viper.SetConfigName("config")
viper.AddConfigPath(".")
if err := viper.ReadInConfig(); err != nil {
    var notFound viper.FileNotFoundError
    if errors.As(err, &notFound) {
        fmt.Println("No config file found, using defaults")
    } else {
        panic(fmt.Errorf("fatal error reading config: %w", err))
    }
}
```

### BAD: Assuming case sensitivity for config keys
```go
viper.Set("DatabaseHost", "localhost")
fmt.Println(viper.GetString("databasehost")) // Works, but confusing
fmt.Println(viper.GetString("DATABASEHOST")) // Also works!
```

### GOOD: Using consistent key naming
```go
viper.Set("database.host", "localhost")
fmt.Println(viper.GetString("database.host")) // Clear and consistent
```

### BAD: Expecting deep merge of complex values
```go
viper.SetDefault("database", map[string]interface{}{
    "host": "localhost",
    "port": 5432,
})
viper.Set("database", map[string]interface{}{
    "host": "production.example.com",
})
// port is now missing! The entire map was replaced
```

### GOOD: Setting individual nested keys
```go
viper.SetDefault("database.host", "localhost")
viper.SetDefault("database.port", 5432)
viper.Set("database.host", "production.example.com")
// port remains 5432
```

### BAD: Using Get without type assertion
```go
value := viper.Get("port")
port := value.(int) // Panics if value is float64 from YAML
```

### GOOD: Using type-safe getters
```go
port := viper.GetInt("port") // Handles type conversion automatically
host := viper.GetString("host")
debug := viper.GetBool("debug")
```
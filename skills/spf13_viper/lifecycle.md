# Lifecycle

```go
// Construction: Create a new Viper instance
v := viper.New() // Creates isolated instance
v.SetConfigName("app")
v.AddConfigPath(".")
v.ReadInConfig()

// Or use the singleton
viper.SetConfigName("app")
viper.AddConfigPath(".")
viper.ReadInConfig()
```

```go
// Destruction: Viper instances are garbage collected
// No explicit cleanup needed, but stop watchers
v := viper.New()
v.WatchConfig()

// When done with the instance
v.WatchConfig() // Cannot stop watcher, just let GC handle it
// Note: WatchConfig has no Stop method - design limitation
```

```go
// Resource management: Reset and reuse
v := viper.New()
v.SetConfigName("config")
v.AddConfigPath(".")
v.ReadInConfig()

// Reset for new configuration
v.Reset() // Clears all settings, defaults, and config sources
v.SetConfigName("new-config")
v.AddConfigPath("/new/path")
v.ReadInConfig()
```

```go
// Move semantics: Viper instances are not movable
// BAD: Cannot move Viper
v1 := viper.New()
v1.Set("key", "value")
// v2 := v1 // Copy would share internal state

// GOOD: Create new instance and copy needed values
v2 := viper.New()
v2.Set("key", v1.Get("key"))
```
# Best Practices

```go
// Use a configuration struct for type safety
type Config struct {
    Server   ServerConfig   `mapstructure:"server"`
    Database DatabaseConfig `mapstructure:"database"`
}

type ServerConfig struct {
    Port int    `mapstructure:"port"`
    Host string `mapstructure:"host"`
}

type DatabaseConfig struct {
    URL  string `mapstructure:"url"`
    Pool int    `mapstructure:"pool"`
}

func LoadConfig() (*Config, error) {
    viper.SetConfigName("config")
    viper.AddConfigPath(".")
    viper.AddConfigPath("/etc/myapp/")
    viper.AddConfigPath("$HOME/.myapp")
    
    viper.SetDefault("server.port", 8080)
    viper.SetDefault("database.pool", 10)
    
    if err := viper.ReadInConfig(); err != nil {
        if _, ok := err.(viper.ConfigFileNotFoundError); !ok {
            return nil, err
        }
    }
    
    var config Config
    if err := viper.Unmarshal(&config); err != nil {
        return nil, err
    }
    return &config, nil
}
```

```go
// Use environment variable binding with consistent naming
func initConfig() {
    viper.SetEnvPrefix("MYAPP")
    viper.AutomaticEnv()
    
    // Bind specific keys to env vars
    viper.BindEnv("database.url", "DATABASE_URL")
    viper.BindEnv("server.port", "PORT")
    
    // Use underscore notation for nested keys
    viper.BindEnv("database.host")
    // Reads from MYAPP_DATABASE_HOST
}
```

```go
// Implement graceful config reloading
type App struct {
    config *Config
    mu     sync.RWMutex
}

func (a *App) WatchConfig() {
    viper.OnConfigChange(func(e fsnotify.Event) {
        a.mu.Lock()
        defer a.mu.Unlock()
        
        var newConfig Config
        if err := viper.Unmarshal(&newConfig); err != nil {
            log.Printf("Error reloading config: %v", err)
            return
        }
        a.config = &newConfig
        log.Println("Configuration reloaded successfully")
    })
    viper.WatchConfig()
}
```
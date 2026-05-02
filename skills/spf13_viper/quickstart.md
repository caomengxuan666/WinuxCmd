# Quickstart

```go
// Basic config file loading
package main

import (
    "fmt"
    "github.com/spf13/viper"
)

func main() {
    viper.SetConfigName("config")
    viper.AddConfigPath(".")
    if err := viper.ReadInConfig(); err != nil {
        panic(fmt.Errorf("fatal error config file: %w", err))
    }
    fmt.Println("Database host:", viper.GetString("database.host"))
}
```

```go
// Environment variable binding
package main

import (
    "fmt"
    "github.com/spf13/viper"
)

func main() {
    viper.SetEnvPrefix("MYAPP")
    viper.BindEnv("database.host")
    viper.AutomaticEnv()
    
    // Reads from MYAPP_DATABASE_HOST environment variable
    fmt.Println("Database host:", viper.GetString("database.host"))
}
```

```go
// Setting defaults and overrides
package main

import (
    "fmt"
    "github.com/spf13/viper"
)

func main() {
    viper.SetDefault("port", 8080)
    viper.SetDefault("debug", false)
    
    // Override at runtime
    viper.Set("debug", true)
    
    fmt.Println("Port:", viper.GetInt("port"))
    fmt.Println("Debug:", viper.GetBool("debug"))
}
```

```go
// Reading from io.Reader
package main

import (
    "bytes"
    "fmt"
    "github.com/spf13/viper"
)

func main() {
    viper.SetConfigType("yaml")
    yamlExample := []byte(`
server:
  port: 9090
  host: "localhost"
`)
    viper.ReadConfig(bytes.NewBuffer(yamlExample))
    fmt.Println("Server port:", viper.GetInt("server.port"))
}
```

```go
// Watching config file changes
package main

import (
    "fmt"
    "github.com/fsnotify/fsnotify"
    "github.com/spf13/viper"
)

func main() {
    viper.SetConfigName("config")
    viper.AddConfigPath(".")
    viper.ReadInConfig()
    
    viper.OnConfigChange(func(e fsnotify.Event) {
        fmt.Println("Config file changed:", e.Name)
    })
    viper.WatchConfig()
    
    select {} // Block forever
}
```

```go
// Writing config to file
package main

import (
    "github.com/spf13/viper"
)

func main() {
    viper.SetConfigName("config")
    viper.AddConfigPath(".")
    viper.ReadInConfig()
    
    viper.Set("new_key", "new_value")
    viper.WriteConfig() // Overwrites existing file
}
```
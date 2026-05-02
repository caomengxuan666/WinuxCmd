# Lifecycle

**Construction: Initialize commands in `init()` functions**

Commands should be constructed and configured in `init()` functions to ensure proper setup before `main()` executes.
```go
var rootCmd *cobra.Command
var configCmd *cobra.Command

func init() {
    rootCmd = &cobra.Command{
        Use:   "app",
        Short: "Application CLI",
    }
    
    configCmd = &cobra.Command{
        Use:   "config",
        Short: "Manage configuration",
        RunE: func(cmd *cobra.Command, args []string) error {
            return handleConfig(cmd, args)
        },
    }
    
    rootCmd.AddCommand(configCmd)
}
```

**Resource management: Use `PersistentPreRunE` for setup and `PersistentPostRunE` for cleanup**

Leverage Cobra's lifecycle hooks for resource management across command execution.
```go
var db *sql.DB

var rootCmd = &cobra.Command{
    Use: "app",
    PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
        var err error
        db, err = sql.Open("postgres", "connection_string")
        if err != nil {
            return fmt.Errorf("connecting to database: %w", err)
        }
        return nil
    },
    PersistentPostRunE: func(cmd *cobra.Command, args []string) error {
        if db != nil {
            return db.Close()
        }
        return nil
    },
}
```

**Move semantics: Commands are value types - use pointers for sharing**

Cobra commands are structs that should be passed as pointers to avoid copying and ensure flag state is shared.
```go
// Good: Using pointers
func createCommand(name string) *cobra.Command {
    cmd := &cobra.Command{
        Use: name,
        Run: func(cmd *cobra.Command, args []string) {
            fmt.Printf("Running %s\n", name)
        },
    }
    return cmd
}

// Usage
rootCmd.AddCommand(createCommand("serve"))
rootCmd.AddCommand(createCommand("build"))
```

**Resource cleanup: Use deferred cleanup in command execution**

When commands allocate resources, ensure proper cleanup even on errors.
```go
var processCmd = &cobra.Command{
    Use: "process <file>",
    RunE: func(cmd *cobra.Command, args []string) (err error) {
        file, err := os.Open(args[0])
        if err != nil {
            return err
        }
        defer file.Close()
        
        // Process file
        result, err := processFile(file)
        if err != nil {
            return err
        }
        
        outputFile, err := os.Create("output.txt")
        if err != nil {
            return err
        }
        defer outputFile.Close()
        
        _, err = outputFile.Write(result)
        return err
    },
}
```
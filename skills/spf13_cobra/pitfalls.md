# Pitfalls

**Pitfall 1: Not checking flag types before use**

BAD: Using flag values without type checking
```go
var count int
rootCmd.Flags().IntVar(&count, "count", 0, "number of items")
// count is always 0 if flag not provided - no way to distinguish
```

GOOD: Using pointer flags or checking if flag was set
```go
var count *int
rootCmd.Flags().IntVar(count, "count", 0, "number of items")
// Or check if flag was explicitly provided
if rootCmd.Flags().Changed("count") {
    count, _ := rootCmd.Flags().GetInt("count")
    fmt.Printf("Count was explicitly set to %d\n", count)
}
```

**Pitfall 2: Forgetting to call `MarkFlagRequired` for required flags**

BAD: Required flag not marked
```go
var configFile string
rootCmd.Flags().StringVar(&configFile, "config", "", "config file")
// User can run without --config and get empty string
```

GOOD: Mark flag as required
```go
var configFile string
rootCmd.Flags().StringVar(&configFile, "config", "", "config file (required)")
rootCmd.MarkFlagRequired("config")
// Cobra will show error if --config is missing
```

**Pitfall 3: Using `Run` instead of `RunE` and ignoring errors**

BAD: Silently swallowing errors
```go
var processCmd = &cobra.Command{
    Use: "process",
    Run: func(cmd *cobra.Command, args []string) {
        result, err := doSomething()
        if err != nil {
            fmt.Println("Error:", err) // Doesn't return error to Cobra
        }
        fmt.Println(result)
    },
}
```

GOOD: Using `RunE` to return errors properly
```go
var processCmd = &cobra.Command{
    Use: "process",
    RunE: func(cmd *cobra.Command, args []string) error {
        result, err := doSomething()
        if err != nil {
            return fmt.Errorf("processing failed: %w", err)
        }
        fmt.Println(result)
        return nil
    },
}
```

**Pitfall 4: Not handling argument validation**

BAD: No argument validation
```go
var deleteCmd = &cobra.Command{
    Use: "delete [id]",
    Run: func(cmd *cobra.Command, args []string) {
        if len(args) == 0 {
            fmt.Println("Please provide an ID")
            return
        }
        // args[0] might be empty string
    },
}
```

GOOD: Using Cobra's built-in validators
```go
var deleteCmd = &cobra.Command{
    Use: "delete <id>",
    Args: cobra.ExactArgs(1),
    Run: func(cmd *cobra.Command, args []string) {
        id := args[0]
        if id == "" {
            return fmt.Errorf("id cannot be empty")
        }
        // Safe to use args[0]
    },
}
```

**Pitfall 5: Confusing local and persistent flags**

BAD: Using local flag where persistent is needed
```go
rootCmd.Flags().BoolP("verbose", "v", false, "verbose output")
// Subcommands can't access --verbose
```

GOOD: Using persistent flags for inherited behavior
```go
rootCmd.PersistentFlags().BoolP("verbose", "v", false, "verbose output")
// All subcommands can access --verbose
```

**Pitfall 6: Not using `SilenceUsage` and `SilenceErrors` properly**

BAD: Duplicate error messages
```go
rootCmd.Execute()
// Cobra prints usage + error, then user might print error again
```

GOOD: Controlling error output
```go
rootCmd.SilenceUsage = true
rootCmd.SilenceErrors = true
if err := rootCmd.Execute(); err != nil {
    fmt.Fprintf(os.Stderr, "Error: %v\n", err)
    os.Exit(1)
}
```

**Pitfall 7: Modifying flags after `Execute()` is called**

BAD: Race condition on flags
```go
go func() {
    rootCmd.Execute()
}()
rootCmd.Flags().String("new-flag", "", "added too late")
// Flag added after execution started - undefined behavior
```

GOOD: Setting up all flags before execution
```go
func init() {
    rootCmd.Flags().String("new-flag", "", "added before execution")
}
// Then call Execute() later
```
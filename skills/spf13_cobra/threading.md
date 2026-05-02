# Threading

**Cobra commands are NOT thread-safe for modification**

Commands should be constructed and configured in a single goroutine (typically `init()` or `main()`). Concurrent modification of command structures leads to undefined behavior.
```go
// BAD: Concurrent modification
var cmd = &cobra.Command{Use: "app"}

go func() {
    cmd.Flags().String("flag1", "", "first flag") // Race condition
}()

go func() {
    cmd.Flags().String("flag2", "", "second flag") // Race condition
}()
```

```go
// GOOD: Single-threaded setup
func init() {
    cmd := &cobra.Command{Use: "app"}
    cmd.Flags().String("flag1", "", "first flag")
    cmd.Flags().String("flag2", "", "second flag")
}
```

**`Execute()` is not thread-safe - call from one goroutine only**

The `Execute()` method should be called from a single goroutine. Do not call `Execute()` concurrently on the same command tree.
```go
// BAD: Concurrent execution
go rootCmd.Execute() // Undefined behavior
go rootCmd.Execute() // Undefined behavior
```

```go
// GOOD: Single execution
if err := rootCmd.Execute(); err != nil {
    log.Fatal(err)
}
```

**Flag values are safe to read after `Execute()` returns**

After `Execute()` completes, flag values are populated and safe to read from any goroutine, as long as no further modifications occur.
```go
var verbose bool

func init() {
    rootCmd.PersistentFlags().BoolVarP(&verbose, "verbose", "v", false, "verbose")
}

func main() {
    if err := rootCmd.Execute(); err != nil {
        log.Fatal(err)
    }
    
    // Safe to read verbose from multiple goroutines after Execute()
    go func() {
        if verbose {
            fmt.Println("Verbose mode enabled")
        }
    }()
    
    go func() {
        if verbose {
            fmt.Println("Also verbose")
        }
    }()
}
```

**Use synchronization for shared state in command handlers**

If command handlers access shared state, use proper synchronization mechanisms.
```go
var counter int
var mu sync.Mutex

var incrementCmd = &cobra.Command{
    Use: "increment",
    RunE: func(cmd *cobra.Command, args []string) error {
        mu.Lock()
        counter++
        mu.Unlock()
        return nil
    },
}

// Safe to run multiple times
for i := 0; i < 10; i++ {
    go func() {
        incrementCmd.RunE(incrementCmd, []string{})
    }()
}
```
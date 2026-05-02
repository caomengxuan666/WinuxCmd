# Safety

**Red Line 1: NEVER call `Execute()` on a nil command**

BAD: Nil command causes panic
```go
var cmd *cobra.Command
cmd.Execute() // PANIC: runtime error: invalid memory address
```

GOOD: Always initialize before execution
```go
cmd := &cobra.Command{Use: "app"}
if err := cmd.Execute(); err != nil {
    log.Fatal(err)
}
```

**Red Line 2: NEVER add flags after `Execute()` has been called**

BAD: Adding flags after execution
```go
cmd := &cobra.Command{Use: "app"}
go func() {
    cmd.Execute()
}()
cmd.Flags().String("flag", "", "added too late") // UNDEFINED BEHAVIOR
```

GOOD: Set up all flags before execution
```go
cmd := &cobra.Command{Use: "app"}
cmd.Flags().String("flag", "", "set up before execution")
cmd.Execute()
```

**Red Line 3: NEVER use `Run` and `RunE` on the same command**

BAD: Both Run and RunE defined
```go
cmd := &cobra.Command{
    Use: "test",
    Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("Run")
    },
    RunE: func(cmd *cobra.Command, args []string) error {
        fmt.Println("RunE")
        return nil
    },
}
// Only RunE executes - Run is silently ignored
```

GOOD: Use only one execution function
```go
cmd := &cobra.Command{
    Use: "test",
    RunE: func(cmd *cobra.Command, args []string) error {
        fmt.Println("RunE")
        return nil
    },
}
```

**Red Line 4: NEVER ignore the error returned by `Execute()`**

BAD: Ignoring execution error
```go
cmd.Execute() // Error silently discarded
```

GOOD: Always handle execution error
```go
if err := cmd.Execute(); err != nil {
    fmt.Fprintf(os.Stderr, "Error: %v\n", err)
    os.Exit(1)
}
```

**Red Line 5: NEVER use `Args` validator that doesn't match `Use` pattern**

BAD: Mismatched args and validator
```go
cmd := &cobra.Command{
    Use: "cmd <arg1> <arg2>",
    Args: cobra.ExactArgs(1), // Expects 2 args but validates 1
    Run: func(cmd *cobra.Command, args []string) {
        // args[1] will panic
    },
}
```

GOOD: Consistent argument specification
```go
cmd := &cobra.Command{
    Use: "cmd <arg>",
    Args: cobra.ExactArgs(1),
    Run: func(cmd *cobra.Command, args []string) {
        fmt.Println(args[0])
    },
}
```
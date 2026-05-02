# Performance

**Flag parsing is fast but avoid excessive flag definitions**

Cobra's flag parsing is efficient, but defining hundreds of flags on a single command can slow down parsing. Group flags logically and use persistent flags sparingly.
```go
// Good: Minimal, well-organized flags
var queryCmd = &cobra.Command{
    Use: "query",
    RunE: func(cmd *cobra.Command, args []string) error {
        // Use flags directly
        format, _ := cmd.Flags().GetString("format")
        limit, _ := cmd.Flags().GetInt("limit")
        return executeQuery(format, limit)
    },
}

func init() {
    queryCmd.Flags().StringP("format", "f", "json", "Output format")
    queryCmd.Flags().IntP("limit", "l", 100, "Max results")
}
```

**Use `SilenceUsage` to avoid unnecessary help text generation**

When errors occur, Cobra generates help text by default. Disable this for production to avoid string allocations.
```go
// Good: Disable usage on errors for production
rootCmd.SilenceUsage = true
rootCmd.SilenceErrors = true

if err := rootCmd.Execute(); err != nil {
    log.Printf("Error: %v", err)
    os.Exit(1)
}
```

**Pre-allocate slices for argument validation**

When implementing custom argument validators, pre-allocate slices to reduce allocations.
```go
// Good: Pre-allocate for performance
var processCmd = &cobra.Command{
    Use: "process <files...>",
    Args: func(cmd *cobra.Command, args []string) error {
        if len(args) == 0 {
            return fmt.Errorf("requires at least one file")
        }
        // Pre-allocate for validation
        validFiles := make([]string, 0, len(args))
        for _, f := range args {
            if _, err := os.Stat(f); err == nil {
                validFiles = append(validFiles, f)
            }
        }
        if len(validFiles) == 0 {
            return fmt.Errorf("no valid files found")
        }
        return nil
    },
}
```

**Use `ValidArgsFunction` for dynamic completion instead of static lists**

For commands with dynamic arguments, implement completion functions that generate values on demand rather than pre-computing large lists.
```go
// Good: Dynamic completion
var deleteCmd = &cobra.Command{
    Use: "delete <id>",
    ValidArgsFunction: func(cmd *cobra.Command, args []string, toComplete string) ([]string, cobra.ShellCompDirective) {
        // Fetch only matching items from database
        items, err := db.FindItems(toComplete, 10) // Limit to 10 results
        if err != nil {
            return nil, cobra.ShellCompDirectiveError
        }
        return items, cobra.ShellCompDirectiveNoFileComp
    },
}
```
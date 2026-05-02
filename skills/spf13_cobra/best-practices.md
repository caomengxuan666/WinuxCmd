# Best Practices

**Structure commands in a clear hierarchy**

Organize commands by domain with clear parent-child relationships. Each command should represent a single action.
```go
// Good: Clear hierarchy
var rootCmd = &cobra.Command{Use: "app"}
var userCmd = &cobra.Command{Use: "user"}
var userCreateCmd = &cobra.Command{Use: "create"}
var userDeleteCmd = &cobra.Command{Use: "delete"}

userCmd.AddCommand(userCreateCmd, userDeleteCmd)
rootCmd.AddCommand(userCmd)
```

**Use `RunE` consistently for error handling**

Always return errors from `RunE` instead of printing and exiting. This allows Cobra to handle error formatting consistently.
```go
var processCmd = &cobra.Command{
    Use: "process <file>",
    Args: cobra.ExactArgs(1),
    RunE: func(cmd *cobra.Command, args []string) error {
        file := args[0]
        data, err := os.ReadFile(file)
        if err != nil {
            return fmt.Errorf("reading file: %w", err)
        }
        result, err := processData(data)
        if err != nil {
            return fmt.Errorf("processing data: %w", err)
        }
        fmt.Println(result)
        return nil
    },
}
```

**Group related flags and use persistent flags wisely**

Use persistent flags for options that apply to multiple commands, and local flags for command-specific options.
```go
func init() {
    // Global options
    rootCmd.PersistentFlags().StringP("output", "o", "text", "Output format (text|json)")
    rootCmd.PersistentFlags().BoolP("verbose", "v", false, "Verbose output")
    
    // Command-specific options
    serverCmd.Flags().IntP("port", "p", 8080, "Port to listen on")
    serverCmd.Flags().StringP("host", "H", "0.0.0.0", "Host to bind to")
}
```

**Use command groups for better help organization**

Group related subcommands together in help output for better user experience.
```go
var rootCmd = &cobra.Command{
    Use: "app",
    Groups: []*cobra.Group{
        {ID: "management", Title: "Management Commands"},
        {ID: "utility", Title: "Utility Commands"},
    },
}

var startCmd = &cobra.Command{
    Use:     "start",
    GroupID: "management",
}
var backupCmd = &cobra.Command{
    Use:     "backup",
    GroupID: "utility",
}
```

**Implement shell completion for better UX**

Provide completion functions for dynamic arguments to improve user experience.
```go
var deployCmd = &cobra.Command{
    Use: "deploy <environment>",
    ValidArgsFunction: func(cmd *cobra.Command, args []string, toComplete string) ([]string, cobra.ShellCompDirective) {
        environments := []string{"dev", "staging", "production"}
        return environments, cobra.ShellCompDirectiveNoFileComp
    },
    RunE: func(cmd *cobra.Command, args []string) error {
        env := args[0]
        return deployTo(env)
    },
}
```
# Quickstart

```go
package main

import (
	"fmt"
	"os"
	"github.com/spf13/cobra"
)

// Pattern 1: Simple command with Run
var rootCmd = &cobra.Command{
	Use:   "app",
	Short: "A brief description",
	Long:  `A longer description that spans multiple lines.`,
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("Hello from app!")
	},
}

// Pattern 2: Command with flags
var greetCmd = &cobra.Command{
	Use:   "greet [name]",
	Short: "Greet someone",
	Args:  cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		name := args[0]
		uppercase, _ := cmd.Flags().GetBool("uppercase")
		if uppercase {
			fmt.Printf("HELLO %s!\n", name)
		} else {
			fmt.Printf("Hello %s!\n", name)
		}
	},
}

// Pattern 3: Persistent flags (available to subcommands)
var verbose bool

var serverCmd = &cobra.Command{
	Use:   "server",
	Short: "Start the server",
	Run: func(cmd *cobra.Command, args []string) {
		port, _ := cmd.Flags().GetInt("port")
		if verbose {
			fmt.Printf("Starting server on port %d (verbose mode)\n", port)
		} else {
			fmt.Printf("Starting server on port %d\n", port)
		}
	},
}

// Pattern 4: Command with required flags
var configCmd = &cobra.Command{
	Use:   "config",
	Short: "Manage configuration",
	Run: func(cmd *cobra.Command, args []string) {
		file, _ := cmd.Flags().GetString("file")
		format, _ := cmd.Flags().GetString("format")
		fmt.Printf("Loading config from %s in %s format\n", file, format)
	},
}

// Pattern 5: Subcommands with parent command
var dbCmd = &cobra.Command{
	Use:   "db",
	Short: "Database operations",
}

var dbMigrateCmd = &cobra.Command{
	Use:   "migrate",
	Short: "Run database migrations",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("Running migrations...")
	},
}

// Pattern 6: Command with custom argument validation
var validateCmd = &cobra.Command{
	Use:   "validate [email]",
	Short: "Validate an email address",
	Args: func(cmd *cobra.Command, args []string) error {
		if len(args) != 1 {
			return fmt.Errorf("requires exactly one email address")
		}
		if len(args[0]) < 5 {
			return fmt.Errorf("email address too short")
		}
		return nil
	},
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Printf("Validating email: %s\n", args[0])
	},
}

// Pattern 7: Command with aliases
var listCmd = &cobra.Command{
	Use:     "list",
	Aliases: []string{"ls", "show"},
	Short:   "List items",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("Listing items...")
	},
}

// Pattern 8: Command with completion
var completionCmd = &cobra.Command{
	Use:   "completion [bash|zsh|fish|powershell]",
	Short: "Generate completion script",
	Args:  cobra.ExactValidArgs(1),
	ValidArgsFunction: func(cmd *cobra.Command, args []string, toComplete string) ([]string, cobra.ShellCompDirective) {
		return []string{"bash", "zsh", "fish", "powershell"}, cobra.ShellCompDirectiveNoFileComp
	},
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Printf("Generating %s completion...\n", args[0])
	},
}

func init() {
	// Add flags to greet command
	greetCmd.Flags().BoolP("uppercase", "u", false, "Print in uppercase")

	// Add persistent flag to root (available to all subcommands)
	rootCmd.PersistentFlags().BoolVarP(&verbose, "verbose", "v", false, "verbose output")

	// Add flags to server command
	serverCmd.Flags().IntP("port", "p", 8080, "Port to listen on")

	// Add required flags to config command
	configCmd.Flags().StringP("file", "f", "", "Configuration file (required)")
	configCmd.Flags().StringP("format", "t", "yaml", "Configuration format")
	configCmd.MarkFlagRequired("file")

	// Build command tree
	rootCmd.AddCommand(greetCmd)
	rootCmd.AddCommand(serverCmd)
	rootCmd.AddCommand(configCmd)
	rootCmd.AddCommand(dbCmd)
	rootCmd.AddCommand(validateCmd)
	rootCmd.AddCommand(listCmd)
	rootCmd.AddCommand(completionCmd)

	// Add subcommands to db
	dbCmd.AddCommand(dbMigrateCmd)
}

func main() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
```
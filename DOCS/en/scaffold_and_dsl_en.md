# Scaffold and DSL Documentation

## Scaffold Tool Overview

The scaffold tool is designed to streamline the creation of new command modules for the WinuxCmd project. It generates boilerplate code that follows the project's conventions and best practices, allowing developers to focus on implementing the core functionality of the command. The tool now generates code that uses the new pipeline-based architecture.

### Key Features

- **Automated Code Generation**: Creates all necessary files and boilerplate code for a new command
- **Consistent Structure**: Ensures all commands follow the same structure and conventions
- **Integration with DSL**: Generates code that uses the project's DSL for option handling and command registration
- **Error Handling**: Includes basic error handling patterns using `core::pipeline::Result`
- **Documentation Template**: Provides placeholders for command documentation
- **Pipeline Architecture**: Generates code that follows the new pipeline-based architecture

### How to Use

To create a new command module using the scaffold tool, run the following command:

```bash
# From the project root directory
python scripts/scaffold.py <command_name> [options]

# Example: Create a new command called "example"
python scripts/scaffold.py example
```

### Generated Files

When you run the scaffold tool, it generates the following files:

1. **`src/cmd/<command_name>.cppm`**: The main command module file

### Generated Code Structure

The generated code follows this structure, which implements the new pipeline-based architecture:

```cpp
// Main module file (<command_name>.cppm)
export module cmd.<command_name>;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace <command_name>_constants {
    // Command-specific constants
}

// Option definitions
export auto constexpr <COMMAND_NAME>_OPTIONS =
    std::array{
        // Option definitions here
    };

namespace <command_name>_pipeline {
    namespace cp = core::pipeline;

    // Validate arguments
    auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
        std::vector<std::string> validated_args;
        for (auto arg : args) {
            validated_args.push_back(std::string(arg));
        }
        return validated_args;
    }

    // Process command
    template<size_t N>
    auto process_command(const CommandContext<N>& ctx)
        -> cp::Result<std::vector<std::string>>
    {
        auto args_result = validate_arguments(ctx.positionals);
        if (!args_result) {
            return args_result.error();
        }
        return *args_result;
    }

}

REGISTER_COMMAND(<command_name>,
                 /* name */
                 "<command_name>",

                 /* synopsis */
                 "<command_name> [options] [arguments]",

                 /* description */
                 "<Command description>",

                 /* examples */
                 "<Command examples>",

                 /* see_also */
                 "<See also commands>",

                 /* author */
                 "WinuxCmd Team",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 <COMMAND_NAME>_OPTIONS
) {
    using namespace <command_name>_pipeline;

    auto result = process_command(ctx);
    if (!result) {
        cp::report_error(result, L"<command_name>");
        return 1;
    }

    auto args = *result;

    // Command implementation here

    return 0;
}
```

## DSL Design Overview

The WinuxCmd project uses a Domain-Specific Language (DSL) to simplify command implementation, option handling, and command registration. The DSL is implemented through macros and template metaprogramming to provide a clean, declarative syntax. The DSL has been updated to support the new pipeline-based architecture.

### Design Principles

- **Declarative Syntax**: Focus on what the command does, not how to parse options
- **Type Safety**: Use C++ type system to catch errors at compile time
- **Minimal Boilerplate**: Reduce repetitive code
- **Consistency**: Ensure all commands follow the same patterns
- **Extensibility**: Allow for custom option types and handling
- **Pipeline Architecture**: Support for the new pipeline-based command processing

### Core Components

#### 1. Option Definition

The `OPTION` macro is used to define command options:

```cpp
OPTION("<short_option>", "<long_option>", "<description>")
```

Example:

```cpp
OPTION("-l", "--list", "Use a long listing format")
```

#### 2. Command Registration

The `REGISTER_COMMAND` macro is used to register a command with the system:

```cpp
REGISTER_COMMAND(
    <command_name>,        // Command identifier
    "<command_name>",      // Command name
    "<synopsis>",          // Command synopsis
    "<description>",       // Command description
    "<examples>",          // Command examples
    "<see_also>",          // See also commands
    "<author>",            // Author
    "<copyright>",         // Copyright
    <OPTIONS_ARRAY>         // Options array
) {
    // Command implementation here
}
```

#### 3. CommandContext

The `CommandContext` class is used to access parsed options in a type-safe manner:

```cpp
// Get a boolean option (default: false)
bool option_value = ctx.get<bool>("--option-name", false);

// Get a string option (default: "")
std::string option_value = ctx.get<std::string>("--option-name", "");

// Get an integer option (default: 0)
int option_value = ctx.get<int>("--option-name", 0);
```

#### 4. Pipeline Components

The DSL supports the creation of pipeline components for command processing:

```cpp
namespace <command_name>_pipeline {
    namespace cp = core::pipeline;

    // Validate arguments
    auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
        // Validation logic here
    }

    // Process command
    template<size_t N>
    auto process_command(const CommandContext<N>& ctx) -> cp::Result<ReturnType> {
        // Processing logic here
    }

}
```

### Using the DSL with Pipeline Architecture

To use the DSL with the new pipeline-based architecture, follow these steps:

1. **Import Required Modules**: Import `core` and `utils` modules
2. **Define Constants**: Create a namespace for command-specific constants
3. **Define Options**: Use the `OPTION` macro to define command options
4. **Create Pipeline Components**: Implement validation and processing functions
5. **Implement Command Logic**: Write the main command logic using the pipeline components
6. **Register Command**: Use the `REGISTER_COMMAND` macro to register the command

### Example: Full Command Implementation (Pipeline Architecture)

```cpp
// echo.cppm
export module cmd.echo;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace echo_constants {
    // Command-specific constants
}

// Option definitions
export auto constexpr ECHO_OPTIONS =
    std::array{
        OPTION("-n", "--no-newline", "Do not output a trailing newline"),
        OPTION("-e", "--escape", "Enable interpretation of backslash escapes"),
    };

namespace echo_pipeline {
    namespace cp = core::pipeline;

    // Validate arguments
    auto validate_arguments(std::span<const std::string_view> args) -> cp::Result<std::vector<std::string>> {
        std::vector<std::string> validated_args;
        for (auto arg : args) {
            validated_args.push_back(std::string(arg));
        }
        return validated_args;
    }

    // Build text from arguments
    auto build_text(const std::vector<std::string>& args) -> std::string {
        std::string text;
        for (size_t i = 0; i < args.size(); ++i) {
            text += args[i];
            if (i < args.size() - 1) {
                text += " ";
            }
        }
        return text;
    }

    // Process command
    template<size_t N>
    auto process_command(const CommandContext<N>& ctx)
        -> cp::Result<std::string>
    {
        auto args_result = validate_arguments(ctx.positionals);
        if (!args_result) {
            return args_result.error();
        }
        auto args = *args_result;
        return build_text(args);
    }

}

REGISTER_COMMAND(echo,
                 /* name */
                 "echo",

                 /* synopsis */
                 "echo [OPTION]... [STRING]...",

                 /* description */
                 "Display a line of text.",

                 /* examples */
                 "echo Hello World      Display 'Hello World'\n"
                 "echo -n Hello         Display 'Hello' without trailing newline\n"
                 "echo -e Hello\\nWorld  Display 'Hello' and 'World' on separate lines",

                 /* see_also */
                 "cat, printf",

                 /* author */
                 "WinuxCmd Team",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 ECHO_OPTIONS
) {
    using namespace echo_pipeline;

    auto result = process_command(ctx);
    if (!result) {
        cp::report_error(result, L"echo");
        return 1;
    }

    auto text = *result;

    // Get options using CommandContext
    bool no_newline = ctx.get<bool>("--no-newline", false);
    bool escape = ctx.get<bool>("--escape", false);

    // If enabled, process escape sequences
    if (escape) {
        // Implement escape sequence processing
    }

    // Output result
    std::cout << text;
    if (!no_newline) {
        std::cout << std::endl;
    }

    return 0;
}
```

### Advanced Features

#### Custom Option Types

The DSL supports custom option types through template specialization:

```cpp
// Custom option type for integer values
struct IntOption {
    int value;
};

// Specialize OptionType for IntOption
template<>
struct OptionType<IntOption> {
    static constexpr auto name = "int";
    static constexpr auto parse = [](const std::string_view& value) -> std::optional<IntOption> {
        try {
            return IntOption{std::stoi(std::string(value))};
        } catch (...) {
            return std::nullopt;
        }
    };
};

// Use custom option type
OPTION("-n", "--number", "Specify a number", OptionType<IntOption>{})
```

#### Subcommands

The DSL supports subcommands through nested command registration:

```cpp
// Register main command
REGISTER_COMMAND(
    git,
    "git",
    "git [subcommand] [options]",
    "Git version control system",
    "git status\ngit commit",
    "",
    "WinuxCmd Team",
    "Copyright © 2026 WinuxCmd",
    GIT_OPTIONS
) {
    // Main git command implementation
    return 0;
};

// Register subcommand
REGISTER_COMMAND(
    git_status,
    "git status",
    "git status [options]",
    "Show the working tree status",
    "git status",
    "git",
    "WinuxCmd Team",
    "Copyright © 2026 WinuxCmd",
    GIT_STATUS_OPTIONS
) {
    // git status implementation
    return 0;
};
```

## Best Practices

### For Scaffold Tool

1. **Use Descriptive Command Names**: Choose command names that clearly describe what the command does
2. **Follow Naming Conventions**: Use lowercase for command names, uppercase for constants
3. **Keep Options Simple**: Start with the most essential options, add more later
4. **Document Thoroughly**: Fill in all documentation placeholders
5. **Test Early**: Test the generated command structure before adding complex logic
6. **Follow Pipeline Architecture**: Use the generated pipeline components structure

### For DSL Usage

1. **Use the OPTION Macro**: Always use the `OPTION` macro for option definitions
2. **Follow Option Naming**: Use `-` for short options, `--` for long options
3. **Provide Clear Descriptions**: Write clear, concise descriptions for options
4. **Register Commands Properly**: Fill in all fields of the `REGISTER_COMMAND` macro
5. **Handle Errors Gracefully**: Use `core::pipeline::Result` for error handling
6. **Use CommandContext**: Use `CommandContext` for type-safe option access
7. **Implement Pipeline Components**: Separate validation and processing logic into pipeline components

## Troubleshooting

### Common Issues

1. **Option Parsing Errors**: Check that options are defined correctly and that required arguments are provided
2. **Command Registration Failures**: Ensure all fields of the `REGISTER_COMMAND` macro are filled in
3. **Compilation Errors**: Check that all required modules are imported and that the code follows C++23 syntax
4. **Pipeline Component Errors**: Ensure pipeline components return `core::pipeline::Result` and handle errors properly

### Debugging Tips

1. **Enable Debug Output**: Set the `WINUX_DEBUG` environment variable to `1` to enable debug output
2. **Check Generated Code**: Examine the generated code for syntax errors or missing components
3. **Test with Simple Commands**: Start with a simple command implementation and build up complexity
4. **Refer to Existing Commands**: Look at how existing commands are implemented, especially `echo.cppm` which serves as a reference

## Conclusion

The scaffold tool and DSL are powerful tools that simplify the creation and maintenance of commands in the WinuxCmd project. With the new pipeline-based architecture, commands are now more modular, easier to test, and better organized. By following the patterns and best practices described in this document, developers can create consistent, well-structured commands that integrate seamlessly with the project's architecture.

For more information, refer to the project's README.md file and the existing command implementations, especially `echo.cppm` which serves as a reference for the new pipeline-based architecture.
# Command Implementation Status

This document tracks the implementation status of commands in the WinuxCmd project, which now uses a pipeline-based architecture for command processing.

## Categories

### File Management

| Command | Status | Priority | Description | Parameters/Options | Implementation Notes |
|---------|--------|----------|-------------|-------------------|---------------------|
| `ls` | ✅ Done | High | List directory contents | `-a, --all`: Do not ignore entries starting with `.`<br>`-l, --list`: Use a long listing format<br>`-h, --human-readable`: With `-l`, print sizes in human readable format<br>`-r, --reverse`: Reverse order while sorting<br>`-t`: Sort by modification time<br>`--color`: Colorize output | Implemented with pipeline architecture, supports color output |
| `cat` | ✅ Done | High | Concatenate files and print on the standard output | `-n, --number`: Number all output lines<br>`-b, --number-nonblank`: Number non-empty output lines<br>`-E, --show-ends`: Display $ at end of each line<br>`-s, --squeeze-blank`: Squeeze multiple adjacent empty lines | Simple file reading and writing with pipeline structure |
| `cp` | ✅ Done | High | Copy files and directories | `-i, --interactive`: Prompt before overwrite<br>`-r, --recursive`: Copy directories recursively<br>`-v, --verbose`: Explain what is being done<br>`-f, --force`: Force copy without prompt | File system operations with error handling |
| `mv` | ✅ Done | High | Move (rename) files | `-i, --interactive`: Prompt before overwrite<br>`-v, --verbose`: Explain what is being done<br>`-f, --force`: Do not prompt before overwriting<br>`-n, --no-clobber`: Do not overwrite an existing file | File system operations with error handling |
| `rm` | ✅ Done | High | Remove files or directories | `-f, --force`: Ignore nonexistent files and arguments, never prompt<br>`-i, --interactive`: Prompt before every removal<br>`-r, -R, --recursive`: Remove directories and their contents recursively<br>`-v, --verbose`: Explain what is being done | File system operations with error handling |
| `mkdir` | ✅ Done | High | Make directories | `-p, --parents`: No error if existing, make parent directories as needed<br>`-v, --verbose`: Print a message for each created directory<br>`-m, --mode`: Set file mode (as in chmod) | File system operations with error handling |
| `rmdir` | ✅ Done | Medium | Remove empty directories | `--ignore-fail-on-non-empty`: Ignore each failure to remove a directory that is non-empty<br>`-p, --parents`: Remove DIRECTORY and its ancestors<br>`-v, --verbose`: Print a message for each removed directory | File system operations with error handling |
| `touch` | ✅ Done | Medium | Change file timestamps or create empty files | `-a`: Change only the access time<br>`-m`: Change only the modification time<br>`-c, --no-create`: Do not create any files<br>`-d, --date`: Parse STRING and use it instead of current time<br>`-r, --reference`: Use this file's times instead of current time | File system operations with error handling |

### Text Processing

| Command | Status | Priority | Description | Parameters/Options | Implementation Notes |
|---------|--------|----------|-------------|-------------------|---------------------|
| `echo` | ✅ Done | High | Display a line of text | `-n`: Do not output the trailing newline<br>`-e`: Enable interpretation of backslash escapes<br>`-E`: Disable interpretation of backslash escapes | Implemented with pipeline architecture and serves as reference implementation |
| `grep` | ✅ Done | High | Print lines matching a pattern | `-E, --extended-regexp`: Interpret PATTERNS as extended regular expressions<br>`-F, --fixed-strings`: Interpret PATTERNS as fixed strings<br>`-G, --basic-regexp`: Interpret PATTERNS as basic regular expressions<br>`-i, --ignore-case`: Ignore case distinctions<br>`-v, --invert-match`: Invert the sense of matching<br>`-n, --line-number`: Prefix each line of output with the 1-based line number<br>`-c, --count`: Print only a count of selected lines<br>`-l, --files-with-matches`: Print only names of FILEs containing matches<br>`-L, --files-without-match`: Print only names of FILEs containing no match<br>`--color`: Use markers to highlight matches | Pattern matching with regex support |
| `sort` | ✅ Done | Medium | Sort lines of text files | `-b, --ignore-leading-blanks`: Ignore leading blanks
`-f, --ignore-case`: Fold lower case to upper case characters
`-n, --numeric-sort`: Compare according to string numerical value
`-r, --reverse`: Reverse the result of comparisons
`-u, --unique`: With -c, check for strict ordering
`-k, --key`: Sort via a key
`-t, --field-separator`: Use SEP instead of non-blank to blank transition | Sorting with multiple key support |
| `wc` | ✅ Done | Medium | Print newline, word, and byte counts for each file | `-c, --bytes`: Print the byte counts<br>`-l, --lines`: Print the newline counts<br>`-w, --words`: Print the word counts<br>`-m, --chars`: Print the character counts<br>`-L, --max-line-length`: Print the maximum display width | Character counting with multiple modes |
| `head` | ✅ Done | Low | Output the first part of files | `-n, --lines`: Print the first N lines instead of the first 10<br>`-c, --bytes`: Print the first N bytes<br>`-q, --quiet, --silent`: Never print headers giving file names<br>`-v, --verbose`: Always print headers giving file names | File head extraction with byte/line options |
| `tail` | ✅ Done | Low | Output the last part of files | `-n, --lines`: Output the last N lines<br>`-c, --bytes`: Output the last N bytes<br>`-z, --zero-terminated`: Line delimiter is NUL, not newline<br>`-f, --follow`: Output appended data as the file grows | File tail extraction with follow support |
| `sed` | ✅ Done | Medium | Stream editor for filtering and transforming text | `-e, --expression`: Add SCRIPT to the commands to be executed
`-f, --file`: Add the contents of SCRIPT-FILE to the commands
`-n, --quiet, --silent`: Suppress automatic printing of pattern space
`-i[SUFFIX], --in-place[=SUFFIX]`: Edit files in place | Text stream editing with script support |
| `uniq` | ✅ Done | Medium | Report or omit repeated lines | `-c, --count`: Prefix lines by the number of occurrences
`-d, --repeated`: Only print duplicate lines, one for each group
`-f, --skip-fields`: Avoid comparing the first N fields
`-i, --ignore-case`: Ignore differences in case when comparing
`-s, --skip-chars`: Avoid comparing the first N characters
`-u, --unique`: Only print unique lines
`-w, --check-chars`: Compare no more than N characters in lines | Duplicate line detection and filtering |
| `cut` | ✅ Done | Medium | Remove sections from each line of files | `-d, --delimiter`: Use DELIM instead of TAB for field delimiter
`-f, --fields`: Select only these fields
`-s, --only-delimited`: Do not print lines not containing delimiters
`-z, --zero-terminated`: Line delimiter is NUL, not newline | Field extraction from delimited text |

### System Information

| Command | Status | Priority | Description | Parameters/Options | Implementation Notes |
|---------|--------|----------|-------------|-------------------|---------------------|
| `which` | ✅ Done | High | Locate a command in PATH | `-a, --all`: Print all matching pathnames of each argument<br>`--skip-dot`: Skip directories in PATH that start with a dot<br>`--skip-tilde`: Skip directories in PATH that start with a tilde | Path searching with PATHEXT support |
| `env` | ✅ Done | Medium | Run a command in a modified environment | `-i, --ignore-environment`: Start with an empty environment
`-u, --unset`: Remove variable from the environment
`-0, --null`: End each output line with NUL, not newline | Environment variable manipulation |

### Network

| Command | Status | Priority | Description | Parameters/Options | Implementation Notes |
|---------|--------|----------|-------------|-------------------|---------------------|
| `ping` | ❌ TODO | High | Send ICMP ECHO_REQUEST to network hosts | `-c COUNT`: Stop after sending COUNT ECHO_REQUEST packets<br>`-i INTERVAL`: Wait INTERVAL seconds between sending each packet | Not yet implemented |
| `curl` | ❌ TODO | Medium | Transfer data from or to a server | `-s, --silent`: Silent mode<br>`-o, --output`: Write output to a file instead of stdout | Not yet implemented |

### Other

| Command | Status | Priority | Description | Parameters/Options | Implementation Notes |
|---------|--------|----------|-------------|-------------------|---------------------|
| `help` | ✅ Done | High | Display help information about commands | `COMMAND`: Display help for specific command | Uses command metadata for help generation |
| `exit` | ✅ Done | High | Exit the shell | No options | Simple exit call with pipeline structure |
| `clear` | ✅ Done | High | Clear the terminal screen | No options | Uses system call to clear screen |
| `cd` | ✅ Done | High | Change the current directory | `-L, --logical`: Force symbolic links to be followed<br>`-P, --physical`: Use the physical directory structure without following symbolic links | Uses SetCurrentDirectory API with pipeline structure |
| `type` | ❌ TODO | Medium | Describe command type | `COMMAND`: Describe specific command | Not yet implemented |
| `alias` | ❌ TODO | Low | Create an alias for a command | `NAME=VALUE`: Define an alias<br>`-p`: Print all defined aliases | Not yet implemented |

## Implementation Guidelines

### General Structure

Each command should follow this general structure:

1. **Module Declaration**: Start with `export module cmd.<command>;`
2. **Imports**: Import necessary modules (`import core;`, `import utils;`, etc.)
3. **Constants Namespace**: Define command-specific constants
4. **Option Definitions**: Define command options using the `OPTION` macro
5. **Pipeline Components**: Implement pipeline components for command processing
6. **Command Implementation**: Implement the main command logic
7. **Registration**: Register the command using `REGISTER_COMMAND` macro

### Example Structure

```cpp
export module cmd.echo;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace echo_constants {
    // Command-specific constants
}

// Define options
export auto constexpr ECHO_OPTIONS =
    std::array{
        OPTION("-n", "--no-newline", "do not output the trailing newline"),
        OPTION("-e", "--escape", "enable interpretation of backslash escapes"),
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

    // Handle escape sequences
    auto to_uppercase(std::string text) -> std::string {
        std::ranges::transform(text, text.begin(), ::toupper);
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
                 "echo [SHORT-OPTION]... [STRING]...",

                 /* description */
                 "Display a line of text.",

                 /* examples */
                 "  echo Hello World      Display 'Hello World'\n"
                 "  echo -n Hello         Display 'Hello' without trailing newline\n"
                 "  echo -e Hello\\nWorld  Display 'Hello' and 'World' on separate lines",

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

    // Get options
    bool no_newline = ctx.get<bool>("--no-newline", false);
    bool escape = ctx.get<bool>("--escape", false);

    // Handle escape sequences if enabled
    if (escape) {
        // Implement escape sequence handling
    }

    // Output result
    std::cout << text;
    if (!no_newline) {
        std::cout << std::endl;
    }

    return 0;
}
```

### Best Practices

1. **Pipeline Architecture**: Use pipeline components for modular processing
2. **Type Safety**: Use `CommandContext` for type-safe option access
3. **Error Handling**: Use `core::pipeline::Result` for consistent error handling
4. **Documentation**: Provide clear documentation for each command
5. **Testing**: Test commands with various inputs and options
6. **Performance**: Optimize for performance where appropriate
7. **Compatibility**: Follow POSIX standards where possible

### Testing

Each command should be tested with:

1. **Basic functionality**: Test the command with no options
2. **All options**: Test each option individually
3. **Combined options**: Test multiple options together
4. **Edge cases**: Test with empty inputs, large inputs, etc.
5. **Error cases**: Test with invalid inputs and options

## Migration Guide

### From Legacy Implementation

To migrate a command from the legacy implementation to the new pipeline-based structure:

1. **Update module declaration**: Use `export module cmd.<command>;`
2. **Add imports**: Add necessary imports (`import core;`, `import utils;`)
3. **Define options**: Use the `OPTION` macro to define options
4. **Create pipeline namespace**: Implement pipeline components
5. **Update command implementation**: Use `CommandContext` for option access
6. **Register command**: Use the new `REGISTER_COMMAND` macro

### Example Migration

#### After (Pipeline-Based Architecture)

```cpp
export module cmd.echo;

import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

namespace echo_constants {
    // Command-specific constants can be defined here
}

// Define options
export auto constexpr ECHO_OPTIONS =
    std::array{
        OPTION("-n", "--no-newline", "do not output the trailing newline"),
        OPTION("-e", "--escape", "enable interpretation of backslash escapes"),
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
                 "echo [SHORT-OPTION]... [STRING]...",

                 /* description */
                 "Display a line of text.",

                 /* examples */
                 "  echo Hello World      Display 'Hello World'\n"
                 "  echo -n Hello         Display 'Hello' without trailing newline\n"
                 "  echo -e Hello\\nWorld  Display 'Hello' and 'World' on separate lines",

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

    // Handle escape sequences if enabled
    if (escape) {
        // Implement escape sequence handling
    }

    // Output result
    std::cout << text;
    if (!no_newline) {
        std::cout << std::endl;
    }

    return 0;
}
```

## Reference Implementation

The `echo.cppm` file serves as the reference implementation for the new pipeline-based architecture. It demonstrates:

1. **Pipeline Components**: Separate validation and processing logic
2. **Option Handling**: Use of `CommandContext` for type-safe option access
3. **Error Handling**: Use of `core::pipeline::Result` for error reporting
4. **Command Registration**: Proper use of `REGISTER_COMMAND` macro

All new commands should follow this pattern for consistency and maintainability.
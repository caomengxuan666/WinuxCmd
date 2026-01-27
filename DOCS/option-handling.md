# Option Handling Guide

## Overview

This guide explains how to use the `core.opt` module for consistent option handling across all commands in the WinuxCmd project. The module provides a unified way to generate option handlers from option definitions, reducing code duplication and ensuring consistency.

## Core Concepts

### OptionHandler Structure

The `OptionHandler` structure represents a single option handler with the following fields:

```cpp
struct OptionHandler {
  char short_opt;         // Short option character (e.g., 'l' for -l)
  bool requires_arg = false; // Whether the option requires an argument
  const char* long_opt = nullptr; // Long option name (e.g., "--long-list")
};
```

### Key Functions

#### generate_option_handlers

Generates an `std::array<OptionHandler>` from an array of `OptionMeta` objects. This function has two overloads:

1. **Base case**: No argument-requiring options specified
   ```cpp
   template <size_t N>
   constexpr auto generate_option_handlers(const std::array<cmd::meta::OptionMeta, N>& options);
   ```

2. **With custom argument-requiring options**:
   ```cpp
   template <size_t N, typename... Args>
   constexpr auto generate_option_handlers(const std::array<cmd::meta::OptionMeta, N>& options, Args... args);
   ```

#### requires_argument

Checks if an option requires an argument. This function has two overloads:

1. **Base case**: Always returns false
   ```cpp
   constexpr bool requires_argument(std::string_view long_name);
   ```

2. **With custom argument-requiring options**:
   ```cpp
   template <typename... Args>
   constexpr bool requires_argument(std::string_view long_name, Args... args);
   ```

## Usage Example (ls command)

### 1. Define Options

First, define your command options using the `OPTION` macro:

```cpp
constexpr auto LS_OPTIONS = std::array{
    OPTION("-a", "--all", "do not ignore entries starting with ."),
    OPTION("-T", "--tabsize", "assume tab stops at each COLS instead of 8"),
    OPTION("-w", "--width", "set output width to COLS. 0 means no limit"),
    // ... other options
};
```

### 2. Generate Option Handlers

Generate `OPTION_HANDLERS` from your options array. Specify any options that require arguments:

```cpp
// Auto-generated lookup table for options from LS_OPTIONS
constexpr auto OPTION_HANDLERS = generate_option_handlers(LS_OPTIONS, "--tabsize", "--width");
```

### 3. Use Option Handlers

Use the generated `OPTION_HANDLERS` in your option parsing logic:

```cpp
// Example: Parsing long options
if (arg.starts_with("--")) {
  for (const auto& handler : OPTION_HANDLERS) {
    if (handler.long_opt && arg == handler.long_opt) {
      if (handler.requires_arg) {
        // Handle option with argument
      } else {
        // Handle boolean option
      }
      break;
    }
  }
}

// Example: Parsing short options
for (size_t j = 1; j < arg.size(); ++j) {
  char opt_char = arg[j];
  for (const auto& handler : OPTION_HANDLERS) {
    if (handler.short_opt == opt_char) {
      if (handler.requires_arg) {
        // Handle option with argument
      } else {
        // Handle boolean option
      }
      break;
    }
  }
}
```

## Migration Guide

To migrate an existing command to use the `core.opt` module:

1. **Add import statement**:
   ```cpp
   import core.opt;
   ```

2. **Remove duplicate code**:
   - Remove any local `OptionHandler` struct definitions
   - Remove any local option handler generation logic

3. **Generate option handlers**:
   - Replace manually defined `OPTION_HANDLERS` with a call to `generate_option_handlers`
   - Specify any options that require arguments

4. **Update option parsing**:
   - Use the generated `OPTION_HANDLERS` in your parsing logic
   - Ensure consistent handling of options with arguments

## Best Practices

- **Single source of truth**: Define options only once in the `OPTIONS` array
- **Explicit argument requirements**: Always specify which options require arguments when calling `generate_option_handlers`
- **Consistent naming**: Use the same option names across all commands when possible
- **Documentation**: Document all options with clear descriptions

## Troubleshooting

### Common Issues

1. **Option not found**: Ensure the option is defined in the `OPTIONS` array
2. **Argument handling** : Ensure options that require arguments are properly specified in the `generate_option_handlers` call
3. **Module imports**: Ensure `import core.opt;` is present in your module

### Debugging Tips

- Print the generated `OPTION_HANDLERS` array to verify it contains the expected handlers
- Check that short options are correctly extracted from the short_name field
- Ensure long options are correctly mapped to their handlers

## Example: Migrating ls.cppm

The `ls.cppm` file serves as the reference implementation for using the `core.opt` module. Key changes include:

1. **Added import**: `import core.opt;`
2. **Removed local definitions**: Removed duplicate `OptionHandler` struct and helper functions
3. **Generated handlers**: Used `generate_option_handlers(LS_OPTIONS, "--tabsize", "--width")`
4. **Updated parsing**: Used the generated `OPTION_HANDLERS` in the parsing logic

By following this pattern, all commands can benefit from consistent, maintainable option handling.

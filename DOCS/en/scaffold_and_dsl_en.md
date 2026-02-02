# Scaffold and DSL Design Documentation

## 1. Scaffold Tool

### 1.1 Overview

The scaffold tool is a command-line utility provided by the WinuxCmd project to quickly generate template code for new commands. It helps developers avoid writing repetitive boilerplate code, improves development efficiency, and ensures all commands follow the project's coding standards and architectural design.

### 1.2 Features

- **Automatic command template generation**: Generates complete command implementation templates based on user input
- **Follows project specifications**: Ensures generated code complies with Google C++ Style Guide and project coding standards
- **Includes necessary components**: Automatically generates command registration, parameter parsing, core logic, and other necessary components
- **Supports author information**: Generated code includes author information fields for contributor tracking

### 1.3 Usage

1. **Build the scaffold tool**:
   ```bash
   # Execute in project root directory
   cmake --build . --target scaffold
   ```

2. **Run the scaffold tool**:
   ```bash
   # Execute in build directory
   ./scaffold.exe <command name> <command description>
   ```

   For example, to create a command named `grep` with the description "search text", you can execute:
   ```bash
   ./scaffold.exe grep "search text"
   ```

3. **View generated files**:
   The scaffold will generate the corresponding command file in the `src/commands/` directory, such as `grep.cppm`.

### 1.4 Generated File Structure

Generated command files include the following main parts:

- **Module declaration**: Declares the command module using C++23 module system
- **Necessary imports**: Imports project core modules and standard library
- **Command option definitions**: Defines command-supported options using DSL
- **Command registration**: Registers the command using `REGISTER_COMMAND` macro
- **Core logic implementation**: Contains the main functionality implementation of the command
- **Author information**: Contains author name and email fields

## 2. DSL Design

### 2.1 Overview

DSL (Domain-Specific Language) is a declarative syntax designed by the WinuxCmd project to simplify command option definition and processing. It allows developers to define command options and parameters in a concise, intuitive way without writing tedious parsing code.

### 2.2 Design Principles

DSL design is based on the following principles:

- **Simplicity**: Uses concise syntax to define command options, reducing boilerplate code
- **Type safety**: Utilizes C++ type system to ensure correctness of option definitions
- **Extensibility**: Easy to add new option types and processing logic
- **Compile-time validation**: Validates correctness of option definitions at compile time, avoiding runtime errors

### 2.3 Core Components

#### 2.3.1 OPTION Macro

The `OPTION` macro is one of the core components of DSL, used to define command options. Its syntax is as follows:

```cpp
OPTION(short option, long option, description)
```

For example:

```cpp
OPTION("-l", "--long", "Use long format")
```

#### 2.3.2 REGISTER_COMMAND Macro

The `REGISTER_COMMAND` macro is used to register commands, associating command names, descriptions, option lists, and implementation functions. Its syntax is as follows:

```cpp
REGISTER_COMMAND(command name, command description, option list, command function, help information)
```

#### 2.3.3 Option Processing System

The option processing system is the runtime part of DSL, responsible for:

- Parsing command-line arguments
- Validating option validity
- Processing option parameters
- Generating help information

### 2.4 Usage Example

The following is an example of using DSL to define command options:

```cpp
// Define command options
constexpr auto LS_OPTIONS = std::array{
    OPTION("-l", "--long", "Use long format"),
    OPTION("-a", "--all", "Show all files, including hidden files"),
    OPTION("-h", "--human-readable", "Show file sizes in human-readable format"),
    OPTION("-r", "--reverse", "Reverse sort"),
};

// Register command
REGISTER_COMMAND("ls", "List directory contents", LS_OPTIONS, ls_func, "List directory contents")
{
    // Command implementation
    // ...
}
```

### 2.5 Advanced Features

#### 2.5.1 Flag Options

Flag options are options that do not require parameters, used to enable or disable certain features. In DSL, flag options are defined the same way as regular options, but no parameters are fetched during processing.

#### 2.5.2 Parameter Options

Parameter options are options that require parameters, such as `-f <file>`. DSL automatically handles parameter fetching and validation for parameter options.

#### 2.5.3 Help Information Generation

DSL automatically generates help information based on option definitions, including option descriptions, usage examples, etc.

## 3. Development Process

### 3.1 New Command Development Steps

1. **Generate command template using scaffold**
2. **Modify command option definitions**: Modify option definitions according to the actual needs of the command
3. **Implement core logic**: Implement the core functionality of the command in the generated template
4. **Test the command**: Compile and test the functionality of the command
5. **Update documentation**: Update the status and description in the command implementation plan document

### 3.2 Best Practices

- **Follow coding standards**: Ensure code complies with Google C++ Style Guide
- **Keep it simple**: Implement the core functionality of the command, avoid overly complex implementations
- **Test thoroughly**: Write unit tests for the command to ensure correct functionality
- **Complete documentation**: Update the command's help information and documentation

## 4. Common Issues

### 4.1 Generated Code Fails to Compile

**Possible causes**:
- Command name does not conform to naming conventions
- Command description contains special characters

**Solutions**:
- Use valid command names (only letters, numbers, and underscores)
- Ensure command descriptions do not contain special characters

### 4.2 Option Parsing Fails

**Possible causes**:
- Option definition is incorrect
- Command-line parameter format is wrong

**Solutions**:
- Check if option definitions comply with DSL syntax
- Ensure command-line parameter format is correct

### 4.3 Command Registration Fails

**Possible causes**:
- Command name already exists
- Option list format is incorrect

**Solutions**:
- Use a unique command name
- Check if option list is correctly defined

## 5. Summary

Scaffold and DSL are important tools for the WinuxCmd project, making command development simpler, more efficient, and more standardized. By using these tools, developers can focus on implementing the core functionality of commands without worrying about tedious boilerplate code and option parsing logic.

We hope this document helps developers understand and use these tools to contribute more high-quality command implementations to the WinuxCmd project.
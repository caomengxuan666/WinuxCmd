# Contributing to WinuxCmd

Thank you for considering contributing to WinuxCmd! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Code Style](#code-style)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)
- [Feature Requests](#feature-requests)
- [Documentation](#documentation)
- [Code of Conduct](#code-of-conduct)

## Getting Started

### Prerequisites

- Visual Studio 2026 or later (MSVC)
- CMake 3.21 or later
- Windows 10 or later
- Git

### Setting Up the Development Environment

1. **Clone the repository**
   ```bash
   git clone https://github.com/caomengxuan666/WinuxCmd.git
   cd WinuxCmd
   ```

2. **Configure CMake**
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2026" -A x64
   ```

3. **Build the project**
   ```bash
   cmake --build . --config Release
   ```

4. **Run tests**
   ```bash
   ctest -C Release
   ```

## Development Workflow

### Branching Strategy

- `main`: Main development branch
- Feature branches: For new features and bug fixes
- Release branches: For version releases

### Creating a New Command

1. **Use the scaffold tool**
   ```bash
   # Build the scaffold tool
   cmake --build . --target scaffold
   
   # Create a new command
   ./scaffold.exe mycommand
   ```

2. **Implement the command**
   - Edit the generated files in `src/commands/`
   - Follow the existing command patterns

3. **Add tests**
   - Create test files in `tests/unit/mycommand/`
   - Follow the existing test patterns

4. **Update documentation**
   - Add the command to `README.md` and `README-zh.md`
   - Update the command implementation documentation

## Code Style

### C++ Style Guide

WinuxCmd follows the Google C++ Style Guide with some modifications:

- **Indentation**: 2 spaces (no tabs)
- **Line Length**: 80 characters
- **Naming Conventions**:
  - Classes: `CamelCase` (e.g., `MyClass`)
  - Functions: `camelCase` (e.g., `myFunction`)
  - Variables: `camelCase` (e.g., `myVariable`)
  - Constants: `kCamelCase` (e.g., `kMyConstant`)
  - Macros: `UPPER_SNAKE_CASE` (e.g., `MY_MACRO`)
- **Braces**: K&R style (opening brace on same line, closing brace on new line)
- **Include Order**: Standard libraries first, then third-party libraries, then project headers

### Formatting

Use the provided format script to ensure consistent formatting:

```bash
python scripts/format.py
```

### Linting

The project uses clang-tidy for static analysis:

```bash
# Run clang-tidy
clang-tidy src/commands/*.cpp
```

## Testing

### Writing Tests

1. **Create test files** in `tests/unit/{command}/`
2. **Use the test framework** described in `DOCS/en/testing_framework_en.md`
3. **Follow best practices**:
   - Isolate tests
   - Use `TempDir` for temporary files
   - Test both success and failure scenarios
   - Add clear assertions

### Running Tests

```bash
# Run all tests
ctest -C Release

# Run specific test
ctest -C Release -R "ls.ls_basic"

# Run tests with verbose output
ctest -C Release -V
```

### Test Coverage

While formal test coverage metrics are not currently collected, contributors are encouraged to:

- Test all command options and flags
- Test edge cases and error conditions
- Ensure tests pass on both 32-bit and 64-bit systems

## Submitting Changes

### Pull Request Process

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/my-feature
   ```
3. **Make changes**
4. **Commit changes** with clear commit messages
   ```bash
   git commit -m "[Feature] Add my-feature command"
   ```
5. **Push changes** to your fork
   ```bash
   git push origin feature/my-feature
   ```
6. **Create a pull request**
   - Describe the changes in detail
   - Reference any related issues
   - Ensure all tests pass

### Commit Message Format

Follow this format for commit messages:

```
[Area] Brief description of changes

Detailed explanation of the changes (if needed).

Fixes #issue_number (if applicable)
```

Where `Area` can be:
- `Command`: For command implementations
- `Framework`: For test framework changes
- `Core`: For core functionality changes
- `Docs`: For documentation changes
- `Build`: For build system changes

## Reporting Issues

### Bug Reports

When reporting bugs, please include:

- **Steps to reproduce** the issue
- **Expected behavior**
- **Actual behavior**
- **Environment** (Windows version, Visual Studio version)
- **Error messages** (if any)
- **Screenshots** (if applicable)

### Security Issues

For security issues, please contact the maintainers directly at <2507560089@qq.com> instead of creating a public issue.

## Feature Requests

When requesting new features, please include:

- **Description** of the feature
- **Use case** for the feature
- **Examples** of how the feature would be used
- **References** to similar features in other tools (if applicable)

## Documentation

### Updating Documentation

- **README.md** and **README-zh.md**: Update for new commands and features
- **DOCS/en/**: English documentation
- **DOCS/zh/**: Chinese documentation

### Documentation Guidelines

- Use clear, concise language
- Provide examples where appropriate
- Follow the existing documentation structure
- Keep English and Chinese documentation in sync

## Code of Conduct

### Our Pledge

We as members, contributors, and leaders pledge to make participation in our community a harassment-free experience for everyone, regardless of age, body size, visible or invisible disability, ethnicity, sex characteristics, gender identity and expression, level of experience, education, socio-economic status, nationality, personal appearance, race, religion, or sexual identity and orientation.

### Our Standards

Examples of behavior that contributes to creating a positive environment include:

- Using welcoming and inclusive language
- Being respectful of differing viewpoints and experiences
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

Examples of unacceptable behavior by participants include:

- The use of sexualized language or imagery and unwelcome sexual attention or advances
- Trolling, insulting/derogatory comments, and personal or political attacks
- Public or private harassment
- Publishing others' private information, such as a physical or electronic address, without explicit permission
- Other conduct which could reasonably be considered inappropriate in a professional setting

### Enforcement Responsibilities

Community leaders are responsible for clarifying and enforcing our standards of acceptable behavior and will take appropriate and fair corrective action in response to any behavior that they deem inappropriate, threatening, offensive, or harmful.

### Scope

This Code of Conduct applies within all community spaces, and also applies when an individual is officially representing the community in public spaces.

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be reported by contacting the maintainers at <2507560089@qq.com>. All complaints will be reviewed and investigated and will result in a response that is deemed necessary and appropriate to the circumstances.

## Acknowledgements

We appreciate your contributions to WinuxCmd! Your help makes this project better for everyone.

## Contact

- Maintainer: <2507560089@qq.com>
- GitHub: [@caomengxuan666](https://github.com/caomengxuan666)
- Website: [blog.caomengxuan666.com](https://blog.caomengxuan666.com)

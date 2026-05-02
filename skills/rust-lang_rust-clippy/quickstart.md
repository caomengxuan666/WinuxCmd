# Quickstart

```markdown
# Quickstart


Clippy is a collection of lints for Rust that catches common mistakes and improves code quality. Here are the essential usage patterns:

## 1. Basic Usage

Run Clippy on your project with the default lint set:

```bash
cargo clippy
```

## 2. Fixing Issues Automatically

Let Clippy automatically apply suggestions:

```bash
cargo clippy --fix
```

## 3. Running on Specific Package

Target a specific crate in a workspace:

```bash
cargo clippy -p my-crate
```

## 4. Denying All Warnings in CI

Fail the build on any Clippy warning:

```bash
cargo clippy -- -D warnings
```

## 5. Running with All Targets and Features

Check tests, examples, and all features:

```bash
cargo clippy --all-targets --all-features -- -D warnings
```

## 6. Enabling Pedantic Lints

Get stricter linting (may have false positives):

```bash
cargo clippy -- -W clippy::pedantic
```

## 7. Selective Lint Control

Allow all lints except specific ones:

```bash
cargo clippy -- -A clippy::all -W clippy::useless_format -W clippy::single_match
```

## 8. Using in Code

Configure lints directly in your source:

```rust
// Deny all Clippy lints at crate level
#![deny(clippy::all)]

// Allow specific lint on a function
#[allow(clippy::single_match)]
fn my_function() {
    // ...
}
```

For more detailed configuration, see [overview.md](overview.md) and [best-practices.md](best-practices.md).
```
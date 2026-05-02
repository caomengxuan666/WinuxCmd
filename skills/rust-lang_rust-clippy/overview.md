# Overview

```markdown
# Overview


## What Clippy Does

Clippy is a static analysis tool for Rust that provides over 800 lints to catch common mistakes, enforce style conventions, and improve code performance. It extends the Rust compiler's built-in warnings with additional checks organized into categories:

- **correctness**: Code that is outright wrong or useless (deny by default)
- **suspicious**: Code likely wrong or useless (warn by default)
- **style**: Code that should be more idiomatic (warn by default)
- **complexity**: Simple things done in complex ways (warn by default)
- **perf**: Code that can run faster (warn by default)
- **pedantic**: Strict lints with occasional false positives (allow by default)
- **restriction**: Feature-restricting lints (allow by default)
- **nursery**: New lints under development (allow by default)
- **cargo**: Lints for Cargo manifest (allow by default)

## When to Use Clippy

**Use Clippy when:**
- You want to catch bugs before they reach production
- You're establishing or enforcing code style guidelines
- You're optimizing performance-critical code
- You're onboarding new developers to a Rust project
- You're running CI/CD pipelines

**Don't use Clippy when:**
- You're prototyping quickly and don't need strict checks
- You're working with legacy code that would require massive refactoring
- You're using `restriction` lints without careful consideration (see [pitfalls.md](pitfalls.md))

## Key Design Principles

1. **Non-invasive**: Clippy runs as a Cargo subcommand, not a compiler fork
2. **Configurable**: Lints can be allowed/warned/denied at crate, module, or function level
3. **Auto-fixable**: Many lints support automatic fixes via `--fix`
4. **Category-based**: Lints are grouped by severity and purpose
5. **Extensible**: New lints can be added via the nursery category

For common mistakes, see [pitfalls.md](pitfalls.md). For safety considerations, see [safety.md](safety.md).
```
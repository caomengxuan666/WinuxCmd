# Pitfalls

```markdown
# Pitfalls


## 1. Using `-D warnings` Instead of `-D clippy::all`

**BAD**: This fails on ALL warnings, including compiler warnings:
```bash
cargo clippy -- -D warnings
```

**GOOD**: This only fails on Clippy warnings:
```bash
cargo clippy -- -D clippy::all
```

## 2. Enabling All Restriction Lints

**BAD**: This will lint against perfectly reasonable code:
```rust
// This will cause issues
#![deny(clippy::restriction)]
```

**GOOD**: Enable restriction lints selectively:
```rust
// Only forbid specific restriction lints
#![forbid(clippy::unwrap_used)]
#![forbid(clippy::todo)]
```

## 3. Ignoring Pedantic Lint False Positives

**BAD**: Denying all pedantic lints without review:
```rust
#![deny(clippy::pedantic)]
// This might trigger false positives
```

**GOOD**: Allow pedantic lints and enable specific ones:
```rust
#![allow(clippy::pedantic)]
#![warn(clippy::cast_possible_truncation)]
```

## 4. Not Running on Tests

**BAD**: Only checking main code:
```bash
cargo clippy
```

**GOOD**: Check all targets including tests:
```bash
cargo clippy --all-targets
```

## 5. Using `--fix` Without Review

**BAD**: Applying fixes without checking:
```bash
cargo clippy --fix
# Might introduce subtle bugs
```

**GOOD**: Review changes after fixing:
```bash
cargo clippy --fix
git diff  # Review all changes
cargo test  # Verify nothing broke
```

## 6. Mixing Lint Levels Incorrectly

**BAD**: Conflicting lint levels:
```rust
#![deny(clippy::all)]
#[allow(clippy::all)]  // This overrides the deny!
fn my_fn() {}
```

**GOOD**: Consistent lint levels:
```rust
#![deny(clippy::all)]
#[allow(clippy::single_match)]  // Allow specific lint
fn my_fn() {}
```

## 7. Not Using `--no-deps` in Workspaces

**BAD**: Running on entire workspace including dependencies:
```bash
cargo clippy -p my-crate
# This also checks path dependencies
```

**GOOD**: Only check the target crate:
```bash
cargo clippy -p my-crate -- --no-deps
```

## 8. Ignoring Nursery Lints

**BAD**: Never checking new lints:
```bash
cargo clippy
# Missing potential improvements
```

**GOOD**: Periodically check nursery lints:
```bash
cargo clippy -- -W clippy::nursery
```

For more safety considerations, see [safety.md](safety.md).
```
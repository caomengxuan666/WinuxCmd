# Safety

```markdown
# Safety


## 1. NEVER Use `clippy-driver` as a Rust Compiler Replacement

**BAD**: Using `clippy-driver` for production builds:
```bash
clippy-driver --edition 2021 -Copt-level=3 main.rs
# Produces unoptimized artifacts!
```

**GOOD**: Use `clippy-driver` only for linting:
```bash
clippy-driver --edition 2021 main.rs
# Then compile with rustc for production
rustc --edition 2021 -Copt-level=3 main.rs
```

## 2. NEVER Enable All Restriction Lints Without Review

**BAD**: Blindly enabling all restriction lints:
```rust
// This will break your code
#![deny(clippy::restriction)]
```

**GOOD**: Selectively enable restriction lints:
```rust
// Only forbid specific patterns
#![forbid(clippy::unwrap_used)]
#![forbid(clippy::panic)]
```

## 3. NEVER Use `--fix` Without Version Control

**BAD**: Running auto-fix without backup:
```bash
cargo clippy --fix
# No way to revert if something breaks
```

**GOOD**: Always use version control:
```bash
git commit -am "before clippy fix"
cargo clippy --fix
git diff  # Review changes
cargo test  # Verify
```

## 4. NEVER Ignore `correctness` Lints

**BAD**: Allowing correctness lints:
```rust
#![allow(clippy::correctness)]
// This hides actual bugs!
```

**GOOD**: Always enforce correctness:
```rust
#![deny(clippy::correctness)]
// Catches real issues
```

## 5. NEVER Run Clippy Without Testing After Fixes

**BAD**: Assuming fixes are safe:
```bash
cargo clippy --fix
# Deploy without testing
```

**GOOD**: Always test after fixes:
```bash
cargo clippy --fix
cargo test
cargo build
```

For best practices, see [best-practices.md](best-practices.md).
```
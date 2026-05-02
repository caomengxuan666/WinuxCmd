# Best Practices

```markdown
# Best Practices: Production-Ready Clippy Usage

## 1. Establish a Baseline Configuration

Create a consistent lint policy for your project:

```rust
// In lib.rs or main.rs
#![deny(clippy::all)]
#![deny(clippy::correctness)]
#![warn(clippy::pedantic)]
#![allow(clippy::cast_possible_truncation)]  // Known false positive
```

## 2. Use CI Integration Properly

Set up CI to enforce Clippy standards:

```yaml
# .github/workflows/ci.yml
- name: Run Clippy
  run: |
    cargo clippy --all-targets --all-features -- \
      -D clippy::all \
      -D clippy::correctness \
      -W clippy::pedantic
```

## 3. Gradual Adoption Strategy

When adopting Clippy on existing code:

```rust
// Step 1: Start with warnings only
#![warn(clippy::all)]

// Step 2: After fixing, move to deny
#![deny(clippy::all)]

// Step 3: Add stricter lints
#![deny(clippy::pedantic)]
```

## 4. Use Configuration Files

Create a `.clippy.toml` for project-wide settings:

```toml
# .clippy.toml
cognitive-complexity-threshold = 15
too-many-arguments-threshold = 5
```

## 5. Selective Lint Suppression

Suppress lints only where necessary:

```rust
#[allow(clippy::too_many_arguments)]
fn complex_function(
    arg1: i32,
    arg2: i32,
    arg3: i32,
    arg4: i32,
    arg5: i32,
    arg6: i32,
) {
    // This is intentionally complex
}
```

## 6. Regular Nursery Checks

Periodically review new lints:

```bash
# Monthly check for new lints
cargo clippy -- -W clippy::nursery
```

## 7. Document Lint Decisions

Explain why certain lints are suppressed:

```rust
// Reason: This function signature is required by the API
#[allow(clippy::too_many_arguments)]
pub fn api_function(a: i32, b: i32, c: i32, d: i32, e: i32) {}
```

For lifecycle management, see [lifecycle.md](lifecycle.md).
```
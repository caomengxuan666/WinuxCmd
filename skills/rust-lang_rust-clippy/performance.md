# Performance

```markdown
# Performance


## 1. Incremental Analysis

Clippy benefits from Rust's incremental compilation:

```bash
# First run is slower, subsequent runs are faster
cargo clippy
cargo clippy  # Faster due to caching
```

## 2. Selective Target Analysis

Limit analysis to relevant targets:

```bash
# Slow: Checks everything
cargo clippy --all-targets

# Fast: Check only library code
cargo clippy --lib

# Fast: Check only tests
cargo clippy --tests
```

## 3. Dependency Exclusion

Skip dependency analysis for faster runs:

```bash
# Slow: Checks all dependencies
cargo clippy

# Fast: Skip dependency linting
cargo clippy -- --no-deps
```

## 4. Lint Group Performance Impact

Some lint groups are more expensive:

```bash
# Fast: Default lints
cargo clippy

# Slower: Pedantic lints
cargo clippy -- -W clippy::pedantic

# Slowest: All lints
cargo clippy -- -W clippy::all -W clippy::pedantic -W clippy::nursery
```

## 5. Parallel Execution

Leverage parallel compilation:

```bash
# Use multiple cores
CARGO_BUILD_JOBS=8 cargo clippy

# Or set in config
# [build]
# jobs = 8
```

## 6. Cache Optimization

Optimize Clippy's cache usage:

```bash
# Clear cache if it becomes stale
cargo clean
cargo clippy

# Use sccache for distributed caching
RUSTC_WRAPPER=sccache cargo clippy
```

## 7. Focused Analysis

Run specific lints for targeted analysis:

```bash
# Fast: Check only performance lints
cargo clippy -- -W clippy::perf

# Fast: Check only one lint
cargo clippy -- -W clippy::single_match
```

## 8. CI Performance Optimization

Optimize Clippy in CI:

```yaml
# .github/workflows/ci.yml
- name: Clippy (fast path)
  run: |
    # Only check changed files
    cargo clippy --lib -- -D clippy::all
```

For threading considerations, see [threading.md](threading.md).
```
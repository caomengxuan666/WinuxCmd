# Threading

```markdown
# Threading


## 1. Single-Threaded Operation

Clippy runs as a single-threaded analysis tool:

```bash
# Clippy itself is single-threaded
cargo clippy

# But cargo may parallelize compilation
CARGO_BUILD_JOBS=4 cargo clippy
```

## 2. Thread-Safe Configuration

Configuration files are read once and are thread-safe:

```toml
# .clippy.toml - Safe to share across threads
cognitive-complexity-threshold = 15
too-many-arguments-threshold = 5
```

## 3. Concurrent Project Analysis

Multiple Clippy instances can run on different projects:

```bash
# Safe: Different projects
cd project_a && cargo clippy &
cd project_b && cargo clippy &

# Wait for both
wait
```

## 4. Shared Cache Considerations

When using shared caches across threads:

```bash
# Safe with sccache
RUSTC_WRAPPER=sccache cargo clippy

# But avoid concurrent writes to same cache
# Use separate cache directories for CI
```

## 5. CI Parallelization

Parallelize Clippy in CI workflows:

```yaml
# .github/workflows/ci.yml
jobs:
  clippy:
    strategy:
      matrix:
        target: [--lib, --tests, --examples]
    steps:
      - run: cargo clippy ${{ matrix.target }} -- -D clippy::all
```

## 6. Thread Safety of Lint Results

Lint results are deterministic and thread-safe:

```rust
// Clippy produces the same results regardless of thread
// Safe to run in parallel CI jobs
fn main() {
    // This code will get the same lint results
    // whether analyzed on thread 1 or thread 2
    let x = 5;
    let y = x + 1;  // Clippy might suggest using x.saturating_add(1)
}
```

## 7. Avoiding Race Conditions

Clippy itself doesn't have race conditions, but your code might:

```rust
// BAD: Clippy won't catch all race conditions
use std::sync::Mutex;
static DATA: Mutex<i32> = Mutex::new(0);

fn update() {
    let mut data = DATA.lock().unwrap();
    *data += 1;  // Clippy might suggest using atomic operations
}

// GOOD: Use thread-safe patterns
use std::sync::atomic::{AtomicI32, Ordering};
static ATOMIC_DATA: AtomicI32 = AtomicI32::new(0);

fn atomic_update() {
    ATOMIC_DATA.fetch_add(1, Ordering::SeqCst);
}
```

## 8. Parallel Test Execution

Run Clippy on tests in parallel:

```bash
# Safe: Tests are independent
cargo clippy --tests

# Parallel test execution
cargo test -- --test-threads=4
```

For lifecycle management, see [lifecycle.md](lifecycle.md).
```
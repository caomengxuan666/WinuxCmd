# Overview

```markdown
# Overview of smol

## What is smol?

smol is a small, fast, and ergonomic async runtime for Rust. It provides everything you need to write asynchronous code without the complexity of larger runtimes like tokio. smol is built on a modular architecture, re-exporting functionality from smaller, focused crates:

- **async-executor** - Task execution and scheduling
- **async-net** - TCP/UDP/Unix networking
- **async-fs** - Filesystem operations
- **async-io** - I/O adapters and timers
- **async-channel** - Multi-producer multi-consumer channels
- **async-lock** - Mutex, RwLock, Semaphore, Barrier
- **async-process** - Process management
- **blocking** - Thread pool for blocking operations

## When to Use smol

Use smol when you need:
- A lightweight async runtime for small to medium applications
- Simple, readable async code without complex macros
- Fast compilation times (significantly faster than tokio)
- Minimal dependencies and binary size
- Compatibility with tokio-based libraries via `async-compat`

## When NOT to Use smol

Consider alternatives when you need:
- **Massive-scale production systems** - tokio has more extensive ecosystem and battle-testing
- **Specialized I/O patterns** - tokio's reactor provides more control over event handling
- **Complex task scheduling** - tokio's work-stealing scheduler may be more efficient for certain workloads
- **Existing tokio infrastructure** - migrating large tokio codebases may not be worth the effort

## Key Design Principles

1. **Composability**: smol's subcrates can be used independently or together
2. **Simplicity**: Minimal API surface with clear, predictable behavior
3. **Performance**: Efficient single-threaded executor with work-stealing
4. **Ergonomics**: No proc-macros needed for basic usage
5. **Compatibility**: Works with standard library types via `Unblock`

## Architecture

```
┌─────────────────────────────────────────────┐
│                   smol                       │
├─────────────────────────────────────────────┤
│  async-net  │  async-fs  │  async-process   │
├─────────────┴────────────┴──────────────────┤
│              async-executor                  │
├─────────────────────────────────────────────┤
│  async-io  │  async-channel  │  async-lock  │
├────────────┴────────────────┴───────────────┤
│              blocking (thread pool)          │
└─────────────────────────────────────────────┘
```

## Comparison with tokio

| Feature | smol | tokio |
|---------|------|-------|
| Runtime size | ~50KB | ~500KB+ |
| Compile time | Fast | Slow |
| Proc-macros | Optional | Required for `#[tokio::main]` |
| Work-stealing | Yes | Yes |
| I/O drivers | epoll/kqueue/iocp | epoll/kqueue/iocp |
| Ecosystem | Smaller | Larger |
| Learning curve | Gentle | Steeper |
```
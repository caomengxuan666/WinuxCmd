# Overview

libuv is a multi-platform C library focused on asynchronous I/O, originally developed for Node.js. It provides a full-featured event loop backed by epoll (Linux), kqueue (macOS/BSD), IOCP (Windows), and event ports (Solaris). The library handles asynchronous TCP/UDP sockets, DNS resolution, file system operations, file system events, TTY control, IPC with socket sharing, child processes, thread pools, signal handling, and high-resolution clocks.

**When to use libuv:**
- Building high-performance network servers or clients
- Implementing event-driven architectures
- Cross-platform asynchronous I/O needs
- Embedding in applications that require non-blocking operations
- Creating tools that need file system monitoring or child process management

**When NOT to use libuv:**
- Simple synchronous I/O tasks (use standard C library instead)
- Projects requiring C++ abstractions (consider Boost.Asio or similar)
- Applications needing HTTP protocol handling (use a higher-level library)
- Real-time systems with strict latency requirements (libuv's event loop adds overhead)

**Key design principles:**
- Single-threaded event loop with callback-based asynchronous operations
- Handle-based resource management (all handles must be closed with `uv_close`)
- Request objects for operations (like `uv_fs_t`, `uv_getaddrinfo_t`)
- Default loop available via `uv_default_loop()` for simple cases
- All callbacks are invoked from the event loop thread
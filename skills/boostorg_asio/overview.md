# Overview

Boost.Asio is a cross-platform C++ library for network and low-level I/O programming. It provides a consistent asynchronous model using a proactor design pattern, where I/O operations are initiated and their completion is signaled through completion handlers (callbacks). The library supports TCP, UDP, serial ports, timers, and custom I/O objects.

**When to use:**
- Building high-performance network servers/clients
- Implementing asynchronous I/O operations
- Creating real-time systems with timers
- Developing cross-platform networking code
- Working with coroutines (C++20) for cleaner async code

**When not to use:**
- Simple synchronous networking (raw sockets may suffice)
- Projects requiring zero-copy networking (consider io_uring directly)
- Environments without Boost installation (consider standalone Asio)

**Key design concepts:**
- `io_context`: The core I/O event loop that dispatches completion handlers
- `strand`: Provides serialized handler execution for thread safety
- `buffer`: Type-safe wrapper for memory regions in I/O operations
- `error_code`: Lightweight error handling instead of exceptions
- `use_awaitable`: Coroutine integration for modern C++

The library's design separates I/O objects (sockets, timers) from the execution context (io_context), allowing flexible threading models. Handlers are never executed concurrently within a strand, simplifying synchronization.
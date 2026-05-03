# Overview

cpp-httplib is a C++11 single-file header-only HTTP/HTTPS library that provides both server and client functionality. It uses blocking socket I/O and supports only HTTP/1.1. The library is extremely easy to set up - just include the `httplib.h` file in your project.

**When to use:**
- Simple HTTP/HTTPS server or client needs
- Projects requiring SSL/TLS support with minimal setup
- Single-threaded or simple multi-threaded applications
- Rapid prototyping and small to medium-scale services

**When NOT to use:**
- Applications requiring non-blocking I/O
- HTTP/2 or HTTP/3 support needed
- High-performance, large-scale production systems
- 32-bit platforms (not supported, security risks)
- Applications requiring WebSocket support for many concurrent connections

**Key Design:**
- Single-file header-only implementation
- Blocking socket I/O model
- Thread-per-connection for WebSocket connections
- SSL/TLS support via OpenSSL, MbedTLS, or wolfSSL
- Regular expression path matching
- Built-in multipart form data support
- Server-Sent Events (SSE) support

> 💡 See [quickstart.md](quickstart.md) for copy-paste examples.
> See [best-practices.md](best-practices.md) for production patterns.
> See [performance.md](performance.md) for optimization tips.
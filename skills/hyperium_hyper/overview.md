# Overview

hyperium/hyper is a high-performance HTTP client and server library for C++ that provides both synchronous and asynchronous APIs. It is designed for building network applications that require efficient HTTP communication, supporting HTTP/1.1 and HTTP/2 protocols with optional TLS encryption.

Use hyper when you need:
- High-throughput HTTP client/server applications
- Streaming request/response bodies
- Async I/O with callback or future-based patterns
- Custom TLS configuration for secure connections

Do NOT use hyper when:
- You need only simple HTTP requests (consider libcurl instead)
- Your application runs on embedded systems with limited memory
- You require WebSocket support (hyper focuses on HTTP only)

Key design principles:
- Connection pooling for efficient reuse
- Zero-copy body streaming where possible
- Configurable timeouts and retry policies
- Thread-safe client instances for concurrent use
- Builder pattern for request construction
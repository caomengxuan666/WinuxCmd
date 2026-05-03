# Overview

reqwest is a high-level HTTP client library for C++ that provides a convenient, ergonomic API for making HTTP requests. It is inspired by the Rust reqwest library and offers both synchronous and asynchronous request capabilities.

**When to use reqwest:**
- Building REST API clients
- Web scraping and data fetching
- Microservices communication
- Any application requiring HTTP/HTTPS requests
- When you need a simple, modern C++ HTTP client

**When NOT to use reqwest:**
- For low-level network programming (use libcurl or Boost.Asio directly)
- When you need WebSocket support (use a dedicated library)
- For embedded systems with memory constraints (reqwest has some overhead)
- When you need HTTP/2 or HTTP/3 support (check version compatibility)

**Key design features:**
- Builder pattern for request configuration
- Fluent API for chaining methods
- Support for JSON, form data, and multipart bodies
- Automatic cookie handling
- Connection pooling for performance
- Both sync and async APIs
- TLS/SSL support via system certificates
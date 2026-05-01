# Overview

WebSocket++ is a header-only C++ library that implements the WebSocket protocol (RFC 6455). It provides both client and server implementations with support for TLS/SSL, configurable transports (Asio-based), and extensible logging. The library is designed to be lightweight, portable, and easy to integrate into existing C++ projects.

**When to use WebSocket++:**
- Building real-time applications requiring bidirectional communication
- Implementing WebSocket servers or clients in C++
- Projects needing WebSocket support without heavy dependencies
- Applications requiring fine-grained control over WebSocket connections
- Systems that need to handle thousands of concurrent WebSocket connections

**When NOT to use WebSocket++:**
- When you need HTTP-only functionality (use a dedicated HTTP library)
- For simple REST APIs (use HTTP/1.1 or HTTP/2)
- When you need built-in pub/sub or message queuing (use a message broker)
- For projects that require WebRTC or other real-time protocols (use specialized libraries)

**Key Design Features:**
- Header-only: No compilation needed, just include headers
- Configurable: Template-based configuration system for custom transports
- Asio-based: Uses Boost.Asio or standalone Asio for async I/O
- Extensible: Custom logging, transport, and endpoint policies
- Connection lifecycle: Clear open/close/fail/message handlers
- Subprotocol support: Built-in WebSocket subprotocol negotiation
- TLS/SSL: Support for encrypted connections via OpenSSL

**Core Components:**
- `endpoint`: Server or client that manages connections
- `connection`: Individual WebSocket connection
- `message`: Represents incoming/outgoing WebSocket frames
- `config`: Template parameter defining transport, logging, and TLS settings
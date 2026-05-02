# Overview

`aiohttp` is an asynchronous HTTP client/server framework for Python built on top of `asyncio`. It supports both client and server sides of the HTTP protocol, including WebSockets, with a clean async/await interface. The library provides pluggable routing, middleware support, and efficient connection pooling.

Use `aiohttp` when you need high-concurrency HTTP operations, such as building web scrapers, API clients, microservices, or real-time applications with WebSockets. Avoid it for simple synchronous scripts where `requests` would suffice, or when you don't need async I/O. The library is designed for Python 3.7+ and requires understanding of `asyncio` concepts.

Key design principles include: context managers for resource cleanup (`async with`), session-based connection reuse, streaming support for large payloads, and a middleware architecture for server applications. The library uses `yarl` for URL handling and `multidict` for HTTP headers, providing immutable and mutable variants.
# Overview

cppzmq is a lightweight, header-only C++ binding for the ZeroMQ (libzmq) messaging library. It provides type-safe, exception-based wrappers around the C API, with RAII-style resource management. The library consists of two main headers: `zmq.hpp` for core functionality and `zmq_addon.hpp` for higher-level abstractions like multipart message handling.

**When to use cppzmq:**
- Building distributed systems that need reliable messaging patterns (pub/sub, request/reply, push/pull)
- Applications requiring high-throughput, low-latency message passing
- Projects that need to communicate across threads, processes, or networks
- Systems that benefit from ZeroMQ's brokerless architecture

**When NOT to use cppzmq:**
- For simple point-to-point communication where raw sockets suffice
- When you need guaranteed message ordering across multiple hops (ZeroMQ provides best-effort ordering)
- For very large messages (>1GB) without careful memory management
- On platforms without C++11 support

**Key design principles:**
- Header-only: just `#include <zmq.hpp>` to use
- RAII: context and socket objects automatically clean up resources
- Type safety: uses C++ types instead of void pointers
- Exception-based: throws `zmq::error_t` on failures instead of returning error codes
- ZeroMQ socket type system: `zmq::socket_type::push`, `zmq::socket_type::pull`, etc.
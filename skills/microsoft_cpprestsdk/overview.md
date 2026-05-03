# Overview

The C++ REST SDK (cpprestsdk) is a Microsoft project for cloud-based client-server communication in native code using a modern asynchronous C++ API design. It provides HTTP client/server capabilities, JSON parsing, URI handling, asynchronous streams, WebSocket client support, and OAuth functionality. The library is built on PPL Tasks, a powerful model for composing asynchronous operations based on C++11 features.

**When to use:**
- Building native C++ applications that need to communicate with REST APIs
- Creating HTTP servers in C++ for lightweight services
- Working with WebSocket connections in C++ applications
- Parsing and generating JSON data in C++

**When NOT to use:**
- The library is in maintenance-only mode and not recommended for new projects
- For new projects, consider alternatives like Boost.Beast, libcurl with modern C++ wrappers, or platform-specific networking APIs
- Only use for maintaining existing codebases or when migration is not feasible

**Key design:**
- Asynchronous operations using PPL tasks (pplx::task<T>)
- URI and HTTP abstractions that work across platforms
- JSON value model for parsing and constructing JSON
- Stream-based I/O for efficient data handling
- Cross-platform support: Windows, Linux, macOS, iOS, Android
# Overview

`gorilla/mux` is a powerful HTTP request router and dispatcher for Go that extends the standard `http.ServeMux` with advanced routing capabilities. It implements the `http.Handler` interface, making it fully compatible with Go's standard HTTP library.

**When to use:**
- Building RESTful APIs with complex routing requirements
- Applications needing URL parameters and pattern matching
- Projects requiring subdomain or host-based routing
- When you need middleware support and route grouping
- Applications that benefit from URL building (reverse routing)

**When NOT to use:**
- Simple applications with only a few static routes (use standard `http.ServeMux`)
- Microservices with minimal routing needs
- Projects already using a different router (like `chi` or `gin`)
- When you need extreme performance (consider `fasthttp`-based routers)

**Key design features:**
- Route matching based on URL host, path, path prefix, schemes, headers, query values, and HTTP methods
- Support for path variables with optional regular expressions
- Subrouter support for route grouping and namespace management
- Middleware chain support via `Use()` method
- URL building from route names
- Custom matcher functions for complex routing logic
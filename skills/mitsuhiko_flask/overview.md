# Overview

```markdown
# Overview


Flask is a lightweight WSGI web application framework for Python. It provides the essential tools for building web applications without imposing a rigid structure, making it ideal for both small prototypes and large, complex projects.

## What Flask Does

Flask handles HTTP request/response cycles, URL routing, template rendering (via Jinja2), session management, and static file serving. It wraps Werkzeug for WSGI handling and Jinja2 for templates, providing a cohesive API on top of these libraries.

## When to Use Flask

- **Small to medium web applications** where you want simplicity and flexibility
- **RESTful APIs** that don't need a full framework like Django
- **Microservices** where each service is small and focused
- **Prototypes and MVPs** that need to be built quickly
- **Applications with custom requirements** where Django's "batteries-included" approach is too restrictive

## When NOT to Use Flask

- **Large monolithic applications** that benefit from Django's built-in admin, ORM, and authentication
- **Real-time applications** requiring WebSockets (though extensions exist)
- **Projects requiring strict architectural patterns** (Flask is intentionally unopinionated)
- **Applications needing built-in database migrations, admin panels, or authentication** (you must add these via extensions)

## Key Design Principles

Flask follows these core principles:

1. **Minimal core**: The framework itself is small; functionality is added via extensions
2. **Explicit over implicit**: Configuration and behavior are explicit rather than hidden
3. **Flexible**: No enforced project structure or dependencies
4. **Thread-local context**: Request, session, and g objects are thread-local, making them safe in multi-threaded WSGI servers

## Architecture Overview

```
Request → WSGI Server → Flask App → Route Matching → View Function → Response
                              ↓
                        Middleware/Extensions
                              ↓
                        Jinja2 Templates (optional)
```

The application object (`Flask` instance) is the central coordinator. Routes are registered via decorators, and the WSGI server passes requests through the full middleware stack before reaching your view functions.
```
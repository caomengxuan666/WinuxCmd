# Overview

Nana is a C++ GUI library designed to provide a modern, cross-platform way to create graphical user interfaces. It follows C++ standard-like conventions and uses modern C++ features like lambdas, templates, and RAII. The library is regularly tested on Linux (X11) and Windows, with experimental support for macOS and FreeBSD.

**When to use Nana:**
- Building cross-platform desktop applications with a single codebase
- Projects that prefer C++-style APIs over C-style GUI frameworks
- Applications needing basic to intermediate GUI widgets (buttons, labels, listboxes, menus)
- When you want header-only style usage with minimal external dependencies

**When not to use Nana:**
- For mobile or web applications (desktop-only)
- When you need advanced UI features like 3D rendering or complex animations
- For production systems requiring extensive documentation and community support
- When you need strict DPI scaling across multiple monitors (known issues exist)

**Key design principles:**
- Event-driven architecture using lambdas and function objects
- Widget hierarchy with forms as top-level containers
- Layout management through the `place` class
- Drawing capabilities via `paint::graphics`
- Timer support for periodic operations
- File dialog and message box utilities
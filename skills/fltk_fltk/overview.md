# Overview

FLTK (Fast Light Tool Kit) is a cross-platform C++ GUI toolkit that provides modern GUI functionality without bloat. It supports UNIX/Linux (X11 or Wayland), Microsoft Windows, and macOS, with built-in OpenGL support and GLUT emulation for 3D graphics. The library is designed to be lightweight, fast, and easy to use, making it ideal for applications where resource efficiency is critical.

**When to use FLTK:**
- Building lightweight GUI applications that need to run on multiple platforms
- Creating tools and utilities where a minimal footprint is important
- Developing applications that require OpenGL integration
- Projects where you want to avoid heavy dependencies like Qt or GTK
- Embedded systems or resource-constrained environments

**When not to use FLTK:**
- Complex desktop applications requiring advanced widget sets
- Applications needing native look-and-feel on all platforms (FLTK has its own theme)
- Projects requiring extensive accessibility features
- Applications that need complex layout managers (FLTK uses absolute positioning)

**Key design principles:**
- Event-driven architecture with callbacks
- Widget hierarchy with parent-child relationships
- Manual memory management (no smart pointers)
- Single-threaded event loop by default
- Platform abstraction layer for portability
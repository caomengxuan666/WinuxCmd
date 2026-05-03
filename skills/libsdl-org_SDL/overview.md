# Overview

SDL (Simple DirectMedia Layer) is a cross-platform library that provides low-level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL, Vulkan, Direct3D, and Metal. It's designed for building multimedia applications like games, emulators, and media players.

**When to use SDL:**
- You need cross-platform window creation and input handling
- You want hardware-accelerated 2D rendering or OpenGL/Vulkan context management
- You need cross-platform audio output and input
- You're building a game engine, emulator, or media application

**When NOT to use SDL:**
- You only need a GUI toolkit (use Qt, wxWidgets, or GTK)
- You need complex UI widgets (SDL is low-level; use Dear ImGui on top)
- You're building a web application (use WebGL/Canvas directly)
- You need high-level game engine features (use Unity, Unreal, or Godot)

**Key design principles:**
- Single-threaded by default (most operations are not thread-safe)
- Explicit initialization/cleanup (SDL_Init/SDL_Quit)
- Event-driven architecture (SDL_PollEvent, SDL_WaitEvent)
- Resource ownership through opaque pointers (SDL_Window*, SDL_Texture*)
- Error reporting via SDL_GetError() and SDL_SetError()
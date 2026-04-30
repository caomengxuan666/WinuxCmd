# Overview

Dear ImGui is an **immediate-mode graphical user interface library** for C++. Unlike traditional retained-mode UI frameworks, it doesn't store UI state between frames. Instead, you describe your UI every frame using function calls. This makes it ideal for **debugging tools, in-game overlays, content creation tools, and real-time data visualization**.

**When to use:**
- Building developer tools and debug UIs
- Creating in-game editors and inspectors
- Rapid prototyping of UI concepts
- Tools that need to reflect dynamic data sets

**When NOT to use:**
- End-user facing applications with complex layouts
- Accessibility-required applications (no screen reader support)
- Internationalized applications needing RTL text
- Applications requiring native OS widget integration

**Key design principles:**
- **No state synchronization**: You don't maintain UI state separately
- **Frame-by-frame construction**: UI is rebuilt every frame
- **Minimal dependencies**: Self-contained, no external libraries needed
- **Render-agnostic**: Outputs vertex buffers for any 3D pipeline
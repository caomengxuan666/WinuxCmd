# Overview

```cpp
// Overview of martinmoene/filesystem
// A portable C++17 filesystem library implementation for pre-C++17 compilers

#include "filesystem.hpp"
#include <iostream>

namespace fs = ghc::filesystem;

/*
WHAT IT IS:
- A standalone implementation of the C++17 std::filesystem library
- Works with C++11, C++14, and C++17 compilers
- Provides cross-platform file system operations
- Header-only library (single include file)

WHEN TO USE:
- When targeting pre-C++17 compilers but need filesystem functionality
- When you need consistent behavior across Windows, Linux, and macOS
- When you want to avoid platform-specific APIs like _stat() or stat()
- When you need a lightweight, dependency-free filesystem library

WHEN NOT TO USE:
- When using C++17 or later (use std::filesystem instead)
- When you need advanced filesystem features not in C++17 standard
- When performance is critical and you need platform-specific optimizations

KEY DESIGN:
- Namespace: ghc::filesystem (can be aliased to fs)
- Path class for cross-platform path manipulation
- Directory iteration with directory_iterator and recursive_directory_iterator
- File operations: copy, move, create, remove, rename
- File information: size, permissions, timestamps
- Error handling via std::error_code or exceptions
*/

// Example: Checking library version
void version_check() {
    std::cout << "Filesystem version: " 
              << GHC_FILESYSTEM_VERSION << "\n";
}

// Example: Cross-platform path handling
void cross_platform_path() {
    fs::path p = "dir1/dir2/file.txt";  // Works on all platforms
    p = p.make_preferred();  // Convert to platform-specific separators
    std::cout << "Path: " << p << "\n";
}
```
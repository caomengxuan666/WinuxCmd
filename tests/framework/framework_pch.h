/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: framework_pch.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#ifndef FRAMEWORK_PCH_H
#define FRAMEWORK_PCH_H

/**
 * @brief Precompiled header for the WinuxCmd test framework
 *
 * This header includes commonly used system headers and standard library
 * headers to improve compilation performance across the test framework.
 * It also defines Windows-specific macros to reduce header bloat.
 */

// Windows macro definitions to reduce header size and avoid conflicts
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#define NOMINMAX             // Prevent Windows.h from defining min/max macros
#define VC_EXTRALEAN         // Exclude rarely-used MFC stuff

// Windows system headers
#include <Windows.h>

// Standard library headers
#include <chrono>      // For Time utilities
#include <filesystem>  // For File system operations
#include <fstream>     // For File stream operations
#include <iostream>    // For Standard I/O streams
#include <map>         // For Map container
#include <memory>      // For Smart pointers
#include <optional>    // For Optional values
#include <sstream>     // For String streams
#include <stdexcept>   // For Standard exceptions
#include <string>      // For String utilities
#include <thread>      /// For Threading
#include <vector>      // For Dynamic arrays

#endif  // FRAMEWORK_PCH_H

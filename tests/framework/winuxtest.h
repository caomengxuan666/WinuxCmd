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
 *  - File: winuxtest.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#ifndef WINUX_TEST_H
#define WINUX_TEST_H

/**
 * @brief Main include file for WinuxCmd test framework
 *
 * Convenience header that includes all essential test framework components.
 * Provides a single point of inclusion for test files.
 *
 * Components included:
 * - framework_pch.h: Precompiled header with common includes
 * - paths.h: Path utility functions for test environment
 * - pipeline.h: Command pipeline execution utilities
 * - process_win32.h: Low-level process execution functions
 * - temp_dir.h: Temporary directory management
 * - wctest.h: Core testing framework functionality
 */

#include "framework/framework_pch.h"  // Precompiled header
#include "framework/paths.h"          // Path utilities
#include "framework/pipeline.h"       // Pipeline execution
#include "framework/process_win32.h"  // Process management
#include "framework/temp_dir.h"       // Temporary directories
#include "framework/wctest.h"         // Core test framework

#endif  //! WINUX_TEST_H

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
 *  - File: paths.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#pragma once

#include <filesystem>
#include <string>

/**
 * @brief Get the directory containing the current executable
 *
 * Retrieves the full path to the directory where the current executable
 * is located. This is useful for locating test binaries and resources.
 *
 * @return std::filesystem::path Path to the executable's directory
 * @throws std::runtime_error if unable to retrieve the module file name
 */
std::filesystem::path get_current_exe_dir();

/**
 * @brief Utility class for managing project paths in tests
 *
 * Provides static methods to locate binary directories and executables
 * used in testing. Handles both configured paths and automatic detection.
 */
struct ProjectPaths {
  /**
   * @brief Detect the binary directory for test executables
   *
   * Attempts to determine the directory containing test binaries.
   * First checks for compile-time definition, falls back to
   * parent directory of current executable.
   *
   * @return std::filesystem::path Path to the binary directory
   */
  static std::filesystem::path detect_bin_dir() {
#ifdef WINUXCMD_BIN_DIR
    // Use compile-time defined binary directory
    return std::filesystem::path(WINUXCMD_BIN_DIR);
#else
    // Fallback to parent directory of current executable
    return get_current_exe_dir().parent_path();
#endif
  }

  /**
   * @brief Construct full path to an executable
   *
   * Combines the detected binary directory with the given executable name
   * to create a complete path.
   *
   * @param name The name of the executable (without extension on Windows)
   * @return std::filesystem::path Full path to the executable
   */
  static std::filesystem::path exe(const std::wstring& name) {
    return detect_bin_dir() / name;
  }
};

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
 *  - File: paths_win32.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include <windows.h>

#include <stdexcept>

#include "framework/framework_pch.h"
#include "framework/paths.h"

/**
 * @brief Implementation of get_current_exe_dir for Windows platform
 *
 * Uses Windows API GetModuleFileNameW to retrieve the full path of
 * the current executable, then extracts just the directory portion.
 *
 * @return std::filesystem::path Directory containing the current executable
 * @throws std::runtime_error if GetModuleFileNameW fails or buffer is too small
 */
std::filesystem::path get_current_exe_dir() {
  wchar_t buf[MAX_PATH];

  // Retrieve the full path of the current executable
  DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);

  // Check for errors: function failed or buffer was too small
  if (len == 0 || len == MAX_PATH) {
    throw std::runtime_error("GetModuleFileNameW failed");
  }

  // Convert to filesystem path and return parent directory
  return std::filesystem::path(buf).parent_path();
}

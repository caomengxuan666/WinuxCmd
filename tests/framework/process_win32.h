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
 *  - File: process_win32.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#pragma once
#include <string>
#include <vector>

/**
 * @brief Structure containing results from command execution
 *
 * Holds the exit code and captured stdout/stderr output
 * from a single command execution.
 */
struct CommandResult {
  int exit_code = -1;       ///< Process exit code (-1 if not executed)
  std::string stdout_text;  ///< Captured stdout output
  std::string stderr_text;  ///< Captured stderr output
};

/**
 * @brief Execute a single command and capture its output
 *
 * Runs a command with the specified executable, arguments,
 * and optional stdin data. Captures both stdout and stderr output.
 *
 * @param exe Path to the executable
 * @param args Vector of command arguments
 * @param stdin_data Optional string to feed to command's stdin
 * @return CommandResult Structure containing execution results
 */
CommandResult run_command(const std::wstring& exe,
                          const std::vector<std::wstring>& args,
                          const std::string& stdin_data = {});

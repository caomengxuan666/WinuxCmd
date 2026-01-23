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
 *  - File: main.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// src/main.cpp
// Main entry point for WinuxCmd
#include <cstdio>
import std;
import core.dispatcher;

/**
 * @brief Print help information
 * @return Exit code (1 - error)
 */
static int printHelp() noexcept {
  printf("WinuxCmd - Windows Compatible Linux Command Set\n");
  printf("Usage: winuxcmd <command> [options]...\n");
  printf("\n");
  printf("Available commands:\n");
  printf("  ls       List directory contents\n");
  printf("\n");
  printf("Use 'winuxcmd <command> --help' for command-specific help.\n");
  return 1;
}

/**
 * @brief Main function for WinuxCmd
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit code from the executed command (0 = success, non-zero = error)
 */
int main(int argc, char* argv[]) noexcept {
  if (argc < 1) {
    return printHelp();
  }

  // Get the executable name (stem only)
  std::filesystem::path self_path(argv[0]);
  std::string self_name = self_path.stem().string();

  // Convert command-line arguments to string_views for efficiency
  std::vector<std::string_view> args(argv + 1, argv + argc);

  if (self_name == "winuxcmd") {
    // Mode 1: winuxcmd <command> [args...] (e.g., winuxcmd ls -la)
    if (args.empty()) {
      return printHelp();
    }

    // Extract command name and remaining arguments
    const std::string_view cmd_name = args[0];
    const std::span<std::string_view> cmd_args(args.data() + 1,
                                               args.size() - 1);

    // Dispatch the command
    return CommandRegistry::dispatch(cmd_name, cmd_args);
  } else {
    // Mode 2: <command>.exe [args...] (e.g., ls.exe -la)
    // Treat executable name as command name
    const std::span<std::string_view> cmd_args(args.data(), args.size());

    // Dispatch the command
    return CommandRegistry::dispatch(self_name, cmd_args);
  }
}

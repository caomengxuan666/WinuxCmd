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
 *  - File: mkdir.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// Use global module fragment for C standard library
module;
#include <cstdio>

#include "core/command_macros.h"
export module commands.mkdir;

import std;
import core.dispatcher;
import core.cmd_meta;

/**
 * @brief Make directories
 * @param args Command-line arguments
 * @return Exit code (0 on success, non-zero on error)
 *
 * Options:
 *  -p, --parents            No error if existing, make parent directories as
 * needed -v, --verbose            Print a message for each created directory
 */

constexpr auto MKDIR_OPTIONS = std::array{
    OPTION("-m", "--mode", "set file mode (as in chmod), not a=rwx - umask"),
    OPTION("-p", "--parents",
           "no error if existing, make parent directories as needed"),
    OPTION("-v", "--verbose", "print a message for each created directory"),
    OPTION("-Z", "",
           "set SELinux security context of each created directory to the "
           "default type")};

REGISTER_COMMAND(
    mkdir,
    /* cmd_name */ "mkdir",
    /* cmd_synopsis */ "make directories",
    /* cmd_desc */
    "Create the DIRECTORY(ies), if they do not already exist.\n"
    "\n"
    "If the directory already exists, mkdir will fail unless the -p option is "
    "used.\n"
    "With -p, mkdir will create parent directories as needed.\n",
    /* examples */
    "  mkdir dir1                Create directory dir1\n"
    "  mkdir -p dir1/dir2/dir3    Create nested directories\n"
    "  mkdir -v dir1 dir2         Verbose create directories\n"
    "  mkdir -pv dir1/dir2        Verbose create nested directories",
    /* see_also */ "rmdir(1), rm(1), cp(1), mv(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    MKDIR_OPTIONS) {
  // Option flags for mkdir command
  struct MkdirOptions {
    bool parents = false;          // -p, --parents
    bool verbose = false;          // -v, --verbose
    std::string mode;              // -m, --mode
    bool selinux_context = false;  // -Z
  };

  /**
   * @brief Parse command line options for mkdir
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param paths Output parameter for paths to create
   * @return true if parsing succeeded, false on error
   */
  auto parseMkdirOptions = [](std::span<std::string_view> args,
                              MkdirOptions& options,
                              std::vector<std::string>& paths) -> bool {
    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        if (arg == "--parents") {
          options.parents = true;
        } else if (arg == "--verbose") {
          options.verbose = true;
        } else if (arg == "--mode") {
          if (i + 1 < args.size()) {
            options.mode = args[i + 1];
            ++i;
          } else {
            fprintf(stderr, "mkdir: option '--mode' requires an argument\n");
            return false;
          }
        } else {
          fprintf(stderr, "mkdir: invalid option -- '%.*s'\n",
                  static_cast<int>(arg.size() - 2), arg.data() + 2);
          return false;
        }
      } else if (arg.starts_with('-')) {
        // This is a short option
        if (arg == "-") {
          // Single dash, treat as path
          paths.push_back(std::string(arg));
          continue;
        }

        // Process option characters
        for (size_t j = 1; j < arg.size(); ++j) {
          switch (arg[j]) {
            case 'p':
              options.parents = true;
              break;
            case 'v':
              options.verbose = true;
              break;
            case 'm':
              if (j + 1 < arg.size()) {
                options.mode = arg.substr(j + 1);
                j = arg.size() - 1;
              } else if (i + 1 < args.size()) {
                options.mode = args[i + 1];
                ++i;
                break;
              } else {
                fprintf(stderr, "mkdir: option requires an argument -- 'm'\n");
                return false;
              }
            case 'Z':
              options.selinux_context = true;
              break;
            default:
              fprintf(stderr, "mkdir: invalid option -- '%c'\n", arg[j]);
              return false;
          }
        }
      } else {
        // This is a path
        paths.push_back(std::string(arg));
      }
    }

    if (paths.empty()) {
      fprintf(stderr, "mkdir: missing operand\n");
      return false;
    }

    return true;
  };

  /**
   * @brief Create a directory
   * @param path Path to create
   * @param options mkdir command options
   * @return true if directory was created successfully, false on error
   */
  auto createDirectory = [](const std::string& path,
                            const MkdirOptions& options) -> bool {
    try {
      std::filesystem::path fsPath(path);
      if (options.parents) {
        std::filesystem::create_directories(fsPath);
        if (options.verbose)
          printf("mkdir: created directory '%s'\n", path.data());
      } else {
        bool created = std::filesystem::create_directory(fsPath);
        if (created) {
          if (options.verbose)
            printf("mkdir: created directory '%s'\n", path.data());
        } else {
          if (std::filesystem::exists(fsPath)) {
            fprintf(stderr,
                    "mkdir: cannot create directory '%s': File exists\n",
                    path.data());
            return false;
          } else {
            throw std::filesystem::filesystem_error(
                "mkdir: cannot create directory", fsPath, std::error_code());
          }
        }
      }
      return true;
    } catch (const std::filesystem::filesystem_error& e) {
      fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", path.data(),
              e.what());
      return false;
    } catch (const std::exception& e) {
      fprintf(stderr, "mkdir: error: %s\n", e.what());
      return false;
    }
  };

  // Main implementation
  MkdirOptions options;
  std::vector<std::string> paths;
  if (!parseMkdirOptions(args, options, paths)) {
    return 2;  // Invalid option error code
  }

  bool success = true;
  for (const auto& path : paths) {
    if (!createDirectory(path, options)) {
      success = false;
    }
  }

  return success ? 0 : 1;
}

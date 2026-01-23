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
 *  - File: mv.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// Use global module fragment for C standard library
module;
#include <cstdio>
#define WIN32_LEAN_AND_MEAN
#include <shlwapi.h>
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")
#include "core/command_macros.h"
export module commands.mv;

import std;
import core.dispatcher;
import core.cmd_meta;

/**
 * @brief Move files and directories
 * @param args Command-line arguments
 * @return Exit code (0 on success, non-zero on error)
 *
 * Options:
 *  -i, --interactive        Prompt before overwrite
 *  -v, --verbose            Explain what is being done
 */

constexpr auto MV_OPTIONS = std::array{
    OPTION("-b", "", "like --backup but does not accept an argument"),
    OPTION("-f", "--force", "do not prompt before overwriting"),
    OPTION("-i", "--interactive", "prompt before overwrite"),
    OPTION("-n", "--no-clobber", "do not overwrite an existing file"),
    OPTION("-S", "--suffix", "override the usual backup suffix"),
    OPTION("-t", "--target-directory",
           "move all SOURCE arguments into DIRECTORY"),
    OPTION("-T", "--no-target-directory", "treat DEST as a normal file"),
    OPTION("-u", "", "equivalent to --update[=older]"),
    OPTION("-v", "--verbose", "explain what is being done"),
    OPTION("-Z", "--context",
           "set SELinux security context of destination file to default type")};

REGISTER_COMMAND(
    mv,
    /* cmd_name */ "mv",
    /* cmd_synopsis */ "move (rename) files and directories",
    /* cmd_desc */
    "Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n"
    "\n"
    "In the first form, rename SOURCE to DEST.\n"
    "In the second form, move each SOURCE to DIRECTORY.\n",
    /* examples */
    "  mv file1.txt file2.txt       Rename file1.txt to file2.txt\n"
    "  mv file.txt dir/              Move file.txt to directory dir/\n"
    "  mv -v file.txt new.txt        Verbose move\n"
    "  mv -i file.txt existing.txt   Interactive move (prompt before "
    "overwrite)",
    /* see_also */ "cp(1), rm(1), ln(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    MV_OPTIONS) {
  // Option flags for mv command
  struct MvOptions {
    bool interactive = false;          // -i, --interactive
    bool verbose = false;              // -v, --verbose
    bool backup = false;               // -b
    bool force = false;                // -f, --force
    bool no_clobber = false;           // -n, --no-clobber
    bool suffix = false;               // -S, --suffix
    bool target_directory = false;     // -t, --target-directory
    bool no_target_directory = false;  // -T, --no-target-directory
    bool update = false;               // -u
    bool context = false;              // -Z, --context
    std::string backup_suffix = "~";   // --suffix
    std::string target_dir;            // --target-directory
  };

  /**
   * @brief Parse command line options for mv
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param sourcePaths Output parameter for source paths
   * @param destPath Output parameter for destination path
   * @return true if parsing succeeded, false on error
   */
  auto parseMvOptions = [](std::span<std::string_view> args, MvOptions& options,
                           std::vector<std::string>& sourcePaths,
                           std::string& destPath) -> bool {
    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        if (arg == "--interactive") {
          options.interactive = true;
        } else if (arg == "--verbose") {
          options.verbose = true;
        } else if (arg == "--force") {
          options.force = true;
        } else if (arg == "--no-clobber") {
          options.no_clobber = true;
        } else if (arg == "--suffix") {
          if (i + 1 < args.size()) {
            options.backup_suffix = args[i + 1];
            options.suffix = true;
            ++i;
          } else {
            fprintf(stderr, "mv: option '--suffix' requires an argument\n");
            return false;
          }
        } else if (arg == "--target-directory") {
          if (i + 1 < args.size()) {
            options.target_dir = args[i + 1];
            options.target_directory = true;
            ++i;
          } else {
            fprintf(stderr,
                    "mv: option '--target-directory' requires an argument\n");
            return false;
          }
        } else if (arg == "--no-target-directory") {
          options.no_target_directory = true;
        } else if (arg == "--context") {
          options.context = true;
        } else {
          fprintf(stderr, "mv: invalid option -- '%.*s'\n",
                  static_cast<int>(arg.size() - 2), arg.data() + 2);
          return false;
        }
      } else if (arg.starts_with('-')) {
        // This is a short option
        if (arg == "-") {
          // Single dash, treat as path
          sourcePaths.push_back(std::string(arg));
          continue;
        }

        // Process option characters
        for (size_t j = 1; j < arg.size(); ++j) {
          switch (arg[j]) {
            case 'i':
              options.interactive = true;
              break;
            case 'v':
              options.verbose = true;
              break;
            case 'b':
              options.backup = true;
              break;
            case 'f':
              options.force = true;
              break;
            case 'n':
              options.no_clobber = true;
              break;
            case 'S':
              if (j + 1 < arg.size()) {
                options.backup_suffix = arg.substr(j + 1);
                options.suffix = true;
                j = arg.size() - 1;
              } else if (i + 1 < args.size()) {
                options.backup_suffix = args[i + 1];
                options.suffix = true;
                ++i;
                break;
              } else {
                fprintf(stderr, "mv: option requires an argument -- 'S'\n");
                return false;
              }
            case 't':
              if (j + 1 < arg.size()) {
                options.target_dir = arg.substr(j + 1);
                options.target_directory = true;
                j = arg.size() - 1;
              } else if (i + 1 < args.size()) {
                options.target_dir = args[i + 1];
                options.target_directory = true;
                ++i;
                break;
              } else {
                fprintf(stderr, "mv: option requires an argument -- 't'\n");
                return false;
              }
            case 'T':
              options.no_target_directory = true;
              break;
            case 'u':
              options.update = true;
              break;
            case 'Z':
              options.context = true;
              break;
            default:
              fprintf(stderr, "mv: invalid option -- '%c'\n", arg[j]);
              return false;
          }
        }
      } else {
        // This is a path
        sourcePaths.push_back(std::string(arg));
      }
    }

    if (options.target_directory) {
      if (sourcePaths.empty()) {
        fprintf(stderr, "mv: missing file operand\n");
        return false;
      }
      destPath = options.target_dir;
    } else {
      if (sourcePaths.size() < 2) {
        fprintf(stderr, "mv: missing file operand\n");
        return false;
      }
      destPath = sourcePaths.back();
      sourcePaths.pop_back();
    }

    return true;
  };

  /**
   * @brief Move a file or directory
   * @param srcPath Source path
   * @param destPath Destination path
   * @param options mv command options
   * @return true if move was successful, false on error
   */
  auto movePath = [](const std::string& srcPath, const std::string& destPath,
                     const MvOptions& options) -> bool {
    try {
      std::filesystem::path srcFsPath(srcPath);
      std::filesystem::path destFsPath(destPath);

      if (options.interactive) {
        if (std::filesystem::exists(destFsPath)) {
          printf("mv: overwrite '%s'? (y/n) ", destPath.c_str());
          char response;
          std::cin.get(response);

          // Clear any remaining characters in the input buffer (including
          // newline)
          std::cin.ignore(1024, '\n');

          if (response != 'y' && response != 'Y') {
            return true;
          }
        }
      }

      // First try to rename (works if on same filesystem)
      try {
        std::filesystem::rename(srcFsPath, destFsPath);
      } catch (const std::filesystem::filesystem_error& e) {
        // If rename fails (e.g., different filesystems), try copy then delete
        std::filesystem::copy_file(
            srcFsPath, destFsPath,
            std::filesystem::copy_options::overwrite_existing);
        std::filesystem::remove(srcFsPath);
      }

      if (options.verbose) {
        printf("'%s' -> '%s'\n", srcPath.data(), destPath.c_str());
      }

      return true;
    } catch (const std::filesystem::filesystem_error& e) {
      fprintf(stderr, "mv: cannot move '%s' to '%s': %s\n", srcPath.data(),
              destPath.c_str(), e.what());
      return false;
    } catch (const std::exception& e) {
      fprintf(stderr, "mv: error: %s\n", e.what());
      return false;
    }
  };

  /**
   * @brief Check if a path exists and is a directory
   * @param path Path to check
   * @return true if path exists and is a directory, false otherwise
   */
  auto pathExistsAndIsDirectory = [](const std::string& path) -> bool {
    std::wstring wpath(path.begin(), path.end());
    DWORD attr = GetFileAttributesW(wpath.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) &&
           (attr & FILE_ATTRIBUTE_DIRECTORY);
  };

  /**
   * @brief Check if a path exists
   * @param path Path to check
   * @return true if path exists, false otherwise
   */
  auto pathExists = [](const std::string& path) -> bool {
    std::wstring wpath(path.begin(), path.end());
    DWORD attr = GetFileAttributesW(wpath.c_str());
    return attr != INVALID_FILE_ATTRIBUTES;
  };

  // Main implementation
  MvOptions options;
  std::vector<std::string> sourcePaths;
  std::string destPath;

  if (!parseMvOptions(args, options, sourcePaths, destPath)) {
    return 2;  // Invalid option error code
  }

  bool destIsDir = pathExistsAndIsDirectory(destPath);

  if (sourcePaths.size() > 1 && !destIsDir) {
    fprintf(stderr, "mv: target '%s' is not a directory\n", destPath.c_str());
    return 1;
  }

  bool success = true;

  for (const auto& srcPath : sourcePaths) {
    if (!pathExists(srcPath)) {
      fprintf(stderr, "mv: cannot stat '%s': No such file or directory\n",
              srcPath.data());
      success = false;
      continue;
    }

    std::string finalDestPath = destPath;
    if (destIsDir) {
      std::wstring wsrcPath(srcPath.begin(), srcPath.end());
      LPWSTR fileName = PathFindFileNameW(wsrcPath.c_str());
      int fileNameLength =
          WideCharToMultiByte(CP_UTF8, 0, fileName, -1, NULL, 0, NULL, NULL);
      std::string fileNameStr(fileNameLength, 0);
      WideCharToMultiByte(CP_UTF8, 0, fileName, -1, &fileNameStr[0],
                          fileNameLength, NULL, NULL);
      finalDestPath += "\\" + fileNameStr;
    }

    if (!movePath(srcPath, finalDestPath, options)) {
      success = false;
    }
  }

  return success ? 0 : 1;
}

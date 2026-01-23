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
 *  - File: rm.cppm
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
export module commands.rm;

import std;
import core.dispatcher;
import core.cmd_meta;

/**
 * @brief Remove files or directories
 * @param args Command-line arguments
 * @return Exit code (0 on success, non-zero on error)
 *
 * Options:
 *  -r, --recursive          Remove directories and their contents recursively
 *  -i, --interactive        Prompt before every removal
 *  -f, --force              Ignore nonexistent files and arguments, never
 * prompt -v, --verbose            Explain what is being done
 */

constexpr auto RM_OPTIONS = std::array{
    OPTION("-f", "--force",
           "ignore nonexistent files and arguments, never prompt"),
    OPTION("-i", "", "prompt before every removal"),
    OPTION("-I", "",
           "prompt once before removing more than three files, or when "
           "removing recursively"),
    OPTION("-d", "--dir", "remove empty directories"),
    OPTION("-r", "--recursive",
           "remove directories and their contents recursively"),
    OPTION("-R", "--recursive",
           "remove directories and their contents recursively"),
    OPTION("-v", "--verbose", "explain what is being done"),
    OPTION("--interactive", "",
           "prompt according to WHEN: never, once (-I), or always (-i)"),
    OPTION("--one-file-system", "",
           "when removing a hierarchy recursively, skip any directory that is "
           "on a file system different from that of the corresponding command "
           "line argument"),
    OPTION("--no-preserve-root", "", "do not treat '/' specially"),
    OPTION("--preserve-root", "", "do not remove '/' (default)")};

REGISTER_COMMAND(
    rm,
    /* cmd_name */ "rm",
    /* cmd_synopsis */ "remove files or directories",
    /* cmd_desc */
    "Remove the FILE(s).\n"
    "\n"
    "By default, rm does not remove directories. Use the --recursive (-r) "
    "option\n"
    "to remove each listed directory, too, along with all of its contents.\n",
    /* examples */
    "  rm file.txt               Remove file.txt\n"
    "  rm -r dir/                Recursively remove directory dir/\n"
    "  rm -v file1.txt file2.txt Verbose remove\n"
    "  rm -i file.txt            Interactive remove (prompt before removal)",
    /* see_also */ "cp(1), mv(1), mkdir(1), rmdir(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    RM_OPTIONS) {
  // Option flags for rm command
  struct RmOptions {
    bool recursive = false;         // -r, -R, --recursive
    bool interactive = false;       // -i, --interactive
    bool force = false;             // -f, --force
    bool verbose = false;           // -v, --verbose
    bool remove_dir = false;        // -d, --dir
    bool prompt_once = false;       // -I
    bool one_file_system = false;   // --one-file-system
    bool no_preserve_root = false;  // --no-preserve-root
    bool preserve_root = true;      // --preserve-root
  };

  /**
   * @brief Parse command line options for rm
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param paths Output parameter for paths to remove
   * @return true if parsing succeeded, false on error
   */
  auto parseRmOptions = [](std::span<std::string_view> args, RmOptions& options,
                           std::vector<std::string>& paths) -> bool {
    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        if (arg == "--recursive") {
          options.recursive = true;
        } else if (arg == "--interactive") {
          options.interactive = true;
        } else if (arg == "--force") {
          options.force = true;
        } else if (arg == "--verbose") {
          options.verbose = true;
        } else if (arg == "--dir") {
          options.remove_dir = true;
        } else if (arg == "--one-file-system") {
          options.one_file_system = true;
        } else if (arg == "--no-preserve-root") {
          options.no_preserve_root = true;
          options.preserve_root = false;
        } else if (arg == "--preserve-root") {
          options.preserve_root = true;
          options.no_preserve_root = false;
        } else {
          fprintf(stderr, "rm: invalid option -- '%.*s'\n",
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
            case 'r':
            case 'R':
              options.recursive = true;
              break;
            case 'i':
              options.interactive = true;
              break;
            case 'f':
              options.force = true;
              break;
            case 'v':
              options.verbose = true;
              break;
            case 'd':
              options.remove_dir = true;
              break;
            case 'I':
              options.prompt_once = true;
              break;
            default:
              fprintf(stderr, "rm: invalid option -- '%c'\n", arg[j]);
              return false;
          }
        }
      } else {
        // This is a path
        paths.push_back(std::string(arg));
      }
    }

    if (paths.empty()) {
      fprintf(stderr, "rm: missing file operand\n");
      return false;
    }

    return true;
  };

  /**
   * @brief Helper function to convert wstring to UTF-8 string
   * @param wstr Wide string to convert
   * @return UTF-8 string
   */
  auto wstringToUtf8 = [](const std::wstring& wstr) -> std::string {
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0,
                                   nullptr, nullptr);
    if (size == 0) return "";
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr,
                        nullptr);
    result.pop_back();  // Remove trailing null character
    return result;
  };

  /**
   * @brief Remove a file or directory
   * @param path Path to remove
   * @param options rm command options
   * @return true if removal was successful, false on error
   */
  std::function<bool(const std::string&, const RmOptions&)> removePath;

  removePath = [&](const std::string& path, const RmOptions& options) -> bool {
    std::wstring wpath(path.begin(), path.end());
    DWORD attr = GetFileAttributesW(wpath.c_str());

    if (attr == INVALID_FILE_ATTRIBUTES) {
      if (options.force) {
        return true;
      } else {
        fprintf(stderr, "rm: cannot remove '%s': No such file or directory\n",
                path.data());
        return false;
      }
    }

    if (options.interactive) {
      printf("rm: remove '%s'? (y/n) ", path.data());
      char response;
      std::cin.get(response);
      if (response != 'y' && response != 'Y') {
        return true;
      }
    }

    if ((attr & FILE_ATTRIBUTE_DIRECTORY) && !options.recursive) {
      fprintf(stderr, "rm: cannot remove '%s': Is a directory\n", path.data());
      return false;
    }

    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
      // Recursive function to delete directory with post-order traversal
      std::function<bool(const std::wstring&)> removeDirectoryRecursive;
      removeDirectoryRecursive = [&](const std::wstring& dirPath) -> bool {
        // First, enumerate all items in the directory
        std::wstring searchPath = dirPath + L"\\*";
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
          // Directory might be empty or inaccessible
          return true;
        }

        std::vector<std::wstring> subdirs;
        bool success = true;

        do {
          std::wstring itemName(findData.cFileName);
          if (itemName != L"." && itemName != L"..") {
            std::wstring itemPath = dirPath + L"\\" + itemName;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
              // Store subdirectory for later recursive deletion
              subdirs.push_back(itemPath);
            } else {
              // Delete file
              if (!DeleteFileW(itemPath.c_str())) {
                DWORD error = GetLastError();
                char errorMsg[256];
                FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0,
                               errorMsg, sizeof(errorMsg), NULL);
                std::string itemPathStr = wstringToUtf8(itemPath);
                fprintf(stderr, "rm: cannot remove file '%s': %s\n",
                        itemPathStr.c_str(), errorMsg);
                success = false;
              } else if (options.verbose) {
                std::string itemPathStr = wstringToUtf8(itemPath);
                printf("removed '%s'\n", itemPathStr.c_str());
              }
            }
          }
        } while (FindNextFileW(hFind, &findData));

        FindClose(hFind);

        // If we encountered errors, don't continue
        if (!success) {
          return false;
        }

        // Recursively delete all subdirectories (post-order traversal)
        for (const auto& subdir : subdirs) {
          if (!removeDirectoryRecursive(subdir)) {
            return false;
          }
        }

        // Finally, remove the directory itself
        if (!RemoveDirectoryW(dirPath.c_str())) {
          DWORD error = GetLastError();
          char errorMsg[256];
          FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg,
                         sizeof(errorMsg), NULL);
          std::string dirPathStr = wstringToUtf8(dirPath);
          fprintf(stderr, "rm: cannot remove directory '%s': %s\n",
                  dirPathStr.c_str(), errorMsg);
          return false;
        }

        if (options.verbose) {
          std::string dirPathStr = wstringToUtf8(dirPath);
          printf("removed '%s'\n", dirPathStr.c_str());
        }

        return true;
      };

      // Start recursive directory deletion
      return removeDirectoryRecursive(wpath);
    } else {
      // Delete regular file
      BOOL success = DeleteFileW(wpath.c_str());
      if (!success) {
        DWORD error = GetLastError();
        char errorMsg[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg,
                       sizeof(errorMsg), NULL);
        fprintf(stderr, "rm: cannot remove file '%s': %s\n", path.data(),
                errorMsg);
        return false;
      }

      if (options.verbose) {
        printf("removed '%s'\n", path.data());
      }
    }

    return true;
  };

  // Main implementation
  RmOptions options;
  std::vector<std::string> paths;

  if (!parseRmOptions(args, options, paths)) {
    return 2;  // Invalid option error code
  }

  bool success = true;

  for (const auto& path : paths) {
    if (!removePath(path, options)) {
      success = false;
    }
  }

  return success ? 0 : 1;
}

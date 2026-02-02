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
#include <cwchar>
#define WIN32_LEAN_AND_MEAN
#include <shlwapi.h>
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")
#include "core/auto_flags.h"
#include "core/command_macros.h"
export module commands.rm;

import std;
import core.dispatcher;
import core.cmd_meta;
import core.opt;

/**
 * @brief RM command options definition
 * 
 * This array defines all the options supported by the rm command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 * 
 * @par Options:
 * - @a -f, @a --force: Ignore nonexistent files and arguments, never prompt [IMPLEMENTED]
 * - @a -i: Prompt before every removal [IMPLEMENTED]
 * - @a -I: Prompt once before removing more than three files, or when removing recursively [IMPLEMENTED]
 * - @a -d, @a --dir: Remove empty directories [IMPLEMENTED]
 * - @a -r, @a --recursive: Remove directories and their contents recursively [IMPLEMENTED]
 * - @a -R, @a --recursive: Remove directories and their contents recursively [IMPLEMENTED]
 * - @a -v, @a --verbose: Explain what is being done [IMPLEMENTED]
 * - @a --interactive: Prompt according to WHEN: never, once (-I), or always (-i) [IMPLEMENTED]
 * - @a --one-file-system: When removing a hierarchy recursively, skip any directory that is on a file system different from that of the corresponding command line argument [TODO]
 * - @a --no-preserve-root: Do not treat '/' specially [IMPLEMENTED]
 * - @a --preserve-root: Do not remove '/' (default) [IMPLEMENTED]
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

// Auto-generated lookup table for options from RM_OPTIONS
constexpr auto OPTION_HANDLERS =
    generate_option_handlers(RM_OPTIONS, "--interactive");

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
  CREATE_AUTO_FLAGS_CLASS(
      RmOptions, DEFINE_FLAG(recursive, 0)  // -r, -R, --recursive
      DEFINE_FLAG(interactive, 1)           // -i, --interactive
      DEFINE_FLAG(force, 2)                 // -f, --force
      DEFINE_FLAG(verbose, 3)               // -v, --verbose
      DEFINE_FLAG(remove_dir, 4)            // -d, --dir
      DEFINE_FLAG(prompt_once, 5)           // -I
      DEFINE_FLAG(one_file_system, 6)       // --one-file-system
      DEFINE_FLAG(no_preserve_root, 7)      // --no-preserve-root
      DEFINE_FLAG(preserve_root, 8)         // --preserve-root
  )

  /**
   * @brief Parse command line options for rm
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param paths Output parameter for paths to remove
   * @return true if parsing succeeded, false on error
   */
  auto parseRmOptions = [](std::span<std::string_view> args, RmOptions& options,
                           std::vector<std::string>& paths) -> bool {
    // Helper function to find option handler by long option or short option
    auto find_handler = [](std::string_view arg,
                           char opt_char = '\0') -> const OptionHandler* {
      for (const auto& handler : OPTION_HANDLERS) {
        if ((!arg.empty() && handler.long_opt && arg == handler.long_opt) ||
            (opt_char && handler.short_opt == opt_char)) {
          return &handler;
        }
      }
      return nullptr;
    };

    // Helper function to print option error
    auto print_option_error = [](std::string_view arg, char opt_char = '\0') {
      if (!arg.empty()) {
        fwprintf(stderr, L"rm: invalid option -- '%.*s'\n",
                 static_cast<int>(arg.size() - 2), arg.data() + 2);
      } else {
        fwprintf(stderr, L"rm: invalid option -- '%c'\n", opt_char);
      }
    };

    // Helper function to set boolean option
    auto set_boolean_option = [&options](char opt_char) {
      switch (opt_char) {
        case 'r':
        case 'R':
          options.set_recursive(true);
          break;
        case 'i':
          options.set_interactive(true);
          break;
        case 'f':
          options.set_force(true);
          break;
        case 'v':
          options.set_verbose(true);
          break;
        case 'd':
          options.set_remove_dir(true);
          break;
        case 'I':
          options.set_prompt_once(true);
          break;
      }
    };

    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        const auto* handler = find_handler(arg);
        if (handler) {
          if (handler->requires_arg) {
            // Handle options that require arguments
            if (i + 1 < args.size()) {
              // rm command has no options that require arguments
              ++i;
            } else {
              fwprintf(stderr, L"rm: option '%s' requires an argument\n",
                       arg.data());
              return false;
            }
          } else {
            // Special case for --no-preserve-root and --preserve-root
            if (arg == "--no-preserve-root") {
              options.set_no_preserve_root(true);
              options.set_preserve_root(false);
            } else if (arg == "--preserve-root") {
              options.set_preserve_root(true);
              options.set_no_preserve_root(false);
            } else if (arg == "--one-file-system") {
              options.set_one_file_system(true);
            } else {
              set_boolean_option(handler->short_opt);
            }
          }
        } else {
          print_option_error(arg);
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
          char opt_char = arg[j];
          const auto* handler = find_handler("", opt_char);
          if (handler) {
            if (handler->requires_arg) {
              // rm command has no options that require arguments
              fwprintf(stderr, L"rm: option requires an argument -- '%c'\n",
                       opt_char);
              return false;
            } else {
              set_boolean_option(opt_char);
            }
          } else {
            print_option_error("", opt_char);
            return false;
          }
        }
      } else {
        // This is a path
        paths.push_back(std::string(arg));
      }
    }

    if (paths.empty()) {
      fwprintf(stderr, L"rm: missing file operand\n");
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
      if (options.get_force()) {
        return true;
      } else {
        fwprintf(stderr,
                 L"rm: cannot remove '%hs': No such file or directory\n",
                 path.data());
        return false;
      }
    }

    if (options.get_interactive()) {
      wprintf(L"rm: remove '%hs? (y/n) ", path.data());
      char response;
      std::cin.get(response);
      if (response != 'y' && response != 'Y') {
        return true;
      }
    }

    if ((attr & FILE_ATTRIBUTE_DIRECTORY) && !options.get_recursive()) {
      fwprintf(stderr, L"rm: cannot remove '%hs': Is a directory\n",
               path.data());
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
                fwprintf(stderr, L"rm: cannot remove file '%hs': %hs\n",
                         itemPathStr.c_str(), errorMsg);
                success = false;
              } else if (options.get_verbose()) {
                std::string itemPathStr = wstringToUtf8(itemPath);
                wprintf(L"removed '%hs'\n", itemPathStr.c_str());
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
          fwprintf(stderr, L"rm: cannot remove directory '%hs': %hs\n",
                   dirPathStr.c_str(), errorMsg);
          return false;
        }

        if (options.get_verbose()) {
          std::string dirPathStr = wstringToUtf8(dirPath);
          wprintf(L"removed '%hs'\n", dirPathStr.c_str());
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
        fwprintf(stderr, L"rm: cannot remove file '%hs': %hs\n", path.data(),
                 errorMsg);
        return false;
      }

      if (options.get_verbose()) {
        wprintf(L"removed '%hs'\n", path.data());
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

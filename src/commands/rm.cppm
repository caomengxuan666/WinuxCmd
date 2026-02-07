/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
///   - @description:
/// @Description: Implemention for rm.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

module;
#include "pch/pch.h"
#pragma comment(lib, "shlwapi.lib")
#include "core/command_macros.h"
export module commands.rm;

import std;
import core;
import utils;

using namespace core::pipeline;

/**
 * @brief RM command options definition
 *
 * This array defines all the options supported by the rm command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 *
 * - @a -f, @a --force: Ignore nonexistent files and arguments, never prompt
 * [IMPLEMENTED]
 * - @a -i: Prompt before every removal [IMPLEMENTED]
 * - @a -I: Prompt once before removing more than three files, or when removing
 * recursively [IMPLEMENTED]
 * - @a -d, @a --dir: Remove empty directories [IMPLEMENTED]
 * - @a -r, @a --recursive: Remove directories and their contents recursively
 * [IMPLEMENTED]
 * - @a -R, @a --recursive: Remove directories and their contents recursively
 * [IMPLEMENTED]
 * - @a -v, @a --verbose: Explain what is being done [IMPLEMENTED]
 * - @a --interactive: Prompt according to WHEN: never, once (-I), or always
 * (-i) [IMPLEMENTED]
 * - @a --one-file-system: When removing a hierarchy recursively, skip any
 * directory that is on a file system different from that of the corresponding
 * command line argument [TODO]
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

// ======================================================
// Pipeline components
// ======================================================
namespace rm_pipeline {
  namespace cp = core::pipeline;

  /**
   * @brief Helper function to convert wstring to UTF-8 string
   * @param wstr Wide string to convert
   * @return UTF-8 string
   */
  auto wstringToUtf8(const std::wstring& wstr) -> std::string {
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0,
                                   nullptr, nullptr);
    if (size == 0) return "";
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr,
                        nullptr);
    result.pop_back();  // Remove trailing null character
    return result;
  }

  /**
   * @brief Check if paths are provided
   * @param paths Paths to check
   * @return Result with paths if valid, error otherwise
   */
  auto check_paths(const std::vector<std::string>& paths) -> cp::Result<std::vector<std::string>> {
    if (paths.empty()) {
      return std::unexpected("missing file operand");
    }
    return paths;
  }

  /**
   * @brief Remove a file or directory
   * @param path Path to remove
   * @param ctx Command context with options
   * @return true if removal was successful, false on error
   */
  auto remove_path(const std::string& path, const CommandContext<RM_OPTIONS.size()>& ctx) -> bool {
    std::wstring wpath(path.begin(), path.end());
    DWORD attr = GetFileAttributesW(wpath.c_str());

    bool force = ctx.get<bool>("--force", false) || ctx.get<bool>("-f", false);
    bool interactive = ctx.get<bool>("--interactive", false) || ctx.get<bool>("-i", false);
    bool recursive = ctx.get<bool>("--recursive", false) || ctx.get<bool>("-r", false) || ctx.get<bool>("-R", false);
    bool verbose = ctx.get<bool>("--verbose", false) || ctx.get<bool>("-v", false);

    if (attr == INVALID_FILE_ATTRIBUTES) {
      if (force) {
        return true;
      } else {
        std::wstring wpath = utf8_to_wstring(path);
        safeErrorPrintLn(L"rm: cannot remove '" + wpath +
                         L"': No such file or directory");
        return false;
      }
    }

    if (interactive) {
      safeErrorPrint(L"rm: remove '" + utf8_to_wstring(path) + L"'? (y/n) ");
      char response;
      std::cin.get(response);
      if (response != 'y' && response != 'Y') {
        return true;
      }
    }

    if ((attr & FILE_ATTRIBUTE_DIRECTORY) && !recursive) {
      std::wstring wpath = utf8_to_wstring(path);
      safeErrorPrintLn(L"rm: cannot remove '" + wpath + L"': Is a directory");
      return false;
    }

    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
      // Recursive function to delete directory with post-order traversal
      std::function<bool(const std::wstring&)> remove_directory_recursive;
      remove_directory_recursive = [&](const std::wstring& dirPath) -> bool {
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
                std::wstring witemPath = utf8_to_wstring(itemPathStr);
                std::wstring werrorMsg = utf8_to_wstring(std::string(errorMsg));
                safeErrorPrintLn(L"rm: cannot remove file '" + witemPath +
                                 L"': " + werrorMsg);
                success = false;
              } else if (verbose) {
                std::string itemPathStr = wstringToUtf8(itemPath);
                safePrintLn(L"removed '" + utf8_to_wstring(itemPathStr) + L"'");
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
          if (!remove_directory_recursive(subdir)) {
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
          std::wstring wdirPath = utf8_to_wstring(dirPathStr);
          std::wstring werrorMsg = utf8_to_wstring(std::string(errorMsg));
          safeErrorPrintLn(L"rm: cannot remove directory '" + wdirPath +
                           L"': " + werrorMsg);
          return false;
        }

        if (verbose) {
          std::string dirPathStr = wstringToUtf8(dirPath);
          safePrintLn(L"removed '" + utf8_to_wstring(dirPathStr) + L"'");
        }

        return true;
      };

      // Start recursive directory deletion
      return remove_directory_recursive(wpath);
    } else {
      // Delete regular file
      BOOL success = DeleteFileW(wpath.c_str());
      if (!success) {
        DWORD error = GetLastError();
        char errorMsg[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg,
                       sizeof(errorMsg), NULL);
        std::wstring wpath_str = utf8_to_wstring(std::string(path));
        std::wstring werrorMsg = utf8_to_wstring(std::string(errorMsg));
        safeErrorPrintLn(L"rm: cannot remove file '" + wpath_str + L"': " +
                         werrorMsg);
        return false;
      }

      if (verbose) {
        safePrintLn(L"removed '" + utf8_to_wstring(path) + L"'");
      }
    }

    return true;
  }

  /**
   * @brief Process all paths
   * @param paths Paths to process
   * @param ctx Command context with options
   * @return true if all paths were processed successfully, false otherwise
   */
  auto process_paths(const std::vector<std::string>& paths, const CommandContext<RM_OPTIONS.size()>& ctx) -> bool {
    bool success = true;
    for (const auto& path : paths) {
      if (!remove_path(path, ctx)) {
        success = false;
      }
    }
    return success;
  }

  /**
   * @brief Main pipeline
   * @param ctx Command context
   * @return Result with success status
   */
  auto process_command(const CommandContext<RM_OPTIONS.size()>& ctx) -> cp::Result<bool> {
    std::vector<std::string> paths;
    for (auto arg : ctx.positionals) {
      paths.push_back(std::string(arg));
    }

    return check_paths(paths)
        .and_then([&](const std::vector<std::string>& valid_paths) {
            return process_paths(valid_paths, ctx);
        });
  }
} // namespace rm_pipeline

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
  using namespace rm_pipeline;

  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"rm");
    return 1;
  }

  return *result ? 0 : 1;
}

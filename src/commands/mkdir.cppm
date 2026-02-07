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
 *  - File: mkdir.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
///   - @description:
/// @Description: Implemention for mkdir.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

module;
#include "core/command_macros.h"
#include "pch/pch.h"
export module commands.mkdir;

import std;
import core;
import utils;

using namespace core::pipeline;

/**
 * @brief MKDIR command options definition
 *
 * This array defines all the options supported by the mkdir command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 *
 * - @a -m, @a --mode: Set file mode (as in chmod), not a=rwx - umask [TODO]
 * - @a -p, @a --parents: No error if existing, make parent directories as
 * needed [IMPLEMENTED]
 * - @a -v, @a --verbose: Print a message for each created directory
 * [IMPLEMENTED]
 * - @a -Z: Set SELinux security context of each created directory to the
 * default type [TODO]
 */
constexpr auto MKDIR_OPTIONS = std::array{
    OPTION("-m", "--mode", "set file mode (as in chmod), not a=rwx - umask"),
    OPTION("-p", "--parents",
           "no error if existing, make parent directories as needed"),
    OPTION("-v", "--verbose", "print a message for each created directory"),
    OPTION("-Z", "",
           "set SELinux security context of each created directory to the "
           "default type")};


// ======================================================
// Pipeline components
// ======================================================
namespace mkdir_pipeline {
  namespace cp = core::pipeline;

  /**
   * @brief Check if paths are provided
   * @param paths Paths to check
   * @return Result with paths if valid, error otherwise
   */
  auto check_paths(const std::vector<std::string>& paths) -> cp::Result<std::vector<std::string>> {
    if (paths.empty()) {
      return std::unexpected("missing operand");
    }
    return paths;
  }

  /**
   * @brief Create a directory
   * @param path Path to create
   * @param ctx Command context with options
   * @return true if directory was created successfully, false on error
   */
  auto create_directory(const std::string& path, const CommandContext<MKDIR_OPTIONS.size()>& ctx) -> bool {
    try {
      std::filesystem::path fsPath(path);
      bool parents = ctx.get<bool>("--parents", false) || ctx.get<bool>("-p", false);
      bool verbose = ctx.get<bool>("--verbose", false) || ctx.get<bool>("-v", false);
      
      if (parents) {
        std::filesystem::create_directories(fsPath);
        if (verbose)
          safePrintLn(L"mkdir: created directory '" + fsPath.wstring() + L"'");
      } else {
        bool created = std::filesystem::create_directory(fsPath);
        if (created) {
          if (verbose)
            safePrintLn(L"mkdir: created directory '" + fsPath.wstring() +
                        L"'");
        } else {
          if (std::filesystem::exists(fsPath)) {
            safeErrorPrintLn(L"mkdir: cannot create directory '" +
                             fsPath.wstring() + L"': File exists");
            return false;
          } else {
            throw std::filesystem::filesystem_error(
                "mkdir: cannot create directory", fsPath, std::error_code());
          }
        }
      }
      return true;
    } catch (const std::filesystem::filesystem_error& e) {
      std::wstring wpath = utf8_to_wstring(path);
      std::wstring errorMsg = utf8_to_wstring(e.what());
      safeErrorPrintLn(L"mkdir: cannot create directory '" + wpath + L"': " +
                       errorMsg);
      return false;
    } catch (const std::exception& e) {
      std::string msg(e.what());
      std::wstring wmsg = utf8_to_wstring(msg);
      safeErrorPrintLn(L"mkdir: error: " + wmsg);
      return false;
    }
  }

  /**
   * @brief Process all paths
   * @param paths Paths to process
   * @param ctx Command context with options
   * @return true if all paths were processed successfully, false otherwise
   */
  auto process_paths(const std::vector<std::string>& paths, const CommandContext<MKDIR_OPTIONS.size()>& ctx) -> bool {
    bool success = true;
    for (const auto& path : paths) {
      if (!create_directory(path, ctx)) {
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
  auto process_command(const CommandContext<MKDIR_OPTIONS.size()>& ctx) -> cp::Result<bool> {
    std::vector<std::string> paths;
    for (auto arg : ctx.positionals) {
      paths.push_back(std::string(arg));
    }

    return check_paths(paths)
        .and_then([&](const std::vector<std::string>& valid_paths) {
            return process_paths(valid_paths, ctx);
        });
  }
} // namespace mkdir_pipeline

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
  using namespace mkdir_pipeline;

  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"mkdir");
    return 1;
  }

  return *result ? 0 : 1;
}

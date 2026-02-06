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
#include "core/auto_flags.h"
#include "core/command_macros.h"
#include "pch/pch.h"
export module commands.mkdir;

import std;
import core;
import utils;
/**
 * @brief MKDIR command options definition
 *
 * This array defines all the options supported by the mkdir command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
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

// Auto-generated lookup table for options from MKDIR_OPTIONS
constexpr auto OPTION_HANDLERS =
    generate_option_handlers(MKDIR_OPTIONS, "--mode");

CREATE_AUTO_FLAGS_CLASS(MkdirOptions,
                        DECLARE_STRING_OPTION(mode, "")  // -m, --mode

                        // Define all flags
                        DEFINE_FLAG(parents, 0)          // -p, --parents
                        DEFINE_FLAG(verbose, 1)          // -v, --verbose
                        DEFINE_FLAG(selinux_context, 2)  // -Z
)

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
        std::string opt_str(arg.data() + 2, arg.size() - 2);
        std::wstring wopt_str = utf8_to_wstring(opt_str);
        safeErrorPrintLn(L"mkdir: invalid option -- '" + wopt_str + L"'");
      } else {
        safeErrorPrintLn(L"mkdir: invalid option -- '" +
                         std::wstring(1, static_cast<wchar_t>(opt_char)) +
                         L"'");
      }
    };

    // Helper function to handle argument options
    auto handle_arg_option = [&options, &args](char opt_char, size_t& i,
                                               size_t j,
                                               std::string_view arg) -> bool {
      if (j + 1 < arg.size()) {
        // Argument is part of the same string
        if (opt_char == 'm') {
          options.set_mode(arg.substr(j + 1));
        }
      } else if (i + 1 < args.size()) {
        // Argument is the next string
        if (opt_char == 'm') {
          options.set_mode(args[i + 1]);
        }
        ++i;
      } else {
        // No argument provided
        safeErrorPrintLn(L"mkdir: option requires an argument -- '" +
                         std::wstring(1, static_cast<wchar_t>(opt_char)) +
                         L"'");
        return false;
      }
      return true;
    };

    // Helper function to set boolean option
    auto set_boolean_option = [&options](char opt_char) {
      switch (opt_char) {
        case 'p':
          options.set_parents(true);
          break;
        case 'v':
          options.set_verbose(true);
          break;
        case 'Z':
          options.set_selinux_context(true);
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
              if (handler->short_opt == 'm') {
                options.set_mode(args[i + 1]);
              }
              ++i;
            } else {
              std::wstring warg = utf8_to_wstring(arg);
              safeErrorPrintLn(L"mkdir: option '" + warg +
                               L"' requires an argument");
              return false;
            }
          } else {
            set_boolean_option(handler->short_opt);
          }
        } else {
          print_option_error(arg);
          return false;
        }
      } else if (arg.starts_with("-")) {
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
              // Handle options that require arguments
              if (!handle_arg_option(opt_char, i, j, arg)) {
                return false;
              }
              if (j + 1 < arg.size()) {
                j = arg.size() - 1;
              }
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
      safeErrorPrintLn(L"mkdir: missing operand");
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
      if (options.get_parents()) {
        std::filesystem::create_directories(fsPath);
        if (options.get_verbose())
          safePrintLn(L"mkdir: created directory '" + fsPath.wstring() + L"'");
      } else {
        bool created = std::filesystem::create_directory(fsPath);
        if (created) {
          if (options.get_verbose())
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

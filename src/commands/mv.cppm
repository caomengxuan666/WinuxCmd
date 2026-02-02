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
#include "core/auto_flags.h"
#include "core/command_macros.h"
export module commands.mv;

import std;
import core.dispatcher;
import core.cmd_meta;
import core.opt;

/**
 * @brief MV command options definition
 * 
 * This array defines all the options supported by the mv command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 * 
 * @par Options:
 * - @a -b: Like --backup but does not accept an argument [TODO]
 * - @a -f, @a --force: Do not prompt before overwriting [IMPLEMENTED]
 * - @a -i: Prompt before overwrite [IMPLEMENTED]
 * - @a -n, @a --no-clobber: Do not overwrite an existing file [IMPLEMENTED]
 * - @a --strip-trailing-slashes: Remove any trailing slashes from each SOURCE argument [TODO]
 * - @a -S, @a --suffix: Override the usual backup suffix [TODO]
 * - @a -t, @a --target-directory: Move all SOURCE arguments into DIRECTORY [IMPLEMENTED]
 * - @a -T, @a --no-target-directory: Treat DEST as a normal file [IMPLEMENTED]
 * - @a -u: Move only when the SOURCE file is newer than the destination file or when the destination file is missing [TODO]
 * - @a -v, @a --verbose: Explain what is being done [IMPLEMENTED]
 * - @a -Z, @a --context: Set SELinux security context of destination file to default type [TODO]
 * - @a --backup: Make a backup of each existing destination file [TODO]
 * - @a --force: Do not prompt before overwriting [IMPLEMENTED]
 * - @a --interactive: Prompt according to WHEN: never, once (-I), or always (-i) [IMPLEMENTED]
 * - @a --no-clobber: Do not overwrite an existing file [IMPLEMENTED]
 * - @a --suffix: Override the usual backup suffix [TODO]
 * - @a --target-directory: Move all SOURCE arguments into DIRECTORY [IMPLEMENTED]
 * - @a --no-target-directory: Treat DEST as a normal file [IMPLEMENTED]
 * - @a --update: Move only when the SOURCE file is newer than the destination file or when the destination file is missing [TODO]
 * - @a --verbose: Explain what is being done [IMPLEMENTED]
 * - @a --context: Set SELinux security context of destination file to default type [TODO]
 * - @a --help: Display this help and exit [TODO]
 * - @a --version: Output version information and exit [TODO]
 */
constexpr auto MV_OPTIONS = std::array{
    OPTION("-b", "", "like --backup but does not accept an argument"),
    OPTION("-f", "--force", "do not prompt before overwriting"),
    OPTION("-i", "", "prompt before overwrite"),
    OPTION("-n", "--no-clobber", "do not overwrite an existing file"),
    OPTION("--strip-trailing-slashes", "",
           "remove any trailing slashes from each SOURCE argument"),
    OPTION("-S", "--suffix", "override the usual backup suffix"),
    OPTION("-t", "--target-directory",
           "move all SOURCE arguments into DIRECTORY"),
    OPTION("-T", "--no-target-directory", "treat DEST as a normal file"),
    OPTION("-u", "",
           "move only when the SOURCE file is newer than the destination file "
           "or when the destination file is missing"),
    OPTION("-v", "--verbose", "explain what is being done"),
    OPTION("-Z", "--context",
           "set SELinux security context of destination file to default type"),
    OPTION("--backup", "", "make a backup of each existing destination file"),
    OPTION("--force", "", "do not prompt before overwriting"),
    OPTION("--interactive", "",
           "prompt according to WHEN: never, once (-I), or always (-i)"),
    OPTION("--no-clobber", "", "do not overwrite an existing file"),
    OPTION("--suffix", "", "override the usual backup suffix"),
    OPTION("--target-directory", "",
           "move all SOURCE arguments into DIRECTORY"),
    OPTION("--no-target-directory", "", "treat DEST as a normal file"),
    OPTION("--update", "",
           "move only when the SOURCE file is newer than the destination file "
           "or when the destination file is missing"),
    OPTION("--verbose", "", "explain what is being done"),
    OPTION("--context", "",
           "set SELinux security context of destination file to default type"),
    OPTION("--help", "", "display this help and exit"),
    OPTION("--version", "", "output version information and exit")};

// Auto-generated lookup table for options from MV_OPTIONS
constexpr auto OPTION_HANDLERS =
    generate_option_handlers(MV_OPTIONS, "--interactive");

REGISTER_COMMAND(
    mv,
    /* cmd_name */ "mv",
    /* cmd_synopsis */ "move (rename) files",
    /* cmd_desc */
    "Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options "
    "too.\n",
    /* examples */
    "  mv file1 file2            Rename file1 to file2\n"
    "  mv file1 file2 dir        Move file1 and file2 to directory dir\n"
    "  mv -i file1 file2         Prompt before overwriting file2\n"
    "  mv -v file1 file2         Verbose output\n",
    /* see_also */ "cp(1), rm(1), ln(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    MV_OPTIONS) {
  // Option flags for mv command
  CREATE_AUTO_FLAGS_CLASS(
      MvOptions, DECLARE_STRING_OPTION(backup_suffix, "~")  // --suffix
      DECLARE_STRING_OPTION(target_dir, "")  // --target-directory
      DEFINE_FLAG(interactive, 0)            // -i, --interactive
      DEFINE_FLAG(verbose, 1)                // -v, --verbose
      DEFINE_FLAG(force, 2)                  // -f, --force
      DEFINE_FLAG(no_clobber, 3)             // --no-clobber
      DEFINE_FLAG(backup, 4)                 // -b
      DEFINE_FLAG(suffix, 5)                 // --suffix
      DEFINE_FLAG(target_directory, 6)       // --target-directory
      DEFINE_FLAG(no_target_directory, 7)    // --no-target-directory
      DEFINE_FLAG(update, 8)                 // -u
      DEFINE_FLAG(context, 9)                // -Z
  )

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
          options.set_interactive(true);
        } else if (arg == "--verbose") {
          options.set_verbose(true);
        } else if (arg == "--force") {
          options.set_force(true);
        } else if (arg == "--no-clobber") {
          options.set_no_clobber(true);
        } else if (arg == "--suffix") {
          if (i + 1 < args.size()) {
            options.set_backup_suffix(args[i + 1]);
            options.set_suffix(true);
            ++i;
          } else {
            fwprintf(stderr, L"mv: option '--suffix' requires an argument\n");
            return false;
          }
        } else if (arg == "--target-directory") {
          if (i + 1 < args.size()) {
            options.set_target_dir(args[i + 1]);
            options.set_target_directory(true);
            ++i;
          } else {
            fwprintf(stderr,
                     L"mv: option '--target-directory' requires an argument\n");
            return false;
          }
        } else if (arg == "--no-target-directory") {
          options.set_no_target_directory(true);
        } else if (arg == "--context") {
          options.set_context(true);
        } else {
          fwprintf(stderr, L"mv: invalid option -- '%.*s'\n",
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
          char opt_char = arg[j];
          bool found = false;

          for (const auto& handler : OPTION_HANDLERS) {
            if (handler.short_opt == opt_char) {
              if (handler.requires_arg) {
                if (j + 1 < arg.size()) {
                  if (opt_char == 'S') {
                    options.set_backup_suffix(arg.substr(j + 1));
                    options.set_suffix(true);
                  } else if (opt_char == 't') {
                    options.set_target_dir(arg.substr(j + 1));
                    options.set_target_directory(true);
                  }
                  j = arg.size() - 1;
                } else if (i + 1 < args.size()) {
                  if (opt_char == 'S') {
                    options.set_backup_suffix(args[i + 1]);
                    options.set_suffix(true);
                  } else if (opt_char == 't') {
                    options.set_target_dir(args[i + 1]);
                    options.set_target_directory(true);
                  }
                  ++i;
                } else {
                  fwprintf(stderr, L"mv: option requires an argument -- '%c'\n",
                           opt_char);
                  return false;
                }
              } else {
                // Set boolean option based on opt_char
                switch (opt_char) {
                  case 'i':
                    options.set_interactive(true);
                    break;
                  case 'v':
                    options.set_verbose(true);
                    break;
                  case 'b':
                    options.set_backup(true);
                    break;
                  case 'f':
                    options.set_force(true);
                    break;
                  case 'n':
                    options.set_no_clobber(true);
                    break;
                  case 'T':
                    options.set_no_target_directory(true);
                    break;
                  case 'u':
                    options.set_update(true);
                    break;
                  case 'Z':
                    options.set_context(true);
                    break;
                }
              }
              found = true;
              break;
            }
          }

          if (!found) {
            fwprintf(stderr, L"mv: invalid option -- '%c'\n", opt_char);
            return false;
          }
        }
      } else {
        // This is a path
        sourcePaths.push_back(std::string(arg));
      }
    }

    if (options.get_target_directory()) {
      if (sourcePaths.empty()) {
        fwprintf(stderr, L"mv: missing file operand\n");
        return false;
      }
      destPath = options.get_target_dir();
    } else {
      if (sourcePaths.size() < 2) {
        fwprintf(stderr, L"mv: missing file operand\n");
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

      if (options.get_interactive()) {
        if (std::filesystem::exists(destFsPath)) {
          wprintf(L"mv: overwrite '%s'? (y/n) ", destPath.c_str());
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

      if (options.get_verbose()) {
        wprintf(L"'%s' -> '%s'\n", srcPath.data(), destPath.c_str());
      }

      return true;
    } catch (const std::filesystem::filesystem_error& e) {
      fwprintf(stderr, L"mv: cannot move '%s' to '%s': %s\n", srcPath.data(),
               destPath.c_str(), e.what());
      return false;
    } catch (const std::exception& e) {
      fwprintf(stderr, L"mv: error: %s\n", e.what());
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
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
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
    fwprintf(stderr, L"mv: target '%s' is not a directory\n", destPath.c_str());
    return 1;
  }

  bool success = true;

  for (const auto& srcPath : sourcePaths) {
    if (!pathExists(srcPath)) {
      fwprintf(stderr, L"mv: cannot stat '%s': No such file or directory\n",
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

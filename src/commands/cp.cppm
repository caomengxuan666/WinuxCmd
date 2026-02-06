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
 *  - File: cp.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implemention for cp.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

module;
#include "pch/pch.h"
#pragma comment(lib, "shlwapi.lib")
#include "core/auto_flags.h"
#include "core/command_macros.h"
export module commands.cp;

import std;
import core;
import utils;
/**
 * @brief CP command options definition
 *
 * This array defines all the options supported by the cp command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -a, @a --archive: Same as -dR --preserve=all [TODO]
 * - @a -b: Like --backup but does not accept an argument [TODO]
 * - @a -d: Same as --no-dereference --preserve=links [TODO]
 * - @a -f, @a --force: If an existing destination file cannot be opened, remove
 * it and try again [TODO]
 * - @a -i, @a --interactive: Prompt before overwrite [IMPLEMENTED]
 * - @a -H: Follow command-line symbolic links in SOURCE [TODO]
 * - @a -l, @a --link: Hard link files instead of copying [TODO]
 * - @a -L, @a --dereference: Always follow symbolic links in SOURCE [TODO]
 * - @a -n, @a --no-clobber: Do not overwrite an existing file and do not fail
 * [TODO]
 * - @a -P, @a --no-dereference: Never follow symbolic links in SOURCE [TODO]
 * - @a -p: Same as --preserve=mode,ownership,timestamps [TODO]
 * - @a -R, @a --recursive: Copy directories recursively [IMPLEMENTED]
 * - @a -r, @a --recursive: Copy directories recursively [IMPLEMENTED]
 * - @a -s, @a --symbolic-link: Make symbolic links instead of copying [TODO]
 * - @a -S, @a --suffix: Override the usual backup suffix [TODO]
 * - @a -t, @a --target-directory: Copy all SOURCE arguments into DIRECTORY
 * [IMPLEMENTED]
 * - @a -T, @a --no-target-directory: Treat DEST as a normal file [TODO]
 * - @a -u: Equivalent to --update[=older] [TODO]
 * - @a -v, @a --verbose: Explain what is being done [IMPLEMENTED]
 * - @a -x, @a --one-file-system: Stay on this file system [TODO]
 * - @a -Z: Set SELinux security context of destination file to default type
 * [TODO]
 */
constexpr auto CP_OPTIONS = std::array{
    OPTION("-a", "--archive", "same as -dR --preserve=all"),
    OPTION("-b", "", "like --backup but does not accept an argument"),
    OPTION("-d", "", "same as --no-dereference --preserve=links"),
    OPTION("-f", "--force",
           "if an existing destination file cannot be opened, remove it and "
           "try again"),
    OPTION("-i", "--interactive", "prompt before overwrite"),
    OPTION("-H", "", "follow command-line symbolic links in SOURCE"),
    OPTION("-l", "--link", "hard link files instead of copying"),
    OPTION("-L", "--dereference", "always follow symbolic links in SOURCE"),
    OPTION("-n", "--no-clobber",
           "do not overwrite an existing file and do not fail"),
    OPTION("-P", "--no-dereference", "never follow symbolic links in SOURCE"),
    OPTION("-p", "", "same as --preserve=mode,ownership,timestamps"),
    OPTION("-R", "--recursive", "copy directories recursively"),
    OPTION("-r", "--recursive", "copy directories recursively"),
    OPTION("-s", "--symbolic-link", "make symbolic links instead of copying"),
    OPTION("-S", "--suffix", "override the usual backup suffix"),
    OPTION("-t", "--target-directory",
           "copy all SOURCE arguments into DIRECTORY"),
    OPTION("-T", "--no-target-directory", "treat DEST as a normal file"),
    OPTION("-u", "", "equivalent to --update[=older]"),
    OPTION("-v", "--verbose", "explain what is being done"),
    OPTION("-x", "--one-file-system", "stay on this file system"),
    OPTION("-Z", "",
           "set SELinux security context of destination file to default type")};

// Auto-generated lookup table for options from CP_OPTIONS
constexpr auto OPTION_HANDLERS =
    generate_option_handlers(CP_OPTIONS, "--suffix", "--target-directory");

CREATE_AUTO_FLAGS_CLASS(
    CpOptions, DECLARE_STRING_OPTION(backup_suffix, "~")  // --suffix
    DECLARE_STRING_OPTION(target_dir, "")                 // --target-directory

    // Define all flags
    DEFINE_FLAG(recursive, 0)             // -r, -R, --recursive
    DEFINE_FLAG(interactive, 1)           // -i, --interactive
    DEFINE_FLAG(verbose, 2)               // -v, --verbose
    DEFINE_FLAG(archive, 3)               // -a, --archive
    DEFINE_FLAG(backup, 4)                // -b
    DEFINE_FLAG(no_dereference, 5)        // -d, --no-dereference
    DEFINE_FLAG(force, 6)                 // -f, --force
    DEFINE_FLAG(follow_symlinks, 7)       // -H
    DEFINE_FLAG(hard_link, 8)             // -l, --link
    DEFINE_FLAG(always_dereference, 9)    // -L, --dereference
    DEFINE_FLAG(no_clobber, 10)           // -n, --no-clobber
    DEFINE_FLAG(preserve, 11)             // -p
    DEFINE_FLAG(symbolic_link, 12)        // -s, --symbolic-link
    DEFINE_FLAG(suffix, 13)               // -S, --suffix
    DEFINE_FLAG(target_directory, 14)     // -t, --target-directory
    DEFINE_FLAG(no_target_directory, 15)  // -T, --no-target-directory
    DEFINE_FLAG(update, 16)               // -u
    DEFINE_FLAG(one_file_system, 17)      // -x, --one-file-system
    DEFINE_FLAG(selinux_context, 18)      // -Z
)

REGISTER_COMMAND(
    cp,
    /* cmd_name */ "cp",

    /* cmd_synopsis */ "copy files and directories",

    /* cmd_desc */
    "Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n"
    "\n"
    "In the first form, copy SOURCE to DEST.\n"
    "In the second form, copy each SOURCE to DIRECTORY.\n",

    /* examples */
    "  cp file1.txt file2.txt       Copy file1.txt to file2.txt\n"
    "  cp -r dir1 dir2              Recursively copy dir1 to dir2\n"
    "  cp -v file.txt dir/           Verbose copy file.txt to dir/\n"
    "  cp -i file.txt file.txt       Interactive copy (prompt before "
    "overwrite)",

    /* see_also */ "mv(1), rm(1), ln(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    CP_OPTIONS) {
  /**
   * @brief Parse command line options for cp
   * @param args Command
   * arguments
   * @param options Output parameter for parsed options
   *
   * @param sourcePaths Output parameter for source paths
   * @param destPath
   * Output parameter for destination path
   * @return true if parsing
   * succeeded, false on error
   */
  auto parseCpOptions = [](std::span<std::string_view> args, CpOptions &options,
                           std::vector<std::string> &sourcePaths,
                           std::string &destPath) -> bool {
    // Helper function to find option handler by long option or short option
    auto find_handler = [](std::string_view arg,
                           char opt_char = '\0') -> const OptionHandler * {
      for (const auto &handler : OPTION_HANDLERS) {
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
        std::wstring warg(arg.begin(), arg.end());
        safeErrorPrintLn(L"cp: invalid option -- '" + warg.substr(2) + L"'");
      } else {
        safeErrorPrintLn(L"cp: invalid option -- '" +
                         std::wstring(1, opt_char) + L"'");
      }
    };

    // Helper function to handle argument options
    auto handle_arg_option = [&options, &args](char opt_char, size_t &i,
                                               size_t j,
                                               std::string_view arg) -> bool {
      if (j + 1 < arg.size()) {
        // Argument is part of the same string
        if (opt_char == 'S') {
          options.set_backup_suffix(arg.substr(j + 1));
          options.set_suffix(true);
        } else if (opt_char == 't') {
          options.set_target_dir(arg.substr(j + 1));
          options.set_target_directory(true);
        }
      } else if (i + 1 < args.size()) {
        // Argument is the next string
        if (opt_char == 'S') {
          options.set_backup_suffix(args[i + 1]);
          options.set_suffix(true);
        } else if (opt_char == 't') {
          options.set_target_dir(args[i + 1]);
          options.set_target_directory(true);
        }
        ++i;
      } else {
        // No argument provided
        std::wostringstream oss;
        oss << L"cp: option requires an argument -- '" << opt_char << L"'";
        safeErrorPrintLn(oss.str());
        return false;
      }
      return true;
    };

    // Helper function to set boolean option
    auto set_boolean_option = [&options](char opt_char) {
      switch (opt_char) {
        case 'r':
        case 'R':
          options.set_recursive(true);
          break;
        OPTION_CASE('i', interactive)
          break;
        OPTION_CASE('v', verbose)
          break;
        OPTION_CASE('a', archive)
          break;
        OPTION_CASE('b', backup)
          break;
        OPTION_CASE('d', no_dereference)
          break;
        OPTION_CASE('f', force)
          break;
        OPTION_CASE('H', follow_symlinks)
          break;
        OPTION_CASE('l', hard_link)
          break;
        OPTION_CASE('L', always_dereference)
          break;
        OPTION_CASE('n', no_clobber)
          break;
        case 'P':
          options.set_no_dereference(true);
          break;  // Note: -P sets no_dereference, same as -d
        OPTION_CASE('p', preserve)
          break;
        OPTION_CASE('s', symbolic_link)
          break;
        OPTION_CASE('T', no_target_directory)
          break;
        OPTION_CASE('u', update)
          break;
        OPTION_CASE('x', one_file_system)
          break;
        OPTION_CASE('Z', selinux_context)
          break;
      }
    };

    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        const auto *handler = find_handler(arg);
        if (handler) {
          if (handler->requires_arg) {
            // Handle options that require arguments
            if (i + 1 < args.size()) {
              if (handler->short_opt == 'S') {
                options.set_backup_suffix(args[i + 1]);
                options.set_suffix(true);
              } else if (handler->short_opt == 't') {
                options.set_target_dir(args[i + 1]);
                options.set_target_directory(true);
              }
              ++i;
            } else {
              std::wostringstream oss;
              oss << L"cp: option '" << arg.data() << L"' requires an argument";
              safeErrorPrintLn(oss.str());
              return false;
            }
          } else {
            set_boolean_option(handler->short_opt);
          }
        } else {
          print_option_error(arg);
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
          const auto *handler = find_handler("", opt_char);
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
        sourcePaths.push_back(std::string(arg));
      }
    }

    if (options.get_target_directory()) {
      if (sourcePaths.empty()) {
        safeErrorPrintLn(L"cp: missing file operand");
        return false;
      }
      destPath = options.get_target_dir();
    } else {
      if (sourcePaths.size() < 2) {
        safeErrorPrintLn(L"cp: missing file operand");
        return false;
      }
      destPath = sourcePaths.back();
      sourcePaths.pop_back();
    }

    return true;
  };

  /**
   * @brief Create directory recursively
   * @param path Directory path to create
   * @return true if directory was created successfully, false on error
   */
  std::function<bool(const std::string &)> createDirectoryRecursive;
  createDirectoryRecursive =
      [&createDirectoryRecursive](const std::string &path) -> bool {
    std::wstring wpath(path.begin(), path.end());
    if (CreateDirectoryW(wpath.c_str(), NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS) {
      return true;
    }
    // If parent directory doesn't exist, create it first
    size_t lastSlash = path.find_last_of('\\');
    if (lastSlash == std::string::npos) {
      return false;
    }
    std::string parentPath = path.substr(0, lastSlash);
    if (!createDirectoryRecursive(parentPath)) {
      return false;
    }
    // Now create the current directory
    return CreateDirectoryW(wpath.c_str(), NULL) != 0;
  };

  /**
   * @brief Recursive function to copy directory
   * @param srcPath Source directory path
   * @param destPath Destination directory path
   * @param options cp command options
   * @return true if directory was copied successfully, false on error
   */
  std::function<bool(const std::string &, const std::string &,
                     const CpOptions &)>
      copyDirectory;
  copyDirectory = [&copyDirectory, &createDirectoryRecursive](
                      const std::string &srcPath, const std::string &destPath,
                      const CpOptions &options) -> bool {
    // Create destination directory if it doesn't exist
    if (!createDirectoryRecursive(destPath)) {
      std::wstring wdestPath = utf8_to_wstring(destPath);
      safeErrorPrintLn(L"cp: cannot create directory '" + wdestPath + L"'");
      return false;
    }

    // Open source directory
    std::wstring wsrcPath(srcPath.begin(), srcPath.end());
    std::wstring searchPath = wsrcPath + L"\\*";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
      std::wstring wsrcPath = utf8_to_wstring(srcPath);
      safeErrorPrintLn(L"cp: cannot open directory '" + wsrcPath + L"'");
      return false;
    }

    bool success = true;

    // Process each item in the directory
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        // Skip . and ..
        if (!(wcscmp(findData.cFileName, L".") == 0 ||
              wcscmp(findData.cFileName, L"..") == 0)) {
          // Get the full path of the source file/directory
          int fileNameLength = WideCharToMultiByte(
              CP_UTF8, 0, findData.cFileName, -1, NULL, 0, NULL, NULL);
          std::string fileName(fileNameLength, 0);
          WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, &fileName[0],
                              fileNameLength, NULL, NULL);
          std::string srcItemPath = srcPath + "\\" + fileName;
          std::string destItemPath = destPath + "\\" + fileName;

          // Check if it's a directory
          if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recursively copy subdirectory
            if (!copyDirectory(srcItemPath, destItemPath, options)) {
              success = false;
            }
          } else {
            // Copy file
            try {
              if (options.get_interactive()) {
                std::ifstream destTest(destItemPath);
                if (destTest.good()) {
                  safeErrorPrint(L"cp: overwrite '" +
                                 utf8_to_wstring(destItemPath) + L"'? (y/n) ");
                  char response;
                  std::cin.get(response);
                  if (response != 'y' && response != 'Y') {
                    // Skip this file
                  } else {
                    // Copy the file
                    std::ifstream srcFile(srcItemPath, std::ios::binary);
                    if (!srcFile) {
                      safeErrorPrintLn(L"cp: cannot open '" +
                                       utf8_to_wstring(srcItemPath) +
                                       L"' for reading");
                      success = false;
                      continue;
                    }
                    std::ofstream destFile(destItemPath, std::ios::binary);
                    if (!destFile) {
                      safeErrorPrintLn(L"cp: cannot open '" +
                                       utf8_to_wstring(destItemPath) +
                                       L"' for writing");
                      success = false;
                      continue;
                    }
                    destFile << srcFile.rdbuf();
                    if (!destFile) {
                      safeErrorPrintLn(L"cp: error writing to '" +
                                       utf8_to_wstring(destItemPath) + L"'");
                      success = false;
                      continue;
                    }
                    if (options.get_verbose()) {
                      safePrintLn(L"'" + utf8_to_wstring(srcItemPath) +
                                  L"' -> '" + utf8_to_wstring(destItemPath) +
                                  L"'");
                    }
                  }
                } else {
                  // Destination doesn't exist, just copy
                  std::ifstream srcFile(srcItemPath, std::ios::binary);
                  if (!srcFile) {
                    std::wostringstream oss;
                    oss << L"cp: cannot open '" << srcItemPath.data()
                        << L"' for reading";
                    safeErrorPrintLn(oss.str());
                    success = false;
                    continue;
                  }
                  std::ofstream destFile(destItemPath, std::ios::binary);
                  if (!destFile) {
                    std::wostringstream oss;
                    oss << L"cp: cannot open '" << destItemPath.c_str()
                        << L"' for writing";
                    safeErrorPrintLn(oss.str());
                    success = false;
                    continue;
                  }
                  destFile << srcFile.rdbuf();
                  if (!destFile) {
                    std::wostringstream oss;
                    oss << L"cp: error writing to '" << destItemPath.c_str()
                        << L"'";
                    safeErrorPrintLn(oss.str());
                    success = false;
                    continue;
                  }
                  if (options.get_verbose()) {
                    safePrintLn(L"'" + utf8_to_wstring(srcItemPath) +
                                L"' -> '" + utf8_to_wstring(destItemPath) +
                                L"'");
                  }
                }
              } else {
                // Non-interactive mode, just copy
                std::ifstream srcFile(srcItemPath, std::ios::binary);
                if (!srcFile) {
                  std::wstring wpath_str = utf8_to_wstring(srcItemPath);
                  safeErrorPrintLn(L"cp: cannot open '" + wpath_str +
                                   L"' for reading");
                  success = false;
                  continue;
                }
                std::ofstream destFile(destItemPath, std::ios::binary);
                if (!destFile) {
                  safeErrorPrintLn(L"cp: cannot open '" +
                                   utf8_to_wstring(destItemPath) +
                                   L"' for writing");
                  success = false;
                  continue;
                }
                destFile << srcFile.rdbuf();
                if (!destFile) {
                  std::wstring wpath_str = utf8_to_wstring(destItemPath);
                  safeErrorPrintLn(L"cp: error writing to '" + wpath_str +
                                   L"'");
                  success = false;
                  continue;
                }
                if (options.get_verbose()) {
                  safePrintLn(L"'" + utf8_to_wstring(srcItemPath) + L"' -> '" +
                              utf8_to_wstring(destItemPath) + L"'");
                }
              }
            } catch (const std::exception &e) {
              std::wostringstream oss;
              oss << L"cp: error copying '" << srcItemPath.data() << L"' to '"
                  << destItemPath.c_str() << L"': " << e.what();
              safeErrorPrintLn(oss.str());
              success = false;
            }
          }
        }
      } while (FindNextFileW(hFind, &findData));
    }

    FindClose(hFind);
    return success;
  };

  /**
   * @brief Copy a single file
   * @param srcPath Source file path
   * @param destPath Destination file path
   * @param options cp command options
   * @return true if file was copied successfully, false on error
   */
  auto copyFile = [](const std::string &srcPath, const std::string &destPath,
                     const CpOptions &options) -> bool {
    try {
      if (options.get_interactive()) {
        std::ifstream destTest(destPath);
        if (destTest.good()) {
          safeErrorPrint(L"cp: overwrite '" + utf8_to_wstring(destPath) +
                         L"'? (y/n) ");
          char response;
          std::cin.get(response);
          if (response != 'y' && response != 'Y') {
            return true;
          }
        }
      }
      std::ifstream src(srcPath, std::ios::binary);
      if (!src) {
        safeErrorPrintLn(L"cp: cannot open '" + utf8_to_wstring(srcPath) +
                         L"' for reading");
        return false;
      }
      std::ofstream dest(destPath, std::ios::binary);
      if (!dest) {
        safeErrorPrintLn(L"cp: cannot open '" + utf8_to_wstring(destPath) +
                         L"' for writing");
        return false;
      }
      dest << src.rdbuf();
      if (!dest) {
        safeErrorPrintLn(L"cp: error writing to '" + utf8_to_wstring(destPath) +
                         L"'");
        return false;
      }
      if (options.get_verbose()) {
        safePrintLn(L"'" + utf8_to_wstring(srcPath) + L"' -> '" +
                    utf8_to_wstring(destPath) + L"'");
      }
      return true;
    } catch (const std::exception &e) {
      std::wostringstream oss;
      oss << L"cp: error copying '" << srcPath.data() << L"' to '"
          << destPath.c_str() << L"': " << e.what();
      safeErrorPrintLn(oss.str());
      return false;
    }
  };

  /**
   * @brief Check if a path exists and is a directory
   * @param path Path to check
   * @return true if path exists and is a directory, false otherwise
   */
  auto pathExistsAndIsDirectory = [](const std::string &path) -> bool {
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
  auto pathExists = [](const std::string &path) -> bool {
    std::wstring wpath(path.begin(), path.end());
    DWORD attr = GetFileAttributesW(wpath.c_str());
    return attr != INVALID_FILE_ATTRIBUTES;
  };

  // Main implementation
  CpOptions options;
  std::vector<std::string> sourcePaths;
  std::string destPath;

  if (!parseCpOptions(args, options, sourcePaths, destPath)) {
    return 2;  // Invalid option error code
  }

  bool destIsDir = pathExistsAndIsDirectory(destPath);

  if (sourcePaths.size() > 1 && !destIsDir) {
    std::wostringstream oss;
    oss << L"cp: target '" << destPath.c_str() << L"' is not a directory";
    safeErrorPrintLn(oss.str());
    return 1;
  }

  bool success = true;

  for (const auto &srcPath : sourcePaths) {
    if (!pathExists(srcPath)) {
      std::wostringstream oss;
      oss << L"cp: cannot stat '" << srcPath.data()
          << L"': No such file or directory";
      safeErrorPrintLn(oss.str());
      success = false;
      continue;
    }

    bool srcIsDir = pathExistsAndIsDirectory(srcPath);
    std::string finalDestPath = destPath;

    if (destIsDir) {
      // If destination is a directory, append source filename
      std::wstring wsrcPath(srcPath.begin(), srcPath.end());
      LPWSTR fileName = PathFindFileNameW(wsrcPath.c_str());
      int fileNameLength =
          WideCharToMultiByte(CP_UTF8, 0, fileName, -1, NULL, 0, NULL, NULL);
      std::string fileNameStr(fileNameLength, 0);
      WideCharToMultiByte(CP_UTF8, 0, fileName, -1, &fileNameStr[0],
                          fileNameLength, NULL, NULL);
      finalDestPath += "\\" + fileNameStr;
    }

    if (srcIsDir) {
      if (options.get_recursive()) {
        if (!copyDirectory(srcPath, finalDestPath, options)) {
          success = false;
        }
      } else {
        std::wstring wpath_str = utf8_to_wstring(std::string(srcPath));
        safeErrorPrintLn(L"cp: omitting directory '" + wpath_str + L"'");
        success = false;
      }
    } else {
      if (!copyFile(srcPath, finalDestPath, options)) {
        success = false;
      }
    }
  }

  return success ? 0 : 1;
}

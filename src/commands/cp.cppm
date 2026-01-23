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
// Use global module fragment for C standard library
module;
#include <cstdio>
#define WIN32_LEAN_AND_MEAN
#include <shlwapi.h>
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")
#include "core/command_macros.h"
export module commands.cp;

import std;
import core.dispatcher;
import core.cmd_meta;

/**
 * @brief Copy files and directories
 * @param args Command-line arguments
 * @return Exit code (0 on success, non-zero on error)
 *
 * Options:
 *  -r, --recursive          Copy directories recursively
 *  -i, --interactive        Prompt before overwrite
 *  -v, --verbose            Explain what is being done
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
  // Option flags for cp command
  struct CpOptions {
    bool recursive = false;            // -r, -R, --recursive
    bool interactive = false;          // -i, --interactive
    bool verbose = false;              // -v, --verbose
    bool archive = false;              // -a, --archive
    bool backup = false;               // -b
    bool no_dereference = false;       // -d, --no-dereference
    bool force = false;                // -f, --force
    bool follow_symlinks = false;      // -H
    bool hard_link = false;            // -l, --link
    bool always_dereference = false;   // -L, --dereference
    bool no_clobber = false;           // -n, --no-clobber
    bool preserve = false;             // -p
    bool symbolic_link = false;        // -s, --symbolic-link
    bool suffix = false;               // -S, --suffix
    bool target_directory = false;     // -t, --target-directory
    bool no_target_directory = false;  // -T, --no-target-directory
    bool update = false;               // -u
    bool one_file_system = false;      // -x, --one-file-system
    bool selinux_context = false;      // -Z
    std::string backup_suffix = "~";   // --suffix
    std::string target_dir;            // --target-directory
  };

  /**
   * @brief Parse command line options for cp
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param sourcePaths Output parameter for source paths
   * @param destPath Output parameter for destination path
   * @return true if parsing succeeded, false on error
   */
  auto parseCpOptions = [](std::span<std::string_view> args, CpOptions& options,
                           std::vector<std::string>& sourcePaths,
                           std::string& destPath) -> bool {
    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        if (arg == "--recursive") {
          options.recursive = true;
        } else if (arg == "--interactive") {
          options.interactive = true;
        } else if (arg == "--verbose") {
          options.verbose = true;
        } else if (arg == "--archive") {
          options.archive = true;
        } else if (arg == "--force") {
          options.force = true;
        } else if (arg == "--link") {
          options.hard_link = true;
        } else if (arg == "--dereference") {
          options.always_dereference = true;
        } else if (arg == "--no-clobber") {
          options.no_clobber = true;
        } else if (arg == "--no-dereference") {
          options.no_dereference = true;
        } else if (arg == "--symbolic-link") {
          options.symbolic_link = true;
        } else if (arg == "--suffix") {
          if (i + 1 < args.size()) {
            options.backup_suffix = args[i + 1];
            options.suffix = true;
            ++i;
          } else {
            fprintf(stderr, "cp: option '--suffix' requires an argument\n");
            return false;
          }
        } else if (arg == "--target-directory") {
          if (i + 1 < args.size()) {
            options.target_dir = args[i + 1];
            options.target_directory = true;
            ++i;
          } else {
            fprintf(stderr,
                    "cp: option '--target-directory' requires an argument\n");
            return false;
          }
        } else if (arg == "--no-target-directory") {
          options.no_target_directory = true;
        } else if (arg == "--one-file-system") {
          options.one_file_system = true;
        } else {
          fprintf(stderr, "cp: invalid option -- '%.*s'\n",
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
            case 'r':
            case 'R':
              options.recursive = true;
              break;
            case 'i':
              options.interactive = true;
              break;
            case 'v':
              options.verbose = true;
              break;
            case 'a':
              options.archive = true;
              break;
            case 'b':
              options.backup = true;
              break;
            case 'd':
              options.no_dereference = true;
              break;
            case 'f':
              options.force = true;
              break;
            case 'H':
              options.follow_symlinks = true;
              break;
            case 'l':
              options.hard_link = true;
              break;
            case 'L':
              options.always_dereference = true;
              break;
            case 'n':
              options.no_clobber = true;
              break;
            case 'P':
              options.no_dereference = true;
              break;
            case 'p':
              options.preserve = true;
              break;
            case 's':
              options.symbolic_link = true;
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
                fprintf(stderr, "cp: option requires an argument -- 'S'\n");
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
                fprintf(stderr, "cp: option requires an argument -- 't'\n");
                return false;
              }
            case 'T':
              options.no_target_directory = true;
              break;
            case 'u':
              options.update = true;
              break;
            case 'x':
              options.one_file_system = true;
              break;
            case 'Z':
              options.selinux_context = true;
              break;
            default:
              fprintf(stderr, "cp: invalid option -- '%c'\n", arg[j]);
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
        fprintf(stderr, "cp: missing file operand\n");
        return false;
      }
      destPath = options.target_dir;
    } else {
      if (sourcePaths.size() < 2) {
        fprintf(stderr, "cp: missing file operand\n");
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
  std::function<bool(const std::string&)> createDirectoryRecursive;
  createDirectoryRecursive =
      [&createDirectoryRecursive](const std::string& path) -> bool {
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
  std::function<bool(const std::string&, const std::string&, const CpOptions&)>
      copyDirectory;
  copyDirectory = [&copyDirectory, &createDirectoryRecursive](
                      const std::string& srcPath, const std::string& destPath,
                      const CpOptions& options) -> bool {
    // Create destination directory if it doesn't exist
    if (!createDirectoryRecursive(destPath)) {
      fprintf(stderr, "cp: cannot create directory '%s'\n", destPath.c_str());
      return false;
    }

    // Open source directory
    std::wstring wsrcPath(srcPath.begin(), srcPath.end());
    std::wstring searchPath = wsrcPath + L"\\*";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
      fprintf(stderr, "cp: cannot open directory '%s'\n", srcPath.c_str());
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
              if (options.interactive) {
                std::ifstream destTest(destItemPath);
                if (destTest.good()) {
                  printf("cp: overwrite '%s'? (y/n) ", destItemPath.c_str());
                  char response;
                  std::cin.get(response);
                  if (response != 'y' && response != 'Y') {
                    // Skip this file
                  } else {
                    // Copy the file
                    std::ifstream srcFile(srcItemPath, std::ios::binary);
                    if (!srcFile) {
                      fprintf(stderr, "cp: cannot open '%s' for reading\n",
                              srcItemPath.data());
                      success = false;
                      continue;
                    }
                    std::ofstream destFile(destItemPath, std::ios::binary);
                    if (!destFile) {
                      fprintf(stderr, "cp: cannot open '%s' for writing\n",
                              destItemPath.c_str());
                      success = false;
                      continue;
                    }
                    destFile << srcFile.rdbuf();
                    if (!destFile) {
                      fprintf(stderr, "cp: error writing to '%s'\n",
                              destItemPath.c_str());
                      success = false;
                      continue;
                    }
                    if (options.verbose) {
                      printf("'%s' -> '%s'\n", srcItemPath.data(),
                             destItemPath.c_str());
                    }
                  }
                } else {
                  // Destination doesn't exist, just copy
                  std::ifstream srcFile(srcItemPath, std::ios::binary);
                  if (!srcFile) {
                    fprintf(stderr, "cp: cannot open '%s' for reading\n",
                            srcItemPath.data());
                    success = false;
                    continue;
                  }
                  std::ofstream destFile(destItemPath, std::ios::binary);
                  if (!destFile) {
                    fprintf(stderr, "cp: cannot open '%s' for writing\n",
                            destItemPath.c_str());
                    success = false;
                    continue;
                  }
                  destFile << srcFile.rdbuf();
                  if (!destFile) {
                    fprintf(stderr, "cp: error writing to '%s'\n",
                            destItemPath.c_str());
                    success = false;
                    continue;
                  }
                  if (options.verbose) {
                    printf("'%s' -> '%s'\n", srcItemPath.data(),
                           destItemPath.c_str());
                  }
                }
              } else {
                // Non-interactive mode, just copy
                std::ifstream srcFile(srcItemPath, std::ios::binary);
                if (!srcFile) {
                  fprintf(stderr, "cp: cannot open '%s' for reading\n",
                          srcItemPath.data());
                  success = false;
                  continue;
                }
                std::ofstream destFile(destItemPath, std::ios::binary);
                if (!destFile) {
                  fprintf(stderr, "cp: cannot open '%s' for writing\n",
                          destItemPath.c_str());
                  success = false;
                  continue;
                }
                destFile << srcFile.rdbuf();
                if (!destFile) {
                  fprintf(stderr, "cp: error writing to '%s'\n",
                          destItemPath.c_str());
                  success = false;
                  continue;
                }
                if (options.verbose) {
                  printf("'%s' -> '%s'\n", srcItemPath.data(),
                         destItemPath.c_str());
                }
              }
            } catch (const std::exception& e) {
              fprintf(stderr, "cp: error copying '%s' to '%s': %s\n",
                      srcItemPath.data(), destItemPath.c_str(), e.what());
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
  auto copyFile = [](const std::string& srcPath, const std::string& destPath,
                     const CpOptions& options) -> bool {
    try {
      if (options.interactive) {
        std::ifstream destTest(destPath);
        if (destTest.good()) {
          printf("cp: overwrite '%s'? (y/n) ", destPath.c_str());
          char response;
          std::cin.get(response);
          if (response != 'y' && response != 'Y') {
            return true;
          }
        }
      }
      std::ifstream src(srcPath, std::ios::binary);
      if (!src) {
        fprintf(stderr, "cp: cannot open '%s' for reading\n", srcPath.data());
        return false;
      }
      std::ofstream dest(destPath, std::ios::binary);
      if (!dest) {
        fprintf(stderr, "cp: cannot open '%s' for writing\n", destPath.c_str());
        return false;
      }
      dest << src.rdbuf();
      if (!dest) {
        fprintf(stderr, "cp: error writing to '%s'\n", destPath.c_str());
        return false;
      }
      if (options.verbose) {
        printf("'%s' -> '%s'\n", srcPath.data(), destPath.c_str());
      }
      return true;
    } catch (const std::exception& e) {
      fprintf(stderr, "cp: error copying '%s' to '%s': %s\n", srcPath.data(),
              destPath.c_str(), e.what());
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
  CpOptions options;
  std::vector<std::string> sourcePaths;
  std::string destPath;

  if (!parseCpOptions(args, options, sourcePaths, destPath)) {
    return 2;  // Invalid option error code
  }

  bool destIsDir = pathExistsAndIsDirectory(destPath);

  if (sourcePaths.size() > 1 && !destIsDir) {
    fprintf(stderr, "cp: target '%s' is not a directory\n", destPath.c_str());
    return 1;
  }

  bool success = true;

  for (const auto& srcPath : sourcePaths) {
    if (!pathExists(srcPath)) {
      fprintf(stderr, "cp: cannot stat '%s': No such file or directory\n",
              srcPath.data());
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
      if (options.recursive) {
        if (!copyDirectory(srcPath, finalDestPath, options)) {
          success = false;
        }
      } else {
        fprintf(stderr, "cp: omitting directory '%s'\n", srcPath.data());
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

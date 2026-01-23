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
 *  - File: ls.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// Use global module fragment for C standard library
module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>

#include "core/command_macros.h"
export module commands.ls;

import std;
import core.dispatcher;
import core.cmd_meta;

/**
 * @brief List directory contents
 * @param args Command-line arguments
 * @return Exit code (0 on success, non-zero on error)
 *
 * Options:
 *  -l, --long-list          Use a long listing format
 *  -a, --all                Do not ignore entries starting with .
 *  -h, --human-readable     With -l, print sizes in human readable format
 *  -r, --reverse            Reverse order while sorting
 */

constexpr auto LS_OPTIONS = std::array{
    OPTION("-a", "--all", "do not ignore entries starting with ."),
    OPTION("-A", "--almost-all", "do not list implied . and .."),
    OPTION("-b", "--escape", "print C-style escapes for nongraphic characters"),
    OPTION("-B", "--ignore-backups",
           "do not list implied entries ending with ~"),
    OPTION("-c", "",
           "with -lt: sort by, and show, ctime; with -l: show ctime and sort "
           "by name; otherwise: sort by ctime, newest first"),
    OPTION("-C", "", "list entries by columns"),
    OPTION("-d", "--directory",
           "list directories themselves, not their contents"),
    OPTION("-f", "", "list all entries in directory order"),
    OPTION("-F", "--classify", "append indicator (one of */=>@|) to entries"),
    OPTION("-g", "", "like -l, but do not list owner"),
    OPTION("-h", "--human-readable",
           "with -l and -s, print sizes like 1K 234M 2G etc."),
    OPTION("-i", "--inode", "print the index number of each file"),
    OPTION("-k", "--kibibytes",
           "default to 1024-byte blocks for file system usage"),
    OPTION("-L", "--dereference",
           "when showing file information for a symbolic link, show "
           "information for the file the link references"),
    OPTION("-l", "--long-list", "use a long listing format"),
    OPTION("-m", "", "fill width with a comma separated list of entries"),
    OPTION("-n", "--numeric-uid-gid",
           "like -l, but list numeric user and group IDs"),
    OPTION("-N", "--literal", "print entry names without quoting"),
    OPTION("-o", "", "like -l, but do not list group information"),
    OPTION("-p", "--indicator-style=slash",
           "append / indicator to directories"),
    OPTION("-q", "--hide-control-chars",
           "print ? instead of nongraphic characters"),
    OPTION("-Q", "--quote-name", "enclose entry names in double quotes"),
    OPTION("-r", "--reverse", "reverse order while sorting"),
    OPTION("-R", "--recursive", "list subdirectories recursively"),
    OPTION("-s", "--size", "print the allocated size of each file, in blocks"),
    OPTION("-S", "", "sort by file size, largest first"),
    OPTION("-t", "", "sort by time, newest first"),
    OPTION("-T", "--tabsize", "assume tab stops at each COLS instead of 8"),
    OPTION(
        "-u", "",
        "with -lt: sort by, and show, access time; with -l: show access time "
        "and sort by name; otherwise: sort by access time, newest first"),
    OPTION("-U", "", "do not sort; list entries in directory order"),
    OPTION("-v", "", "natural sort of (version) numbers within text"),
    OPTION("-w", "--width", "set output width to COLS. 0 means no limit"),
    OPTION("-x", "", "list entries by lines instead of by columns"),
    OPTION("-X", "", "sort alphabetically by entry extension"),
    OPTION("-Z", "--context", "print any security context of each file"),
    OPTION("-1", "", "list one file per line")};

REGISTER_COMMAND(
    ls,
    /* cmd_name */ "ls",
    /* cmd_synopsis */ "list directory contents",
    /* cmd_desc */
    "List information about the FILEs (the current directory by default).\n"
    "Sort entries alphabetically if none of -cftuvSUX nor --sort is "
    "specified.\n"
    "\n"
    "With no FILE, list the current directory contents. With a FILE that is a\n"
    "directory, list the files and subdirectories inside that directory.\n"
    "With a FILE that is not a directory, list just that file.\n",
    /* examples */
    "  ls                      List files in current directory\n"
    "  ls -l                   Long listing format\n"
    "  ls -la                  Long listing format including hidden files\n"
    "  ls -lh                  Long listing format with human-readable sizes",
    /* see_also */ "find(1), grep(1), sort(1), wc(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    LS_OPTIONS) {
  // Option flags for ls command
  struct LsOptions {
    bool long_format = false;         // -l, --long-list
    bool show_all = false;            // -a, --all
    bool almost_all = false;          // -A, --almost-all
    bool escape = false;              // -b, --escape
    bool ignore_backups = false;      // -B, --ignore-backups
    bool sort_by_ctime = false;       // -c
    bool columns = false;             // -C
    bool directory = false;           // -d, --directory
    bool directory_order = false;     // -f
    bool classify = false;            // -F, --classify
    bool no_owner = false;            // -g
    bool human_readable = false;      // -h, --human-readable
    bool show_inode = false;          // -i, --inode
    bool kibibytes = false;           // -k, --kibibytes
    bool dereference = false;         // -L, --dereference
    bool comma_separated = false;     // -m
    bool numeric_uid_gid = false;     // -n, --numeric-uid-gid
    bool literal = false;             // -N, --literal
    bool no_group = false;            // -o
    bool indicator_slash = false;     // -p
    bool hide_control_chars = false;  // -q, --hide-control-chars
    bool quote_name = false;          // -Q, --quote-name
    bool reverse_order = false;       // -r, --reverse
    bool recursive = false;           // -R, --recursive
    bool show_size = false;           // -s, --size
    bool sort_by_size = false;        // -S
    bool sort_by_time = false;        // -t
    int tab_size = 8;                 // -T, --tabsize
    bool sort_by_atime = false;       // -u
    bool no_sort = false;             // -U
    bool natural_sort = false;        // -v
    int width = 0;                    // -w, --width
    bool lines = false;               // -x
    bool sort_by_extension = false;   // -X
    bool show_context = false;        // -Z, --context
    bool one_per_line = false;        // -1
  };

  /**
   * @brief Parse command line options for ls
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param paths Output parameter for paths to process
   * @return true if parsing succeeded, false on error
   */
  auto parseLsOptions = [](std::span<std::string_view> args, LsOptions& options,
                           std::vector<std::string_view>& paths) -> bool {
    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        if (arg == "--long-list") {
          options.long_format = true;
        } else if (arg == "--all") {
          options.show_all = true;
        } else if (arg == "--almost-all") {
          options.almost_all = true;
        } else if (arg == "--escape") {
          options.escape = true;
        } else if (arg == "--ignore-backups") {
          options.ignore_backups = true;
        } else if (arg == "--directory") {
          options.directory = true;
        } else if (arg == "--classify") {
          options.classify = true;
        } else if (arg == "--human-readable") {
          options.human_readable = true;
        } else if (arg == "--inode") {
          options.show_inode = true;
        } else if (arg == "--kibibytes") {
          options.kibibytes = true;
        } else if (arg == "--dereference") {
          options.dereference = true;
        } else if (arg == "--numeric-uid-gid") {
          options.numeric_uid_gid = true;
        } else if (arg == "--literal") {
          options.literal = true;
        } else if (arg == "--hide-control-chars") {
          options.hide_control_chars = true;
        } else if (arg == "--quote-name") {
          options.quote_name = true;
        } else if (arg == "--reverse") {
          options.reverse_order = true;
        } else if (arg == "--recursive") {
          options.recursive = true;
        } else if (arg == "--size") {
          options.show_size = true;
        } else if (arg == "--tabsize") {
          if (i + 1 < args.size()) {
            options.tab_size = std::stoi(std::string(args[i + 1]));
            ++i;
          } else {
            fprintf(stderr, "ls: option '--tabsize' requires an argument\n");
            return false;
          }
        } else if (arg == "--width") {
          if (i + 1 < args.size()) {
            options.width = std::stoi(std::string(args[i + 1]));
            ++i;
          } else {
            fprintf(stderr, "ls: option '--width' requires an argument\n");
            return false;
          }
        } else if (arg == "--context") {
          options.show_context = true;
        } else {
          fprintf(stderr, "ls: invalid option -- '%.*s'\n",
                  static_cast<int>(arg.size() - 2), arg.data() + 2);
          return false;
        }
      } else if (arg.starts_with('-')) {
        // This is a short option
        if (arg == "-") {
          // Single dash, treat as path
          paths.push_back(arg);
          continue;
        }

        // Process option characters
        for (size_t j = 1; j < arg.size(); ++j) {
          switch (arg[j]) {
            case 'l':
              options.long_format = true;
              break;
            case 'a':
              options.show_all = true;
              break;
            case 'A':
              options.almost_all = true;
              break;
            case 'b':
              options.escape = true;
              break;
            case 'B':
              options.ignore_backups = true;
              break;
            case 'c':
              options.sort_by_ctime = true;
              break;
            case 'C':
              options.columns = true;
              break;
            case 'd':
              options.directory = true;
              break;
            case 'f':
              options.directory_order = true;
              break;
            case 'F':
              options.classify = true;
              break;
            case 'g':
              options.no_owner = true;
              break;
            case 'h':
              options.human_readable = true;
              break;
            case 'i':
              options.show_inode = true;
              break;
            case 'k':
              options.kibibytes = true;
              break;
            case 'L':
              options.dereference = true;
              break;
            case 'm':
              options.comma_separated = true;
              break;
            case 'n':
              options.numeric_uid_gid = true;
              break;
            case 'N':
              options.literal = true;
              break;
            case 'o':
              options.no_group = true;
              break;
            case 'p':
              options.indicator_slash = true;
              break;
            case 'q':
              options.hide_control_chars = true;
              break;
            case 'Q':
              options.quote_name = true;
              break;
            case 'r':
              options.reverse_order = true;
              break;
            case 'R':
              options.recursive = true;
              break;
            case 's':
              options.show_size = true;
              break;
            case 'S':
              options.sort_by_size = true;
              break;
            case 't':
              options.sort_by_time = true;
              break;
            case 'T':
              if (j + 1 < arg.size()) {
                options.tab_size = std::stoi(std::string(arg.substr(j + 1)));
                j = arg.size() - 1;
              } else if (i + 1 < args.size()) {
                options.tab_size = std::stoi(std::string(args[i + 1]));
                ++i;
                break;
              } else {
                fprintf(stderr, "ls: option requires an argument -- 'T'\n");
                return false;
              }
            case 'u':
              options.sort_by_atime = true;
              break;
            case 'U':
              options.no_sort = true;
              break;
            case 'v':
              options.natural_sort = true;
              break;
            case 'w':
              if (j + 1 < arg.size()) {
                options.width = std::stoi(std::string(arg.substr(j + 1)));
                j = arg.size() - 1;
              } else if (i + 1 < args.size()) {
                options.width = std::stoi(std::string(args[i + 1]));
                ++i;
                break;
              } else {
                fprintf(stderr, "ls: option requires an argument -- 'w'\n");
                return false;
              }
            case 'x':
              options.lines = true;
              break;
            case 'X':
              options.sort_by_extension = true;
              break;
            case 'Z':
              options.show_context = true;
              break;
            case '1':
              options.one_per_line = true;
              break;
            default:
              fprintf(stderr, "ls: invalid option -- '%c'\n", arg[j]);
              return false;
          }
        }
      } else {
        // This is a path
        paths.push_back(arg);
      }
    }

    // Default to current directory if no paths specified
    if (paths.empty()) {
      paths.push_back(".");
    }

    return true;
  };

  /**
   * @brief List directory contents
   * @param path Path to list
   * @param options ls command options
   * @return true if listing succeeded, false on error
   */
  auto listDirectory = [](std::string_view path,
                          const LsOptions& options) -> bool {
    std::wstring wpath(path.begin(), path.end());
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW((wpath + L"\\*").c_str(), &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
      fprintf(stderr, "ls: cannot access '%s': No such file or directory\n",
              path.data());
      return false;
    }

    std::vector<std::wstring> files;
    do {
      std::wstring filename(find_data.cFileName);
      if (!options.show_all && (filename == L"." || filename == L".."))
        continue;
      files.push_back(std::move(filename));
    } while (FindNextFileW(hFind, &find_data) != 0);
    FindClose(hFind);

    if (options.reverse_order) {
      std::sort(files.rbegin(), files.rend());
    } else {
      std::sort(files.begin(), files.end());
    }

    for (const auto& file : files) {
      std::string filename(file.begin(), file.end());
      if (options.long_format) {
        printf("-rwxr-xr-x 1 user group 0 %s\n", filename.c_str());
      } else {
        printf("%s ", filename.c_str());
      }
    }
    if (!options.long_format) printf("\n");

    return true;
  };

  // Main implementation
  LsOptions options;
  std::vector<std::string_view> paths;
  if (!parseLsOptions(args, options, paths)) {
    return 2;  // Invalid option error code
  }

  int result = 0;

  // Process each path
  for (size_t i = 0; i < paths.size(); ++i) {
    if (paths.size() > 1) printf("%s:\n", paths[i].data());
    if (!listDirectory(paths[i], options)) {
      result = 1;  // Directory listing error
    }
    if (i < paths.size() - 1) printf("\n");
  }

  return result;
}

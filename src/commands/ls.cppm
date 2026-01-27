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
#include <fcntl.h>
#include <io.h>

#include "core/command_macros.h"
#include "core/auto_flags.h"
export module commands.ls;

import std;
import core.dispatcher;
import core.cmd_meta;
import core.opt;
import utils;

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

// Auto-generated lookup table for options from LS_OPTIONS
constexpr auto OPTION_HANDLERS = generate_option_handlers(LS_OPTIONS, "--tabsize", "--width");

CREATE_AUTO_FLAGS_CLASS(LsOptions,
    DECLARE_NUMERIC_OPTION(int, tab_size, 8)
    DECLARE_NUMERIC_OPTION(int, width, 0)
    
    // Define all flags
    DEFINE_FLAG(long_format, 0)
    DEFINE_FLAG(show_all, 1)
    DEFINE_FLAG(almost_all, 2)
    DEFINE_FLAG(escape, 3)
    DEFINE_FLAG(ignore_backups, 4)
    DEFINE_FLAG(sort_by_ctime, 5)
    DEFINE_FLAG(columns, 6)
    DEFINE_FLAG(directory, 7)
    DEFINE_FLAG(directory_order, 8)
    DEFINE_FLAG(classify, 9)
    DEFINE_FLAG(no_owner, 10)
    DEFINE_FLAG(human_readable, 11)
    DEFINE_FLAG(show_inode, 12)
    DEFINE_FLAG(kibibytes, 13)
    DEFINE_FLAG(dereference, 14)
    DEFINE_FLAG(comma_separated, 15)
    DEFINE_FLAG(numeric_uid_gid, 16)
    DEFINE_FLAG(literal, 17)
    DEFINE_FLAG(no_group, 18)
    DEFINE_FLAG(indicator_slash, 19)
    DEFINE_FLAG(hide_control_chars, 20)
    DEFINE_FLAG(quote_name, 21)
    DEFINE_FLAG(reverse_order, 22)
    DEFINE_FLAG(recursive, 23)
    DEFINE_FLAG(show_size, 24)
    DEFINE_FLAG(sort_by_size, 25)
    DEFINE_FLAG(sort_by_time, 26)
    DEFINE_FLAG(sort_by_atime, 27)
    DEFINE_FLAG(no_sort, 28)
    DEFINE_FLAG(natural_sort, 29)
    DEFINE_FLAG(lines, 30)
    DEFINE_FLAG(sort_by_extension, 31)
    DEFINE_FLAG(show_context, 32)
    DEFINE_FLAG(one_per_line, 33)
)

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
        bool found = false;
        for (const auto& handler : OPTION_HANDLERS) {
          if (handler.long_opt && arg == handler.long_opt) {
            if (handler.requires_arg) {
              if (i + 1 < args.size()) {
                if (handler.short_opt == 'T') {
                  options.tab_size = std::stoi(std::string(args[i + 1]));
                } else if (handler.short_opt == 'w') {
                  options.width = std::stoi(std::string(args[i + 1]));
                }
                ++i;
              } else {
                std::wstring warg(arg.begin(), arg.end());
                fwprintf(stderr, L"ls: option '%ls' requires an argument\n", warg.c_str());
                return false;
              }
            } else {
              // Set boolean option based on short_opt
              switch (handler.short_opt) {
                case 'l': options.set_long_format(true); break;
                case 'a': options.set_show_all(true); break;
                case 'A': options.set_almost_all(true); break;
                case 'b': options.set_escape(true); break;
                case 'B': options.set_ignore_backups(true); break;
                case 'c': options.set_sort_by_ctime(true); break;
                case 'C': options.set_columns(true); break;
                case 'd': options.set_directory(true); break;
                case 'f': options.set_directory_order(true); break;
                case 'F': options.set_classify(true); break;
                case 'g': options.set_no_owner(true); break;
                case 'h': options.set_human_readable(true); break;
                case 'i': options.set_show_inode(true); break;
                case 'k': options.set_kibibytes(true); break;
                case 'L': options.set_dereference(true); break;
                case 'm': options.set_comma_separated(true); break;
                case 'n': options.set_numeric_uid_gid(true); break;
                case 'N': options.set_literal(true); break;
                case 'o': options.set_no_group(true); break;
                case 'p': options.set_indicator_slash(true); break;
                case 'q': options.set_hide_control_chars(true); break;
                case 'Q': options.set_quote_name(true); break;
                case 'r': options.set_reverse_order(true); break;
                case 'R': options.set_recursive(true); break;
                case 's': options.set_show_size(true); break;
                case 'S': options.set_sort_by_size(true); break;
                case 't': options.set_sort_by_time(true); break;
                case 'u': options.set_sort_by_atime(true); break;
                case 'U': options.set_no_sort(true); break;
                case 'v': options.set_natural_sort(true); break;
                case 'x': options.set_lines(true); break;
                case 'X': options.set_sort_by_extension(true); break;
                case 'Z': options.set_show_context(true); break;
                case '1': options.set_one_per_line(true); break;
              }
            }
            found = true;
            break;
          }
        }
        if (!found) {
          std::wstring warg(arg.begin(), arg.end());
          fwprintf(stderr, L"ls: invalid option -- '%ls'\n", warg.c_str() + 2);
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
          char opt_char = arg[j];
          bool found = false;
          
          for (const auto& handler : OPTION_HANDLERS) {
            if (handler.short_opt == opt_char) {
              if (handler.requires_arg) {
                if (j + 1 < arg.size()) {
                  if (handler.short_opt == 'T') {
                    options.tab_size = std::stoi(std::string(arg.substr(j + 1)));
                  } else if (handler.short_opt == 'w') {
                    options.width = std::stoi(std::string(arg.substr(j + 1)));
                  }
                  j = arg.size() - 1;
                } else if (i + 1 < args.size()) {
                  if (handler.short_opt == 'T') {
                    options.tab_size = std::stoi(std::string(args[i + 1]));
                  } else if (handler.short_opt == 'w') {
                    options.width = std::stoi(std::string(args[i + 1]));
                  }
                  ++i;
                  break;
                } else {
                  fwprintf(stderr, L"ls: option requires an argument -- '%c'\n", opt_char);
                  return false;
                }
              } else {
                // Set boolean option based on opt_char
              switch (opt_char) {
                case 'l': options.set_long_format(true); break;
                case 'a': options.set_show_all(true); break;
                case 'A': options.set_almost_all(true); break;
                case 'b': options.set_escape(true); break;
                case 'B': options.set_ignore_backups(true); break;
                case 'c': options.set_sort_by_ctime(true); break;
                case 'C': options.set_columns(true); break;
                case 'd': options.set_directory(true); break;
                case 'f': options.set_directory_order(true); break;
                case 'F': options.set_classify(true); break;
                case 'g': options.set_no_owner(true); break;
                case 'h': options.set_human_readable(true); break;
                case 'i': options.set_show_inode(true); break;
                case 'k': options.set_kibibytes(true); break;
                case 'L': options.set_dereference(true); break;
                case 'm': options.set_comma_separated(true); break;
                case 'n': options.set_numeric_uid_gid(true); break;
                case 'N': options.set_literal(true); break;
                case 'o': options.set_no_group(true); break;
                case 'p': options.set_indicator_slash(true); break;
                case 'q': options.set_hide_control_chars(true); break;
                case 'Q': options.set_quote_name(true); break;
                case 'r': options.set_reverse_order(true); break;
                case 'R': options.set_recursive(true); break;
                case 's': options.set_show_size(true); break;
                case 'S': options.set_sort_by_size(true); break;
                case 't': options.set_sort_by_time(true); break;
                case 'u': options.set_sort_by_atime(true); break;
                case 'U': options.set_no_sort(true); break;
                case 'v': options.set_natural_sort(true); break;
                case 'x': options.set_lines(true); break;
                case 'X': options.set_sort_by_extension(true); break;
                case 'Z': options.set_show_context(true); break;
                case '1': options.set_one_per_line(true); break;
              }
              }
              found = true;
              break;
            }
          }
          
          if (!found) {
            fwprintf(stderr, L"ls: invalid option -- '%c'\n", opt_char);
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
    std::wstring wpath = utf8_to_wstring(path);
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW((wpath + L"\\*").c_str(), &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
      std::wstring wpath_str = utf8_to_wstring(path);
      fwprintf(stderr, L"ls: cannot access '%ls': No such file or directory\n",
              wpath_str.c_str());
      return false;
    }

    std::vector<std::wstring> files;
    do {
      std::wstring filename(find_data.cFileName);
      if (!options.get_show_all() && (filename == L"." || filename == L".."))
        continue;
      files.push_back(std::move(filename));
    } while (FindNextFileW(hFind, &find_data) != 0);
    FindClose(hFind);

    if (options.get_reverse_order()) {
      std::sort(files.rbegin(), files.rend());
    } else {
      std::ranges::sort(files.begin(), files.end());
    }

    for (const auto& file : files) {
      if (options.get_long_format()) {
        wprintf(L"-rwxr-xr-x 1 user group 0 %s\n", file.c_str());
      } else {
        wprintf(L"%s ", file.c_str());
      }
    }
    if (!options.get_long_format()) wprintf(L"\n");

    return true;
  };

  // Main implementation
  LsOptions options;
  std::vector<std::string_view> paths;

  // Set console to wide character mode to support Chinese characters
  setConsoleToWideCharMode();

  if (!parseLsOptions(args, options, paths)) {
    return 2;  // Invalid option error code
  }

  int result = 0;

  // Process each path
  for (size_t i = 0; i < paths.size(); ++i) {
    if (paths.size() > 1) {
      std::wstring wpath_str = utf8_to_wstring(paths[i]);
      wprintf(L"%s:\n", wpath_str.c_str());
    }
    if (!listDirectory(paths[i], options)) {
      result = 1;  // Directory listing error
    }
    if (i < paths.size() - 1) wprintf(L"\n");
  }

  return result;
}

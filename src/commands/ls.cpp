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
 *  - File: ls.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implemention for ls.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
//include other header after pch.h
#include "core/command_macros.h"

#pragma comment(lib, "advapi32.lib")
import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

/**
 * @brief LS command options definition
 *
 * This array defines all the options supported by the ls command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -a, @a --all: Do not ignore entries starting with . [IMPLEMENTED]
 * - @a -A, @a --almost-all: Do not list implied . and .. [IMPLEMENTED]
 * - @a -b, @a --escape: Print C-style escapes for nongraphic characters [TODO]
 * - @a -B, @a --ignore-backups: Do not list implied entries ending with ~
 * [TODO]
 * - @a -c: With -lt: sort by, and show, ctime; with -l: show ctime and sort by
 * name; otherwise: sort by ctime, newest first [TODO]
 * - @a -C: List entries by columns [IMPLEMENTED]
 * - @a -d, @a --directory: List directories themselves, not their contents
 * [TODO]
 * - @a -f: List all entries in directory order [TODO]
 * - @a -F, @a --classify: Append indicator (one of *=>@|) to entries [TODO]
 * - @a -g: Like -l, but do not list owner [IMPLEMENTED]
 * - @a -h, @a --human-readable: With -l and -s, print sizes like 1K 234M 2G
 * etc. [IMPLEMENTED]
 * - @a -i, @a --inode: Print the index number of each file [TODO]
 * - @a -k, @a --kibibytes: Default to 1024-byte blocks for file system usage
 * [TODO]
 * - @a -L, @a --dereference: When showing file information for a symbolic link,
 * show information for the file the link references [TODO]
 * - @a -l, @a --long-list: Use a long listing format [IMPLEMENTED]
 * - @a -m: Fill width with a comma separated list of entries [TODO]
 * - @a -n, @a --numeric-uid-gid: Like -l, but list numeric user and group IDs
 * [IMPLEMENTED]
 * - @a -N, @a --literal: Print entry names without quoting [TODO]
 * - @a -o: Like -l, but do not list group information [IMPLEMENTED]
 * - @a -p, @a --indicator-style=slash: Append / indicator to directories [TODO]
 * - @a -q, @a --hide-control-chars: Print ? instead of nongraphic characters
 * [TODO]
 * - @a -Q, @a --quote-name: Enclose entry names in double quotes [TODO]
 * - @a -r, @a --reverse: Reverse order while sorting [IMPLEMENTED]
 * - @a -R, @a --recursive: List subdirectories recursively [TODO]
 * - @a -s, @a --size: Print the allocated size of each file, in blocks [TODO]
 * - @a -S: Sort by file size, largest first [TODO]
 * - @a -t: Sort by time, newest first [TODO]
 * - @a -T, @a --tabsize: Assume tab stops at each COLS instead of 8
 * [IMPLEMENTED]
 * - @a -u: With -lt: sort by, and show, access time; with -l: show access time
 * and sort by name; otherwise: sort by access time, newest first [TODO]
 * - @a -U: Do not sort; list entries in directory order [TODO]
 * - @a -v: Natural sort of (version) numbers within text [TODO]
 * - @a -w, @a --width: Set output width to COLS. 0 means no limit [IMPLEMENTED]
 * - @a -x: List entries by lines instead of by columns [TODO]
 * - @a -X: Sort alphabetically by entry extension [TODO]
 * - @a -Z, @a --context: Print any security context of each file [TODO]
 * - @a -1: List one file per line [IMPLEMENTED]
 */
auto constexpr LS_OPTIONS = std::array{
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
    OPTION("", "--color",
           "colorize the output; WHEN can be 'always', 'auto', or 'never'",
           STRING_TYPE),
    OPTION("-1", "", "list one file per line")};

// ======================================================
// Constants
// ======================================================
namespace ls_constants {
constexpr int DEFAULT_TAB_SIZE = 8;
constexpr int DEFAULT_WIDTH = 0;

// File extensions for different types
const std::array<const wchar_t *, 10> COMPRESSED_EXTS = {
    L"zip", L"rar", L"7z",  L"tar", L"gz",
    L"bz2", L"xz",  L"iso", L"cab", L"arc"};

const std::array<const wchar_t *, 10> SCRIPT_EXTS = {
    L"sh", L"bat", L"cmd", L"py", L"pl", L"lua", L"js", L"php", L"rb", L"ps1"};
}  // namespace ls_constants

/**
 * @brief Check if a file handle is a terminal
 * @param stream Stream to check
 * @return True if stream is a terminal
 */
bool is_terminal(FILE *stream) {
  DWORD mode;
  return GetConsoleMode((HANDLE)_get_osfhandle(_fileno(stream)), &mode) != 0;
}

// ======================================================
// Pipeline components
// ======================================================
namespace ls_pipeline {
namespace cp = core::pipeline;

/**
 * @brief Validate arguments
 * @param ctx Command context
 * @return Result with paths to process
 */
auto validate_arguments(const CommandContext<LS_OPTIONS.size()> &ctx)
    -> cp::Result<std::vector<std::string>> {
  std::vector<std::string> paths;
  for (auto arg : ctx.positionals) {
    paths.push_back(std::string(arg));
  }

  if (paths.empty()) {
    paths.push_back(".");
  }

  return paths;
}

/**
 * @brief Get file permissions string
 * @param find_data WIN32_FIND_DATAW structure
 * @return Permissions string in ls format
 */
/**
 * @brief Get file permissions string (improved: simulate real Linux
 * permissions)
 * @param find_data WIN32_FIND_DATAW structure
 * @return Permissions string in ls format
 */
auto get_permissions_string(const WIN32_FIND_DATAW &find_data) -> std::string {
  char perms[11] = "----------";
  perms[10] = '\0';

  // Set file type
  if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    perms[0] = 'd';
    // Directory: owner rwx, group rx, other rx
    perms[1] = 'r';
    perms[2] = 'w';
    perms[3] = 'x';
    perms[4] = 'r';
    perms[5] = '-';
    perms[6] = 'x';
    perms[7] = 'r';
    perms[8] = '-';
    perms[9] = 'x';
  } else if (find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
    perms[0] = 'l';  // Symbolic link
    perms[1] = 'r';
    perms[2] = 'w';
    perms[3] = 'x';
    perms[4] = 'r';
    perms[5] = 'w';
    perms[6] = 'x';
    perms[7] = 'r';
    perms[8] = 'w';
    perms[9] = 'x';
  } else {
    perms[0] = '-';
    // Check if file is executable (exe/bat/cmd/ps1)
    std::wstring filename = find_data.cFileName;
    std::wstring ext;
    size_t dot_pos = filename.find_last_of(L".");
    if (dot_pos != std::wstring::npos) {
      ext = filename.substr(dot_pos + 1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    }

    bool is_executable =
        (ext == L"exe" || ext == L"bat" || ext == L"cmd" || ext == L"ps1");

    // Regular file:
    // - Executable: owner rwx, group rx, other rx
    // - Non-executable: owner rw, group r, other r
    perms[1] = 'r';
    perms[2] = 'w';
    perms[4] = 'r';
    perms[5] = '-';
    perms[7] = 'r';
    perms[8] = '-';

    if (is_executable) {
      perms[3] = 'x';
      perms[6] = 'x';
      perms[9] = 'x';
    } else {
      perms[3] = '-';
      perms[6] = '-';
      perms[9] = '-';
    }
  }

  // Handle hidden files (optional)
  // Note: Keeping 10 characters for consistent formatting
  return std::string(perms, 10);
}

/**
 * @brief Get file size string
 * @param find_data WIN32_FIND_DATAW structure
 * @param ctx Command context
 * @return File size string
 */
auto get_file_size_string(const WIN32_FIND_DATAW &find_data,
                          const CommandContext<LS_OPTIONS.size()> &ctx)
    -> std::string {
  if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    return {};
  }

  // Calculate file size
  uint64_t fileSize = static_cast<uint64_t>(find_data.nFileSizeLow) |
                      (static_cast<uint64_t>(find_data.nFileSizeHigh) << 32);

  char buf[32];

  if (ctx.get<bool>("-h", false) || ctx.get<bool>("--human-readable", false)) {
    const char *units[] = {"B", "K", "M", "G", "T"};
    int unitIndex = 0;
    double size = static_cast<double>(fileSize);

    while (size >= 1024.0 && unitIndex < 4) {
      size /= 1024.0;
      ++unitIndex;
    }

    if (size < 10.0) {
      snprintf(buf, sizeof(buf), "%.1f%s", size, units[unitIndex]);
    } else {
      snprintf(buf, sizeof(buf), "%.0f%s", size, units[unitIndex]);
    }
  } else {
    snprintf(buf, sizeof(buf), "%llu", fileSize);
  }

  return std::string(buf);
}

/**
 * @brief Get file modification time string (improved: timezone support)
 * @param find_data WIN32_FIND_DATAW structure
 * @param use_utc Whether to use UTC time (default: false = local time)
 * @return Modification time string
 */
auto get_modification_time_string(const WIN32_FIND_DATAW &find_data,
                                  bool use_utc = false) -> std::string {
  SYSTEMTIME st;
  if (use_utc) {
    FileTimeToSystemTime(&find_data.ftLastWriteTime, &st);  // UTC
  } else {
    // Convert to local time (align Git Bash's local timezone)
    FILETIME local_ft;
    FileTimeToLocalFileTime(&find_data.ftLastWriteTime, &local_ft);
    FileTimeToSystemTime(&local_ft, &st);
  }

  const char *month_abbrs[] = {"",    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char buf[64];
  snprintf(buf, sizeof(buf), "%s %2d %02d:%02d", month_abbrs[st.wMonth],
           st.wDay, st.wHour, st.wMinute);

  return std::string(buf);
}

/**
 * @brief Get file owner and group information (improved: support numeric ID)
 * @param use_numeric Whether to return numeric UID/GID (-n option)
 * @return Pair of (owner, group) strings
 */
auto get_file_owner_and_group(bool use_numeric = false)
    -> std::pair<std::string, std::string> {
  if (use_numeric) {
    // Get Windows SID (simulate UID/GID)
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
      DWORD bufferSize = 0;
      GetTokenInformation(hToken, TokenUser, nullptr, 0, &bufferSize);
      std::vector<BYTE> buffer(bufferSize);
      PTOKEN_USER pTokenUser = reinterpret_cast<PTOKEN_USER>(buffer.data());
      if (GetTokenInformation(hToken, TokenUser, pTokenUser, bufferSize,
                              &bufferSize)) {
        LPWSTR sidStr = nullptr;
        if (ConvertSidToStringSidW(pTokenUser->User.Sid, &sidStr)) {
          // Extract numeric part from SID (simulate UID 197121)
          std::wstring sid = sidStr;
          size_t lastDash = sid.find_last_of(L'-');
          std::wstring uid_wstr = (lastDash != std::wstring::npos)
                                      ? sid.substr(lastDash + 1)
                                      : L"197121";

          // Convert wide string to UTF-8
          std::string uid = wstring_to_utf8(uid_wstr);
          LocalFree(sidStr);
          CloseHandle(hToken);
          return {uid, uid};  // UID = GID (Windows default)
        }
      }
      CloseHandle(hToken);
    }
    return {"197121", "197121"};  // Fallback
  }

  // Return username using ANSI version for efficiency
  char username[UNLEN + 1];
  DWORD username_len = UNLEN + 1;
  if (!GetUserNameA(username, &username_len)) {
    return {"user", "group"};
  }

  std::string username_str = username;
  size_t pos = username_str.find('\\');
  if (pos != std::string::npos) {
    username_str = username_str.substr(pos + 1);
  }
  return {username_str, username_str};
}

/**
 * @brief Get terminal width
 * @return Terminal width in columns
 */
auto get_terminal_width() -> int {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
  }
  return 80;  // Default to 80 columns if we can't get terminal width
}

/**
 * @brief Get string display width (simplified, assumes 1 character = 1 column)
 * @param str String to measure
 * @return Display width in columns
 */
auto string_display_width(const std::wstring &str) -> size_t {
  return str.length();
}

/**
 * @brief Calculate optimal column layout
 * @param entries List of entries
 * @param terminal_width Terminal width in columns
 * @return Number of columns and number of rows
 */
auto calculate_layout(const std::vector<std::wstring> &entries,
                      int terminal_width) -> std::pair<int, int> {
  if (entries.empty()) {
    return {0, 0};
  }

  // Find the longest entry display width
  size_t max_display_width = 0;
  for (const auto &entry : entries) {
    max_display_width =
        std::max(max_display_width, string_display_width(entry));
  }

  // Minimum column width = max display width + 2 spaces padding
  int min_column_width = static_cast<int>(max_display_width) + 2;
  if (min_column_width <= 0) {
    min_column_width = 1;
  }

  // Calculate maximum possible columns with minimum width
  int max_cols = terminal_width / min_column_width;
  if (max_cols < 1) {
    max_cols = 1;
  }

  // Try to find optimal column width that fills the terminal
  int best_cols = max_cols;
  int best_column_width = min_column_width;

  // If we can fit more than 1 column, try to adjust column width to fill the
  // screen
  if (max_cols > 1) {
    // Calculate how much space is left with minimum column width
    int remaining_space = terminal_width % min_column_width;

    // If there's remaining space, distribute it among columns
    if (remaining_space > 0) {
      // Calculate how much extra space per column
      int extra_per_column = remaining_space / max_cols;

      // New column width with extra space
      int new_column_width = min_column_width + extra_per_column;

      // Calculate new number of columns with adjusted width
      int new_cols = terminal_width / new_column_width;
      if (new_cols > 0) {
        best_cols = new_cols;
        best_column_width = new_column_width;
      }
    }
  }

  // Calculate number of rows
  int rows = (entries.size() + best_cols - 1) / best_cols;

  return {best_cols, rows};
}

/**
 * @brief Print entries in column format
 * @param entries List of entries
 * @param ctx Command context
 * @param base_path Base path for the entries
 */
auto print_columns(const std::vector<std::wstring> &entries,
                   const CommandContext<LS_OPTIONS.size()> &ctx,
                   const std::wstring &base_path) {
  if (entries.empty()) {
    return;
  }

  // Check if color is enabled based on --color option
  bool color_enabled = true;  // Default to enabled
  std::string color_option = ctx.get<std::string>("--color", "auto");
  if (color_option == "never") {
    color_enabled = false;
  } else if (color_option == "auto") {
    // Check if stdout is a terminal
    color_enabled = is_terminal(stdout);
  }
  // "always" or any other value enables color

  // Get terminal width or use specified width
  int width = ctx.get<int>("-w", 0) || ctx.get<int>("--width", 0);
  if (width <= 0) {
    width = get_terminal_width();
  }

  // Calculate layout
  auto [cols, rows] = calculate_layout(entries, width);

  // Calculate column widths to fill the terminal
  size_t max_display_width = 0;
  for (const auto &entry : entries) {
    max_display_width =
        std::max(max_display_width, string_display_width(entry));
  }

  // Calculate base column width
  int base_col_width = static_cast<int>(max_display_width) + 2;
  if (base_col_width <= 0) {
    base_col_width = 1;
  }

  // Calculate total width used by all columns
  int total_used_width = cols * base_col_width;
  int remaining_space = width - total_used_width;

  // Distribute remaining space among columns
  std::vector<int> col_widths(cols, base_col_width);
  if (remaining_space > 0 && cols > 0) {
    int extra_per_col = remaining_space / cols;
    int extra_remaining = remaining_space % cols;

    for (int i = 0; i < cols; ++i) {
      col_widths[i] += extra_per_col;
      if (i < extra_remaining) {
        col_widths[i] += 1;
      }
    }
  }

  // Calculate maximum display width for each column
  std::vector<size_t> col_max_widths(cols, 0);
  for (int col = 0; col < cols; ++col) {
    for (int row = 0; row < rows; ++row) {
      size_t idx = row + col * rows;
      if (idx < entries.size()) {
        col_max_widths[col] =
            std::max(col_max_widths[col], string_display_width(entries[idx]));
      }
    }
  }

  // Print entries in columns
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      size_t index = row + col * rows;
      if (index < entries.size()) {
        const auto &entry = entries[index];

        // Get file type and apply color
        std::wstring path = base_path + L"\\" + entry;
        WIN32_FIND_DATAW find_data;
        HANDLE hFind = FindFirstFileW(path.c_str(), &find_data);

        if (hFind != INVALID_HANDLE_VALUE) {
          if (color_enabled) {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
              safePrint(COLOR_DIR);
            } else if (find_data.dwFileAttributes &
                       FILE_ATTRIBUTE_REPARSE_POINT) {
              safePrint(COLOR_LINK);
            } else {
              // Check file extensions for different types
              std::wstring ext;
              size_t dot_pos = entry.find_last_of(L".");
              if (dot_pos != std::wstring::npos &&
                  dot_pos < entry.length() - 1) {
                ext = entry.substr(dot_pos + 1);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
              }

              // Check for compressed files
              bool is_compressed = false;
              for (const auto *comp_ext : ls_constants::COMPRESSED_EXTS) {
                if (ext == comp_ext) {
                  is_compressed = true;
                  break;
                }
              }

              // Check for script files
              bool is_script = false;
              for (const auto *script_ext : ls_constants::SCRIPT_EXTS) {
                if (ext == script_ext) {
                  is_script = true;
                  break;
                }
              }

              // Check for executable files
              bool is_executable = false;
              if (ext == L"exe" || ext == L"com" || ext == L"bat" ||
                  ext == L"cmd" || ext == L"ps1") {
                is_executable = true;
              }

              // Apply color based on file type
              if (is_compressed) {
                safePrint(COLOR_ARCHIVE);
              } else if (is_script) {
                safePrint(COLOR_SCRIPT);
              } else if (is_executable) {
                safePrint(COLOR_EXEC);
              } else {
                safePrint(COLOR_FILE);
              }
            }
          }
          FindClose(hFind);
        }

        // Print entry
        safePrint(wstring_to_utf8(entry));
        if (color_enabled) {
          safePrint(COLOR_RESET);
        }

        // Add spacing
        if (col < cols - 1) {
          // Use calculated column width for spacing
          size_t current_width = string_display_width(entry);
          int spaces_needed = col_widths[col] - static_cast<int>(current_width);
          if (spaces_needed > 0) {
            for (int i = 0; i < spaces_needed; ++i) {
              safePrint(L" ");
            }
          } else {
            // At least 2 spaces between columns
            safePrint(L"  ");
          }
        }
      }
    }
    safePrintLn(L"");
  }
}

/**
 * @brief List directory contents
 * @param path Path to directory
 * @param ctx Command context
 * @return Result with success status
 */
auto list_directory(const std::string &path,
                    const CommandContext<LS_OPTIONS.size()> &ctx)
    -> cp::Result<bool> {
  std::wstring wpath = utf8_to_wstring(path);
  std::wstring search_path = wpath + L"\\*";

  WIN32_FIND_DATAW find_data;
  HANDLE hFind = FindFirstFileW(search_path.c_str(), &find_data);

  if (hFind == INVALID_HANDLE_VALUE) {
    return std::unexpected("cannot access '" + path +
                           "': No such file or directory");
  }

  std::vector<std::wstring> entries;
  do {
    std::wstring filename = find_data.cFileName;

    // Skip . and .. unless show_all is set
    if (filename == L"." || filename == L"..") {
      if (!ctx.get<bool>("-a", false) && !ctx.get<bool>("--all", false)) {
        continue;
      }
    }
    // Skip hidden files unless show_all or almost_all is set
    else if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
             !ctx.get<bool>("-a", false) && !ctx.get<bool>("--all", false) &&
             !ctx.get<bool>("-A", false) &&
             !ctx.get<bool>("--almost-all", false)) {
      continue;
    }

    entries.push_back(filename);
  } while (FindNextFileW(hFind, &find_data) != 0);

  FindClose(hFind);

  // Sort entries
  bool no_sort = ctx.get<bool>("-U", false);
  if (!no_sort) {
    std::sort(entries.begin(), entries.end());
    if (ctx.get<bool>("-r", false) || ctx.get<bool>("--reverse", false)) {
      std::reverse(entries.begin(), entries.end());
    }
  }

  // Determine output format
  bool long_format =
      ctx.get<bool>("-l", false) || ctx.get<bool>("--long-list", false);
  bool one_per_line = ctx.get<bool>("-1", false);
  bool columns = ctx.get<bool>("-C", false);
  bool use_numeric =
      ctx.get<bool>("-n", false) || ctx.get<bool>("--numeric-uid-gid", false);
  if (long_format) {
    // FileInfo struct for storing file information
    struct FileInfo {
      std::wstring name;
      WIN32_FIND_DATAW find_data;
      std::string perms;
      std::string size;
      std::string mtime;
      std::string owner;
      std::string group;
    };

    // Collect file information
    std::vector<FileInfo> files;
    for (const auto &entry : entries) {
      std::wstring full_path = wpath;
      full_path += L'\\';
      full_path += entry;
      WIN32_FIND_DATAW entry_data;
      HANDLE hEntry = FindFirstFileW(full_path.c_str(), &entry_data);
      if (hEntry != INVALID_HANDLE_VALUE) {
        FileInfo info;
        info.name = entry;
        info.find_data = entry_data;
        info.perms = get_permissions_string(entry_data);
        info.size = get_file_size_string(entry_data, ctx);
        info.mtime = get_modification_time_string(entry_data);

        // Get owner and group
        auto [owner, group] = get_file_owner_and_group(use_numeric);
        info.owner = owner;
        info.group = group;

        files.push_back(info);
        FindClose(hEntry);
      }
    }

    // Calculate maximum widths for alignment
    size_t max_owner_len = 0;
    size_t max_group_len = 0;
    size_t max_size_len = 0;
    for (const auto &file : files) {
      max_owner_len = std::max(max_owner_len, file.owner.length());
      max_group_len = std::max(max_group_len, file.group.length());
      max_size_len = std::max(max_size_len, file.size.length());
    }

    // Set minimum widths to avoid empty values
    if (max_owner_len == 0) max_owner_len = 1;
    if (max_group_len == 0) max_group_len = 1;
    if (max_size_len == 0) max_size_len = 1;

    // Long format output
    for (const auto &file_info : files) {
      // 1. Permissions and link count
      safePrint(std::string_view(file_info.perms));
      safePrint(" ");
      safePrint("1");  // Windows always has 1 link
      safePrint(" ");

      // 2. Owner (left-aligned)
      safePrint(std::string_view(file_info.owner));
      int owner_padding = static_cast<int>(max_owner_len) -
                          static_cast<int>(file_info.owner.length());
      for (int i = 0; i < owner_padding; i++) {
        safePrint(" ");
      }
      safePrint(" ");

      // 3. Group (left-aligned)
      safePrint(std::string_view(file_info.group));
      int group_padding = static_cast<int>(max_group_len) -
                          static_cast<int>(file_info.group.length());
      for (int i = 0; i < group_padding; i++) {
        safePrint(" ");
      }
      safePrint(" ");

      // 4. File size (right-aligned)
      int size_padding = static_cast<int>(max_size_len) -
                         static_cast<int>(file_info.size.length());
      for (int i = 0; i < size_padding; i++) {
        safePrint(" ");
      }
      safePrint(std::string_view(file_info.size));
      safePrint(" ");

      // 5. Modification time
      safePrint(std::string_view(file_info.mtime));
      safePrint(" ");

      // Check if color is enabled based on --color option
      bool color_enabled = true;  // Default to enabled
      std::string color_option = ctx.get<std::string>("--color", "auto");
      if (color_option == "never") {
        color_enabled = false;
      } else if (color_option == "auto") {
        // Check if stdout is a terminal
        color_enabled = is_terminal(stdout);
      }
      // "always" or any other value enables color

      // 6. Filename with color
      if (color_enabled) {
        if (file_info.find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          safePrint(COLOR_DIR);
        } else if (file_info.find_data.dwFileAttributes &
                   FILE_ATTRIBUTE_REPARSE_POINT) {
          safePrint(COLOR_LINK);
        } else {
          // Check file extensions for different types
          std::wstring ext;
          size_t dot_pos = file_info.name.find_last_of(L".");
          if (dot_pos != std::wstring::npos &&
              dot_pos < file_info.name.length() - 1) {
            ext = file_info.name.substr(dot_pos + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
          }

          // Check for compressed files
          bool is_compressed = false;
          for (const auto *comp_ext : ls_constants::COMPRESSED_EXTS) {
            if (ext == comp_ext) {
              is_compressed = true;
              break;
            }
          }

          // Check for script files
          bool is_script = false;
          for (const auto *script_ext : ls_constants::SCRIPT_EXTS) {
            if (ext == script_ext) {
              is_script = true;
              break;
            }
          }

          // Check for executable files
          bool is_executable = false;
          if (ext == L"exe" || ext == L"com" || ext == L"bat" ||
              ext == L"cmd" || ext == L"ps1") {
            is_executable = true;
          }

          // Apply color based on file type
          if (is_compressed) {
            safePrint(COLOR_ARCHIVE);
          } else if (is_script) {
            safePrint(COLOR_SCRIPT);
          } else if (is_executable) {
            safePrint(COLOR_EXEC);
          } else {
            safePrint(COLOR_FILE);
          }
        }
      }

      safePrint(wstring_to_utf8(file_info.name));
      if (color_enabled) {
        safePrint(COLOR_RESET);
      }
      safePrintLn(L"");
    }
  } else if (one_per_line) {
    // Check if color is enabled based on --color option
    bool color_enabled = true;  // Default to enabled
    std::string color_option = ctx.get<std::string>("--color", "auto");
    if (color_option == "never") {
      color_enabled = false;
    } else if (color_option == "auto") {
      // Check if stdout is a terminal
      color_enabled = is_terminal(stdout);
    }
    // "always" or any other value enables color

    // One entry per line
    for (const auto &entry : entries) {
      std::wstring full_path = wpath;
      full_path += L'\\';
      full_path += entry;
      WIN32_FIND_DATAW entry_data;
      HANDLE hEntry = FindFirstFileW(full_path.c_str(), &entry_data);

      if (hEntry != INVALID_HANDLE_VALUE) {
        // Apply color based on file type
        if (color_enabled) {
          if (entry_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            safePrint(COLOR_DIR);
          } else if (entry_data.dwFileAttributes &
                     FILE_ATTRIBUTE_REPARSE_POINT) {
            safePrint(COLOR_LINK);
          } else {
            // Check file extensions for different types
            std::wstring ext;
            size_t dot_pos = entry.find_last_of(L".");
            if (dot_pos != std::wstring::npos && dot_pos < entry.length() - 1) {
              ext = entry.substr(dot_pos + 1);
              std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            }

            // Check for compressed files
            bool is_compressed = false;
            for (const auto *comp_ext : ls_constants::COMPRESSED_EXTS) {
              if (ext == comp_ext) {
                is_compressed = true;
                break;
              }
            }

            // Check for script files
            bool is_script = false;
            for (const auto *script_ext : ls_constants::SCRIPT_EXTS) {
              if (ext == script_ext) {
                is_script = true;
                break;
              }
            }

            // Check for executable files
            bool is_executable = false;
            if (ext == L"exe" || ext == L"com" || ext == L"bat" ||
                ext == L"cmd" || ext == L"ps1") {
              is_executable = true;
            }

            // Apply color based on file type
            if (is_compressed) {
              safePrint(COLOR_ARCHIVE);
            } else if (is_script) {
              safePrint(COLOR_SCRIPT);
            } else if (is_executable) {
              safePrint(COLOR_EXEC);
            } else {
              safePrint(COLOR_FILE);
            }
          }
        }

        safePrint(entry);
        if (color_enabled) {
          safePrint(COLOR_RESET);
        }
        safePrintLn(L"");

        FindClose(hEntry);
      }
    }
  } else {
    // Column format
    print_columns(entries, ctx, wpath);
  }

  return true;
}

/**
 * @brief Process all paths
 * @param paths Paths to process
 * @param ctx Command context
 * @return Result with success status
 */
auto process_paths(const std::vector<std::string> &paths,
                   const CommandContext<LS_OPTIONS.size()> &ctx)
    -> cp::Result<bool> {
  bool success = true;
  for (size_t i = 0; i < paths.size(); ++i) {
    const auto &path = paths[i];

    // Print path header if multiple paths
    if (paths.size() > 1) {
      safePrintLn(std::wstring(path.begin(), path.end()) + L":");
    }

    auto result = list_directory(path, ctx);
    if (!result) {
      return std::unexpected(result.error());
    }
    if (!*result) {
      success = false;
    }

    // Add newline between paths
    if (i < paths.size() - 1) {
      safePrintLn(L"");
    }
  }
  return success;
}

/**
 * @brief Main pipeline
 * @param ctx Command context
 * @return Result with success status
 */
template <size_t N>
auto process_command(const CommandContext<N> &ctx) -> cp::Result<bool> {
  return validate_arguments(ctx).and_then(
      [&](const std::vector<std::string> &paths) {
        return process_paths(paths, ctx);
      });
}

}  // namespace ls_pipeline

// ======================================================
// Command registration
// ======================================================

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
  using namespace ls_pipeline;
  using namespace core::pipeline;

  auto result = process_command(ctx);
  if (!result) {
    report_error(result, L"ls");
    return 1;
  }

  return *result ? 0 : 1;
}

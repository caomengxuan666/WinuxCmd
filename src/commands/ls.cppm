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
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implemention for ls.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

module;
#include "core/auto_flags.h"
#include "core/command_macros.h"
#include "pch/pch.h"
export module commands.ls;

import std;
import core;
import utils;
namespace winux::commands {

// clang-format off
constexpr auto LS_OPTIONS = std::array{
    // Long options with arguments
    option{"tabsize", 'T', true, "set tab size (8)", "COLS"},
    option{"width", 'w', true, "set output width", "COLS"},

    // Long options without arguments
    option{"all", 'a', false, "do not ignore entries starting with ."},
    option{"almost-all", 'A', false, "do not list implied . and .."},
    option{"escape", 'b', false, "print C-style escapes for nongraphic characters"},
    option{"ignore-backups", 'B', false, "do not list implied entries ending with ~"},
    option{"ctime", 'c', false, "sort by, and show, ctime (time of last modification of file status information)"},
    option{"columns", 'C', false, "list entries by columns"},
    option{"directory", 'd', false, "list directories themselves, not their contents"},
    option{"directory-order", 'f', false, "do not sort, enable -aU, disable -ls --color"},
    option{"classify", 'F', false, "append indicator (one of */=>@|) to entries"},
    option{"no-owner", 'g', false, "like -l, but do not list owner"},
    option{"human-readable", 'h', false, "with -l and/or -s, print human readable sizes (e.g., 1K 234M 2G)"},
    option{"inode", 'i', false, "print the index number of each file"},
    option{"kibibytes", 'k', false, "like --block-size=1K"},
    option{"dereference", 'L', false, "dereference symbolic links"},
    option{"comma-separated-values", 'm', false, "fill width with a comma separated list of entries"},
    option{"numeric-uid-gid", 'n', false, "like -l, but list numeric user and group IDs"},
    option{"literal", 'N', false, "print raw entry names (don't treat e.g. control characters specially)"},
    option{"no-group", 'o', false, "like -l, but do not list group"},
    option{"indicator-style", 'p', false, "append / indicator to directories"},
    option{"hide-control-chars", 'q', false, "print ? instead of nongraphic characters"},
    option{"quote-name", 'Q', false, "enclose entry names in double quotes"},
    option{"reverse", 'r', false, "reverse order while sorting"},
    option{"recursive", 'R', false, "list subdirectories recursively"},
    option{"size", 's', false, "print the allocated size of each file, in blocks"},
    option{"sort-size", 'S', false, "sort by file size, largest first"},
    option{"sort-time", 't', false, "sort by modification time, newest first"},
    option{"access-time", 'u', false, "sort by, and show, access time"},
    option{"sort-none", 'U', false, "do not sort, list entries in directory order"},
    option{"natural-sort", 'v', false, "natural sort of (version) numbers within text"},
    option{"lines", 'x', false, "list entries by lines instead of by columns"},
    option{"sort-extension", 'X', false, "sort alphabetically by entry extension"},
    option{"context", 'Z', false, "print any security context of each file"},
    option{"one-file-per-line", '1', false, "list one file per line"}
};
// clang-format on

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
   * @brief Get file permissions string
   * @param find_data WIN32_FIND_DATAW structure
   * @return Permissions string in ls format
   */
  auto getPermissionsString =
      [](const WIN32_FIND_DATAW &find_data) -> std::wstring {
    std::wstring perms = L"----------";

    // Set file type
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      perms[0] = L'd';
    } else if (find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
      perms[0] = L'l';
    } else {
      perms[0] = L'-';
    }

    // Set permissions (simplified for Windows)
    perms[1] = L'r';  // Owner read
    perms[2] = L'w';  // Owner write
    perms[3] = L'x';  // Owner execute
    perms[4] = L'r';  // Group read
    perms[5] = L'w';  // Group write
    perms[6] = L'x';  // Group execute
    perms[7] = L'r';  // Other read
    perms[8] = L'w';  // Other write
    perms[9] = L'x';  // Other execute

    return perms;
  };

  /**
   * @brief Get file size string
   * @param find_data WIN32_FIND_DATAW structure
   * @param ctx Command context
   * @return File size string
   */
  auto getFileSizeString = [&](const WIN32_FIND_DATAW &find_data) -> std::wstring {
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      return L"";  // Empty string for directories
    }

    // Calculate file size
    uint64_t fileSize = static_cast<uint64_t>(find_data.nFileSizeLow) |
                        (static_cast<uint64_t>(find_data.nFileSizeHigh) << 32);

    if (ctx.get<bool>("--human-readable", false) || ctx.get<bool>("-h", false)) {
      // Human-readable format - use std::wostringstream for safety
      const wchar_t *units[] = {L"B", L"K", L"M", L"G", L"T"};
      int unitIndex = 0;
      double size = static_cast<double>(fileSize);

      while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
      }

      // Use string stream for formatting
      std::wostringstream oss;
      if (size < 10.0) {
        oss << std::fixed << std::setprecision(1) << size << units[unitIndex];
      } else {
        oss << std::fixed << std::setprecision(0) << size << units[unitIndex];
      }
      return oss.str();
    } else {
      // Regular format - use string stream
      std::wostringstream oss;
      oss << fileSize;
      return oss.str();
    }
  };

  /**
   * @brief Get file modification time string
   * @param find_data WIN32_FIND_DATAW structure
   * @return Modification time string
   */
  auto getModificationTimeString =
      [](const WIN32_FIND_DATAW &find_data) -> std::wstring {
    SYSTEMTIME st;
    FileTimeToSystemTime(&find_data.ftLastWriteTime, &st);

    // Month abbreviations
    const wchar_t *month_abbrs[] = {L"",    L"Jan", L"Feb", L"Mar", L"Apr",
                                    L"May", L"Jun", L"Jul", L"Aug", L"Sep",
                                    L"Oct", L"Nov", L"Dec"};

    // Use wostringstream for consistency
    std::wostringstream oss;
    oss << month_abbrs[st.wMonth] << L" ";
    if (st.wDay < 10) {
      oss << L" ";
    }
    oss << st.wDay << L" " << std::setfill(L'0') << std::setw(2) << st.wHour
        << L":" << std::setw(2) << st.wMinute;

    return oss.str();
  };

  /**
   * @brief Get file owner and group information
   * @return Pair of (owner, group) strings
   */
  auto getFileOwnerAndGroup = []() -> std::pair<std::wstring, std::wstring> {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (!GetUserNameW(username, &username_len)) {
      return {L"user", L"group"};
    }

    std::wstring username_str = username;

    // Extract only username part from DOMAIN\USER
    size_t pos = username_str.find(L'\\');
    if (pos != std::wstring::npos) {
      username_str = username_str.substr(pos + 1);
    }

    // On Windows, group name is same as username for now
    return {username_str, username_str};
  };

  /**
   * @brief Get color for file based on type
   * @param find_data WIN32_FIND_DATAW structure
   * @return Color string
   */
  auto getFileColor = [](const WIN32_FIND_DATAW &find_data) -> std::wstring {
    if (!isTerminalSupportsColor()) {
      return L"";
    }

    std::wstring filename(find_data.cFileName);
    size_t dotPos = filename.rfind(L'.');
    if (dotPos == std::wstring::npos) {
      // No extension
      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        return COLOR_DIR;
      }
      if (find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        return COLOR_LINK;
      }
      // Regular file
      return COLOR_FILE;
    }

    std::wstring ext = filename.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

    // === Executable files ===
    static const std::unordered_set<std::wstring> execExts = {
        L"exe", L"com", L"bat", L"cmd", L"msi"};
    if (execExts.contains(ext)) {
      return COLOR_EXEC;
    }

    // === Script files ===
    static const std::unordered_set<std::wstring> scriptExts = {
        L"ps1", L"psm1", L"sh", L"bash", L"zsh", L"py",
        L"pl",  L"rb",   L"js", L"ts",   L"lua"};
    if (scriptExts.contains(ext)) {
      return COLOR_SCRIPT;
    }

    // === Archives ===
    static const std::unordered_set<std::wstring> archiveExts = {
        L"zip", L"rar", L"7z",   L"tar", L"gz", L"bz2",
        L"xz",  L"tgz", L"tbz2", L"lz",  L"zst"};
    if (archiveExts.contains(ext)) {
      return COLOR_ARCHIVE;
    }

    // === Source code ===
    static const std::unordered_set<std::wstring> sourceExts = {
        L"c",  L"cpp",  L"cxx", L"h",     L"hpp", L"rs",
        L"go", L"java", L"cs",  L"swift", L"kt",  L"js",
        L"ts", L"jsx",  L"tsx", L"html",  L"css"};
    if (sourceExts.contains(ext)) {
      return COLOR_SOURCE;
    }

    // === Media ===
    static const std::unordered_set<std::wstring> mediaExts = {
        L"jpg", L"jpeg", L"png",  L"gif", L"bmp", L"webp",
        L"mp3", L"wav",  L"flac", L"mp4", L"avi", L"mkv"};
    if (mediaExts.contains(ext)) {
      return COLOR_MEDIA;
    }

    // Default: regular file
    return COLOR_FILE;
  };

  auto formatFilenameWithColor = [&](const WIN32_FIND_DATAW &fd,
                                     const std::wstring &name) -> std::wstring {
    if (isTerminalSupportsColor()) {
      return getFileColor(fd) + name + COLOR_RESET;
    }
    return name;
  };

  /**
   * @brief Calculate optimal column layout
   * @param filenames List of filenames
   * @param terminalWidth Terminal width in columns
   * @return Pair of (columns, rows)
   */
  auto calculateColumns = [](const std::vector<std::wstring> &filenames,
                             int terminalWidth) -> std::pair<int, int> {
    if (filenames.empty()) return {0, 0};

    size_t maxDisplayWidth = 0;
    for (const auto &name : filenames) {
      maxDisplayWidth = std::max(maxDisplayWidth, string_display_width(name));
    }

    // Column width = max display width + 2 spaces padding
    int columnWidth = static_cast<int>(maxDisplayWidth) + 2;
    if (columnWidth <= 0) columnWidth = 1;

    int columns = terminalWidth / columnWidth;
    if (columns < 1) columns = 1;

    int rows = (filenames.size() + columns - 1) / columns;
    return {columns, rows};
  };

  // FileInfo struct definition
  struct FileInfo {
    std::wstring name;
    WIN32_FIND_DATAW find_data;
    std::wstring perms;
    std::wstring size;
    std::wstring mtime;
    std::wstring owner;
    std::wstring group;
  };

  /**
   * @brief List directory contents
   * @param path Path to list
   * @param ctx Command context
   * @return true if listing succeeded, false on error
   */
  auto listDirectory = [&](const std::string &path) -> bool {
    std::wstring wpath = utf8_to_wstring(path);
    WIN32_FIND_DATAW find_data;
    HANDLE hFind = FindFirstFileW((wpath + L"\\*").c_str(), &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
      std::wstring wpath_str = utf8_to_wstring(path);
      safeErrorPrintLn(L"ls: cannot access '" + wpath_str +
                       L"': No such file or directory");
      return false;
    }

    std::vector<FileInfo> files;
    do {
      std::wstring filename(find_data.cFileName);
      if (!ctx.get<bool>("--all", false) && !ctx.get<bool>("-a", false) && (filename == L"." || filename == L".."))
        continue;

      FileInfo info;
      info.name = std::move(filename);
      info.find_data = find_data;
      info.perms = getPermissionsString(find_data);
      info.size = getFileSizeString(find_data);
      info.mtime = getModificationTimeString(find_data);

      // Get owner and group
      auto [owner, group] = getFileOwnerAndGroup();
      info.owner = owner;
      info.group = group;

      files.push_back(std::move(info));
    } while (FindNextFileW(hFind, &find_data) != 0);
    FindClose(hFind);

    // Sort files
    if (ctx.get<bool>("--reverse", false) || ctx.get<bool>("-r", false)) {
      std::sort(
          files.rbegin(), files.rend(),
          [](const FileInfo &a, const FileInfo &b) { return a.name > b.name; });
    } else {
      std::ranges::sort(
          files.begin(), files.end(),
          [](const FileInfo &a, const FileInfo &b) { return a.name < b.name; });
    }

    if (ctx.get<bool>("--all", false) || ctx.get<bool>("-l", false)) {
      // Calculate maximum widths
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

      // Print files with precise alignment
      for (const auto &file_info : files) {
        std::wstring line;

        // 1. Permissions and link count
        line += file_info.perms;
        line += L' ';
        line += L'1';  // Windows always has 1 link
        line += L' ';

        // 2. Owner (left-aligned)
        line += file_info.owner;
        int owner_padding = static_cast<int>(max_owner_len) -
                            static_cast<int>(file_info.owner.length());
        for (int i = 0; i < owner_padding; i++) line += L' ';
        line += L' ';

        // 3. Group (left-aligned)
        line += file_info.group;
        int group_padding = static_cast<int>(max_group_len) -
                            static_cast<int>(file_info.group.length());
        for (int i = 0; i < group_padding; i++) line += L' ';
        line += L' ';

        // 4. File size (right-aligned)
        int size_padding = static_cast<int>(max_size_len) -
                           static_cast<int>(file_info.size.length());
        for (int i = 0; i < size_padding; i++) line += L' ';
        line += file_info.size;
        line += L' ';

        // 5. Modification time + filename
        line += file_info.mtime;
        line += L' ';
        if (isTerminalSupportsColor()) {
          line += getFileColor(file_info.find_data);
        }
        line += file_info.name;
        if (isTerminalSupportsColor()) {
          line += COLOR_RESET;
        }

        safePrintLn(line);
      }
    } else {
      // Brief mode with columns
      std::vector<std::wstring> filenames;
      std::vector<std::wstring> colors;

      for (const auto &file_info : files) {
        filenames.push_back(file_info.name);
        colors.push_back(getFileColor(file_info.find_data));
      }

      // Calculate terminal width
      int terminalWidth = ctx.get<int>("--width", 0) > 0 ? ctx.get<int>("--width", 0) : getTerminalWidth();

      // Compute MAX DISPLAY WIDTH (not .length()!)
      size_t maxDisplayWidth = 0;
      for (const auto &name : filenames) {
        maxDisplayWidth = std::max(maxDisplayWidth, string_display_width(name));
      }

      // Avoid division by zero
      if (maxDisplayWidth == 0) maxDisplayWidth = 1;

      // Column layout
      int columnWidth =
          static_cast<int>(maxDisplayWidth) + 2;  // +2 for spacing
      int columns = terminalWidth / columnWidth;
      if (columns < 1) columns = 1;
      int rows = (filenames.size() + columns - 1) / columns;

      // Print in column-major order
      for (int row = 0; row < rows; ++row) {
        std::wstring row_output;
        for (int col = 0; col < columns; ++col) {
          int index = col * rows + row;
          if (index >= filenames.size()) break;

          // Apply color
          if (isTerminalSupportsColor()) {
            row_output += colors[index];
          }
          row_output += filenames[index];
          if (isTerminalSupportsColor()) {
            row_output += COLOR_RESET;
          }

          // Add padding based on DISPLAY WIDTH, not string length
          size_t currentWidth = string_display_width(filenames[index]);
          int paddingSpaces =
              static_cast<int>(maxDisplayWidth + 2 - currentWidth);
          if (paddingSpaces > 0) {
            row_output.append(paddingSpaces, L' ');
          }
        }
        safePrintLn(row_output);
      }
    }

    return true;
  };

  // Get paths from arguments
  std::vector<std::string> paths;
  for (const auto &arg : args) {
    paths.push_back(std::string(arg));
  }

  // Default to current directory if no paths specified
  if (paths.empty()) {
    paths.push_back(".");
  }

  int result = 0;

  // If there are multiple paths and we're in brief mode (not long format),
  // collect all files for columnar display
  if (paths.size() > 1 && !ctx.get<bool>("--all", false) && !ctx.get<bool>("-l", false)) {
    std::vector<std::wstring> filenames;
    std::vector<std::wstring> colors;

    for (size_t i = 0; i < paths.size(); ++i) {
      std::wstring wpath = utf8_to_wstring(paths[i]);

      // Check if path is a file
      WIN32_FIND_DATAW find_data;
      HANDLE hFind = FindFirstFileW(wpath.c_str(), &find_data);
      bool is_file = (hFind != INVALID_HANDLE_VALUE) &&
                     !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
      }

      if (is_file) {
        // Add file to the list
        filenames.push_back(find_data.cFileName);
        colors.push_back(getFileColor(find_data));
      } else {
        // If it's a directory, list its contents
        if (paths.size() > 1) {
          std::wstring wpath_str = utf8_to_wstring(paths[i]);
          safePrintLn(wpath_str + L":");
        }
        if (!listDirectory(paths[i])) {
          result = 1;  // Directory listing error
        }
      }
    }

    // If all paths were files, display them in columns
    if (!filenames.empty()) {
      // Calculate terminal width
      int terminalWidth = ctx.get<int>("--width", 0) > 0 ? ctx.get<int>("--width", 0) : getTerminalWidth();

      // Compute MAX DISPLAY WIDTH (not .length()!)
      size_t maxDisplayWidth = 0;
      for (const auto &name : filenames) {
        maxDisplayWidth = std::max(maxDisplayWidth, string_display_width(name));
      }

      // Avoid division by zero
      if (maxDisplayWidth == 0) maxDisplayWidth = 1;

      // Column layout
      int columnWidth =
          static_cast<int>(maxDisplayWidth) + 2;  // +2 for spacing
      int columns = terminalWidth / columnWidth;
      if (columns < 1) columns = 1;
      int rows = (filenames.size() + columns - 1) / columns;

      // Print in column-major order
      for (int row = 0; row < rows; ++row) {
        std::wstring row_output;
        for (int col = 0; col < columns; ++col) {
          int index = col * rows + row;
          if (index >= filenames.size()) break;

          // Apply color
          if (isTerminalSupportsColor()) {
            row_output += colors[index];
          }
          row_output += filenames[index];
          if (isTerminalSupportsColor()) {
            row_output += COLOR_RESET;
          }

          // Add padding based on DISPLAY WIDTH, not string length
          size_t currentWidth = string_display_width(filenames[index]);
          int paddingSpaces =
              static_cast<int>(maxDisplayWidth + 2 - currentWidth);
          if (paddingSpaces > 0) {
            row_output.append(paddingSpaces, L' ');
          }
        }
        safePrintLn(row_output);
      }
    }
  } else {
    // Handle single path or long format normally
    for (size_t i = 0; i < paths.size(); ++i) {
      std::wstring wpath = utf8_to_wstring(paths[i]);

      // Check if path is a file
      WIN32_FIND_DATAW find_data;
      HANDLE hFind = FindFirstFileW(wpath.c_str(), &find_data);
      bool is_file = (hFind != INVALID_HANDLE_VALUE) &&
                     !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
      }

      if (is_file) {
        // If it's a file, display it directly
        if (ctx.get<bool>("--all", false) || ctx.get<bool>("-l", false)) {
          // For long format, get file info
          HANDLE hFileFind = FindFirstFileW(wpath.c_str(), &find_data);
          if (hFileFind != INVALID_HANDLE_VALUE) {
            FileInfo info;
            info.name = find_data.cFileName;
            info.find_data = find_data;
            info.perms = getPermissionsString(find_data);
            info.size = getFileSizeString(find_data);
            info.mtime = getModificationTimeString(find_data);

            // Get owner and group
            auto [owner, group] = getFileOwnerAndGroup();
            info.owner = owner;
            info.group = group;

            // Calculate column widths
            size_t max_owner_len = info.owner.length();
            size_t max_group_len = info.group.length();
            size_t max_size_len = info.size.length();

            // Print with long format
            std::wstring line;
            line += info.perms;
            line += L' ';
            line += L'1';  // Windows always has 1 link
            line += L' ';
            line += info.owner;
            int owner_padding = static_cast<int>(max_owner_len) -
                                static_cast<int>(info.owner.length());
            for (int j = 0; j < owner_padding; j++) line += L' ';
            line += L' ';
            line += info.group;
            int group_padding = static_cast<int>(max_group_len) -
                                static_cast<int>(info.group.length());
            for (int j = 0; j < group_padding; j++) line += L' ';
            line += L' ';
            int size_padding = static_cast<int>(max_size_len) -
                               static_cast<int>(info.size.length());
            for (int j = 0; j < size_padding; j++) line += L' ';
            line += info.size;
            line += L' ';
            line += info.mtime;
            line += L' ';
            // Apply color to the line
            line += formatFilenameWithColor(find_data, info.name);
            safePrintLn(line);

            FindClose(hFileFind);
          }
        } else {
          // For brief mode, just print the filename
          safePrintLn(formatFilenameWithColor(find_data, find_data.cFileName));
        }
      } else {
        // If it's a directory, list its contents
        if (paths.size() > 1) {
          std::wstring wpath_str = utf8_to_wstring(paths[i]);
          safePrintLn(wpath_str + L":");
        }
        if (!listDirectory(paths[i])) {
          result = 1;  // Directory listing error
        }
      }
      if (i < paths.size() - 1 && !is_file) {
        safePrintLn(L"");
      }
    }
  }

  return result;
}

}  // namespace winux::commands

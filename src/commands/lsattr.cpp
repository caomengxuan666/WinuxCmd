/*
 *  Copyright © 2026 WinuxCmd
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: lsattr.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for lsattr.
/// @Version: 0.1.0
/// @License: MIT

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr LSATTR_OPTIONS = std::array{
    OPTION("-R", "--recursive", "recursively list attributes of directories"),
    OPTION("-d", "--directory", "list directories themselves, not contents")};

namespace lsattr_pipeline {

struct Config {
  bool recursive = false;
  bool directory = false;
  std::vector<std::string> paths;
};

auto format_attrs(DWORD attrs) -> std::string {
  std::string out;
  out.reserve(12);
  out.push_back((attrs & FILE_ATTRIBUTE_READONLY) != 0 ? 'R' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_HIDDEN) != 0 ? 'H' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_SYSTEM) != 0 ? 'S' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_ARCHIVE) != 0 ? 'A' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0 ? 'D' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0 ? 'C' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_ENCRYPTED) != 0 ? 'E' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) != 0 ? 'I' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_TEMPORARY) != 0 ? 'T' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_OFFLINE) != 0 ? 'O' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_SPARSE_FILE) != 0 ? 'P' : '-');
  out.push_back((attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0 ? 'L' : '-');
  return out;
}

auto print_one(std::string_view display_path) -> bool {
  auto operand = native_path::make_api_path_operand(display_path);
  DWORD attrs = GetFileAttributesW(operand.extended.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    safeErrorPrintLn("lsattr: cannot access '" + std::string(display_path) +
                     "': " + win32_posix_error_text(GetLastError()));
    return false;
  }

  safePrint(format_attrs(attrs));
  safePrint(" ");
  safePrintLn(std::string(display_path));
  return true;
}

auto join_child_path(std::wstring_view parent, std::wstring_view child)
    -> std::wstring {
  std::wstring out(parent);
  if (!out.empty() && out.back() != L'\\' && out.back() != L'/') {
    out.push_back(L'\\');
  }
  out.append(child);
  return out;
}

auto list_directory_children(std::wstring_view path, bool recursive, bool& ok)
    -> void {
  std::wstring pattern = join_child_path(path, L"*");
  WIN32_FIND_DATAW data{};
  UniqueFindHandle find(FindFirstFileW(pattern.c_str(), &data));
  if (!find) {
    DWORD error = GetLastError();
    if (error != ERROR_FILE_NOT_FOUND && error != ERROR_NO_MORE_FILES) {
      safeErrorPrintLn(L"lsattr: cannot read directory '" + std::wstring(path) +
                       L"': " + utf8_to_wstring(win32_posix_error_text(error)));
      ok = false;
    }
    return;
  }

  do {
    std::wstring name = data.cFileName;
    if (name == L"." || name == L"..") continue;

    std::wstring child = join_child_path(path, name);
    std::string display = wstring_to_utf8(child);
    safePrint(format_attrs(data.dwFileAttributes));
    safePrint(" ");
    safePrintLn(display);

    const bool is_dir = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    const bool is_link =
        (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
    if (recursive && is_dir && !is_link) {
      list_directory_children(child, true, ok);
    }
  } while (FindNextFileW(find.get(), &data));
}

auto list_path(const Config& cfg, std::string_view path) -> bool {
  auto operand = native_path::make_api_path_operand(path);
  DWORD attrs = GetFileAttributesW(operand.extended.c_str());
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    safeErrorPrintLn("lsattr: cannot access '" + std::string(path) +
                     "': " + win32_posix_error_text(GetLastError()));
    return false;
  }

  if ((attrs & FILE_ATTRIBUTE_DIRECTORY) == 0 || cfg.directory) {
    return print_one(path);
  }

  bool ok = true;
  list_directory_children(operand.extended, cfg.recursive, ok);
  return ok;
}

auto run(const CommandContext<LSATTR_OPTIONS.size()>& ctx) -> int {
  Config cfg;
  cfg.recursive =
      ctx.get<bool>("-R", false) || ctx.get<bool>("--recursive", false);
  cfg.directory =
      ctx.get<bool>("-d", false) || ctx.get<bool>("--directory", false);

  for (auto arg : ctx.positionals) cfg.paths.emplace_back(arg);
  if (cfg.paths.empty()) cfg.paths.emplace_back(".");

  bool ok = true;
  for (const auto& path : cfg.paths) {
    ok = list_path(cfg, path) && ok;
  }
  return ok ? 0 : 1;
}

}  // namespace lsattr_pipeline

REGISTER_COMMAND(lsattr, "lsattr", "lsattr [OPTION]... [FILE]...",
                 "List Windows file attributes in a stable flag string.\n"
                 "Flags are RHSADCEITOPL for readonly, hidden, system, "
                 "archive, directory, compressed, encrypted, not indexed, "
                 "temporary, offline, sparse, and reparse point.",
                 "  lsattr file.txt\n"
                 "  lsattr -d directory",
                 "chattr(1), stat(1), attrib.exe", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", LSATTR_OPTIONS) {
  return lsattr_pipeline::run(ctx);
}

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
 *  - File: rmdir.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

#include "pch/pch.h"
// include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr RMDIR_OPTIONS =
    std::array{OPTION("", "--ignore-fail-on-non-empty",
                      "ignore each failure to remove a non-empty directory"),
               OPTION("-p", "--parents", "remove DIRECTORY and its ancestors"),
               OPTION("-v", "--verbose",
                      "output a diagnostic for every directory processed")};

namespace rmdir_pipeline {
namespace cp = core::pipeline;

auto is_separator(char ch) -> bool { return ch == '\\' || ch == '/'; }

auto unc_root_length(std::string_view path) -> std::optional<size_t> {
  if (path.size() < 3 || !is_separator(path[0]) || !is_separator(path[1])) {
    return std::nullopt;
  }

  auto server_end = path.find_first_of("\\/", 2);
  if (server_end == std::string_view::npos) return path.size();

  auto share_start = server_end + 1;
  auto share_end = path.find_first_of("\\/", share_start);
  if (share_end == std::string_view::npos) return path.size();
  return share_end;
}

auto strip_trailing_separators(std::string path) -> std::string {
  while (path.size() > 1 && is_separator(path.back())) {
    if (path.size() == 3 && path[1] == ':') break;
    if (auto root_len = unc_root_length(path);
        root_len && path.size() <= *root_len + 1) {
      break;
    }
    path.pop_back();
  }
  return path;
}

auto parent_path(std::string path) -> std::string {
  path = strip_trailing_separators(std::move(path));
  if (path.empty()) return {};

  auto pos = path.find_last_of("\\/");
  if (pos == std::string::npos || pos == 0) return {};
  if (pos == 2 && path.size() >= 3 && path[1] == ':') return {};
  if (auto root_len = unc_root_length(path); root_len && pos <= *root_len) {
    return {};
  }

  auto parent = path.substr(0, pos);
  if (parent == "." || parent == "..") return {};
  return parent;
}

auto remove_one(const std::string& utf8_path, bool ignore_non_empty,
                bool verbose) -> bool {
  std::wstring wpath = utf8_to_wstring(utf8_path);
  if (verbose) {
    safePrint("rmdir: removing directory, '");
    safePrint(utf8_path);
    safePrint("'\n");
  }
  if (RemoveDirectoryW(wpath.c_str())) {
    return true;
  }

  DWORD e = GetLastError();
  if (e == ERROR_DIR_NOT_EMPTY && ignore_non_empty) return true;

  if (e == ERROR_PATH_NOT_FOUND || e == ERROR_FILE_NOT_FOUND) {
    safeErrorPrint("rmdir: failed to remove '");
    safeErrorPrint(utf8_path);
    safeErrorPrint("': No such file or directory\n");
    return false;
  }
  if (e == ERROR_DIRECTORY) {
    safeErrorPrint("rmdir: failed to remove '");
    safeErrorPrint(utf8_path);
    safeErrorPrint("': Not a directory\n");
    return false;
  }
  if (e == ERROR_DIR_NOT_EMPTY) {
    safeErrorPrint("rmdir: failed to remove '");
    safeErrorPrint(utf8_path);
    safeErrorPrint("': Directory not empty\n");
    return false;
  }

  safeErrorPrint("rmdir: failed to remove '");
  safeErrorPrint(utf8_path);
  safeErrorPrint("'\n");
  return false;
}

auto process_command(const CommandContext<RMDIR_OPTIONS.size()>& ctx)
    -> cp::Result<bool> {
  if (ctx.positionals.empty()) return std::unexpected("missing operand");

  bool parents =
      ctx.get<bool>("--parents", false) || ctx.get<bool>("-p", false);
  bool verbose =
      ctx.get<bool>("--verbose", false) || ctx.get<bool>("-v", false);
  bool ignore_non_empty = ctx.get<bool>("--ignore-fail-on-non-empty", false);

  bool ok_all = true;

  for (auto arg : ctx.positionals) {
    std::string cur(arg);
    if (!remove_one(cur, ignore_non_empty, verbose)) {
      ok_all = false;
      continue;
    }

    if (!parents) continue;

    while (true) {
      std::string p = parent_path(cur);
      if (p.empty()) break;

      if (!remove_one(p, ignore_non_empty, verbose)) {
        ok_all = false;
        break;
      }
      cur = p;
    }
  }

  return ok_all;
}
}  // namespace rmdir_pipeline

REGISTER_COMMAND(rmdir, "rmdir", "rmdir [OPTION]... DIRECTORY...",
                 "Remove the DIRECTORY(ies), if they are empty.",
                 "  rmdir dir\n"
                 "  rmdir -p a/b/c\n"
                 "  rmdir --ignore-fail-on-non-empty dir",
                 "mkdir(1), rm(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
                 RMDIR_OPTIONS) {
  using namespace rmdir_pipeline;
  auto result = process_command(ctx);
  if (!result) {
    if (result.error() == "missing operand") {
      safeErrorPrintLn("rmdir: missing operand");
      safeErrorPrintLn("Try 'rmdir --help' for more information.");
      return 1;
    }
    cp::report_error(result, L"rmdir");
    return 1;
  }
  return *result ? 0 : 1;
}

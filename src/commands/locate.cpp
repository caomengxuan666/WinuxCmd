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
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: locate.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for locate (find files by name in database).
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr LOCATE_OPTIONS =
    std::array{OPTION("-b", "--basename", "match on filename, not full path"),
               OPTION("-c", "--count", "count matching entries"),
               OPTION("-d", "--database", "use given database", STRING_TYPE),
               OPTION("-e", "", "limit search to database's root directory"),
               OPTION("-i", "--ignore-case", "ignore case in search"),
               OPTION("-l", "", "ignore case (alias for -i)"),
               OPTION("-m", "", "print more than one match"),
               OPTION("-n", "--limit", "maximum number of results", INT_TYPE),
               OPTION("-P", "--no-pager", "don't pipe through pager"),
               OPTION("-r", "", "treat pattern as basic regex"),
               OPTION("-S", "--stat", "print database info to stderr"),
               OPTION("-s", "", "suppress error messages"),
               OPTION("-t", "--type", "restrict by file type", STRING_TYPE),
               OPTION("-w", "--wholenew", "match whole filename"),
               OPTION("-q", "--quiet", "quiet mode (like -s)")};

namespace locate_pipeline {
namespace cp = core::pipeline;

struct Config {
  bool basename = false;
  bool count = false;
  bool ignore_case = false;
  bool quiet = false;
  int limit = 0;
  std::string database;
  SmallVector<std::string, 8> patterns;
};

auto get_env_utf8(const wchar_t* key) -> std::optional<std::string> {
  DWORD size = GetEnvironmentVariableW(key, nullptr, 0);
  if (size == 0) return std::nullopt;
  std::wstring value;
  value.resize(size - 1);
  if (GetEnvironmentVariableW(key, value.data(), size) == 0)
    return std::nullopt;
  return wstring_to_utf8(value);
}

auto get_default_database() -> std::string {
  if (auto p = get_env_utf8(L"LOCALAPPDATA")) {
    return *p + "/WinuxCmd/locate.db";
  }
  return "locate.db";
}

auto build_config(const CommandContext<LOCATE_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  cfg.basename = ctx.has("-b") || ctx.has("--basename");
  cfg.count = ctx.has("-c") || ctx.has("--count");
  cfg.ignore_case = ctx.has("-i") || ctx.has("--ignore-case") || ctx.has("-l");
  cfg.quiet = ctx.has("-q") || ctx.has("--quiet") || ctx.has("-s");
  if (ctx.has("-n")) {
    cfg.limit = ctx.get<int>("-n", 0);
  }
  if (ctx.has("-d")) {
    cfg.database = ctx.get<std::string>("-d", "");
  } else if (ctx.has("--database")) {
    cfg.database = ctx.get<std::string>("--database", "");
  }
  if (cfg.database.empty()) cfg.database = get_default_database();
  for (auto arg : ctx.positionals) cfg.patterns.push_back(std::string(arg));
  if (cfg.patterns.empty()) return std::unexpected("missing operand");
  return cfg;
}

auto read_db(const std::string& db_path, std::vector<std::string>& lines)
    -> bool {
  std::wstring wpath = utf8_to_wstring(db_path);
  HANDLE h = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h == INVALID_HANDLE_VALUE) return false;
  LARGE_INTEGER sz{};
  if (!GetFileSizeEx(h, &sz) || sz.QuadPart < 0) {
    CloseHandle(h);
    return false;
  }
  std::string data(static_cast<size_t>(sz.QuadPart), '\0');
  DWORD got = 0;
  BOOL ok = TRUE;
  if (!data.empty()) {
    ok = ReadFile(h, data.data(), static_cast<DWORD>(data.size()), &got,
                  nullptr);
  }
  CloseHandle(h);
  if (!ok) return false;
  data.resize(got);
  size_t start = 0;
  while (start <= data.size()) {
    size_t nl = data.find('\n', start);
    if (nl == std::string::npos) {
      std::string line = data.substr(start);
      if (!line.empty()) lines.push_back(std::move(line));
      break;
    }
    std::string line = data.substr(start, nl - start);
    if (!line.empty()) lines.push_back(std::move(line));
    start = nl + 1;
  }
  return true;
}

auto to_lower(std::string_view s) -> std::string {
  std::string out(s);
  for (char& c : out)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return out;
}

auto run(const Config& cfg) -> int {
  std::vector<std::string> db_lines;
  if (!read_db(cfg.database, db_lines)) {
    if (!cfg.quiet) {
      safeErrorPrintLn("locate: cannot open database '" + cfg.database + "'");
      safeErrorPrintLn("Use 'updatedb' to build the database first.");
    }
    return 1;
  }

  int total = 0;
  int printed = 0;
  for (const auto& pattern : cfg.patterns) {
    std::string search = cfg.ignore_case ? to_lower(pattern) : pattern;
    for (const auto& line : db_lines) {
      std::string check = line;
      if (cfg.basename) {
        size_t pos = check.find_last_of("/\\");
        if (pos != std::string::npos) check = check.substr(pos + 1);
      }
      if (cfg.ignore_case) check = to_lower(check);

      bool match = false;
      if (cfg.ignore_case) {
        match = check.find(search) != std::string::npos;
      } else {
        match = check.find(search) != std::string::npos;
      }
      if (match) {
        total++;
        if (printed < cfg.limit || cfg.limit == 0) {
          safePrintLn(line);
          printed++;
        }
        if (cfg.limit > 0 && printed >= cfg.limit) break;
      }
    }
    if (cfg.count) break;
  }

  if (cfg.count) {
    safePrint(total);
    safePrintLn("");
  }
  return 0;
}

}  // namespace locate_pipeline

REGISTER_COMMAND(locate, "locate", "locate [OPTION]... NAME...",
                 "Search a database of file paths for matching names.\n"
                 "Run 'updatedb' first to build the database.",
                 "  locate myprogram\n"
                 "  locate -i readme\n"
                 "  locate -c -i test",
                 "updatedb(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
                 LOCATE_OPTIONS) {
  using namespace locate_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    if (cfg_result.error() == "missing operand") {
      safeErrorPrintLn("locate: missing operand");
      safeErrorPrintLn("Try 'locate --help' for more information.");
      return 1;
    }
    cp::report_error(cfg_result, L"locate");
    return 1;
  }

  return run(*cfg_result);
}

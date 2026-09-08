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
 *  - File: updatedb.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for updatedb (update the locate database).
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

auto constexpr UPDATEDB_OPTIONS = std::array{
    OPTION("-d", "--directory", "directory to start walking", STRING_TYPE),
    OPTION("-e", "--exclude", "prune entries matching pattern", STRING_TYPE),
    OPTION("-f", "", "force walk even if DB is new", BOOL_TYPE),
    OPTION("-g", "--group", "only include files with this group", STRING_TYPE),
    OPTION("-o", "--output", "database output file", STRING_TYPE),
    OPTION("-r", "", "only walk directories owned by root", BOOL_TYPE),
    OPTION("-s", "", "create compacter (but slower) database", BOOL_TYPE),
    OPTION("-u", "--user", "only include files with this user", STRING_TYPE),
    OPTION("-v", "--verbose", "be verbose", BOOL_TYPE),
    OPTION("-x", "--exclude-dir", "directory pattern to exclude", STRING_TYPE)};

namespace updatedb_pipeline {
namespace cp = core::pipeline;

struct Config {
  std::string directory = ".";
  std::string output;
  bool verbose = false;
  bool compact = false;
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

auto build_config(const CommandContext<UPDATEDB_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  if (ctx.has("-d")) cfg.directory = ctx.get<std::string>("-d", ".");
  if (ctx.has("--directory"))
    cfg.directory = ctx.get<std::string>("--directory", ".");
  if (ctx.has("-o")) cfg.output = ctx.get<std::string>("-o", "");
  if (ctx.has("--output")) cfg.output = ctx.get<std::string>("--output", "");
  cfg.verbose = ctx.has("-v") || ctx.has("--verbose");
  cfg.compact = ctx.has("-s");
  if (cfg.output.empty()) cfg.output = get_default_database();
  return cfg;
}

// Recursively walk a directory and collect all file/directory paths.
auto walk_dir(const std::string& dir, std::vector<std::string>& results)
    -> void {
  std::wstring pattern = utf8_to_wstring(dir) + L"\\*";
  WIN32_FIND_DATAW fd;
  HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
  if (h == INVALID_HANDLE_VALUE) return;

  do {
    std::wstring name(fd.cFileName);
    if (name == L"." || name == L"..") continue;
    std::string utf8name = wstring_to_utf8(name);

    std::string full = dir;
    if (full.back() != '/' && full.back() != '\\') full += "/";
    full += utf8name;

    results.push_back(full);

    if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      walk_dir(full, results);
    }
  } while (FindNextFileW(h, &fd));
  FindClose(h);
}

auto run(const Config& cfg) -> int {
  std::vector<std::string> paths;
  if (cfg.verbose) {
    safePrintLn("Walking " + cfg.directory + "...");
  }

  walk_dir(cfg.directory, paths);

  if (cfg.verbose) {
    safePrint("Found " + std::to_string(paths.size()) + " paths. ");
  }

  // Sort
  std::sort(paths.begin(), paths.end());

  // Write database
  std::wstring woutput = utf8_to_wstring(cfg.output);
  // Create parent directory if needed
  {
    size_t last_slash = woutput.find_last_of(L"/\\");
    if (last_slash != std::wstring::npos) {
      std::wstring parent = woutput.substr(0, last_slash);
      CreateDirectoryW(parent.c_str(), nullptr);
    }
  }

  HANDLE h =
      CreateFileW(woutput.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h == INVALID_HANDLE_VALUE) {
    safeErrorPrintLn("updatedb: cannot create database '" + cfg.output + "'");
    return 1;
  }

  std::string content;
  for (const auto& p : paths) {
    content += p;
    content += "\n";
  }

  DWORD written = 0;
  BOOL ok = WriteFile(h, content.data(), static_cast<DWORD>(content.size()),
                      &written, nullptr);
  CloseHandle(h);
  if (!ok || written != static_cast<DWORD>(content.size())) {
    safeErrorPrintLn("updatedb: failed to write database");
    return 1;
  }

  if (cfg.verbose) {
    safePrint("Database written to " + cfg.output + ".");
    safePrintLn("");
  } else {
    safePrintLn("Database created at " + cfg.output);
  }
  return 0;
}

}  // namespace updatedb_pipeline

REGISTER_COMMAND(updatedb, "updatedb", "updatedb [OPTION]...",
                 "Update the filename database used by locate.\n"
                 "Walks the filesystem and writes a sorted path list.",
                 "  updatedb\n"
                 "  updatedb -d /path\n"
                 "  updatedb -d C:\Windows -v",
                 "locate(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
                 UPDATEDB_OPTIONS) {
  using namespace updatedb_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"updatedb");
    return 1;
  }

  return run(*cfg_result);
}

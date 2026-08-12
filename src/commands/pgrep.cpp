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
 *  - File: pgrep.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for pgrep.
/// @Version: 0.1.0
/// @License: MIT

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr PGREP_OPTIONS = std::array{
    OPTION("-f", "--full", "match against full command line"),
    OPTION("-i", "--ignore-case", "match case insensitively"),
    OPTION("-l", "--list-name", "list PID and process name"),
    OPTION("-a", "--list-full", "list PID and full command line"),
    OPTION("-x", "--exact", "match the whole name or command line"),
    OPTION("-c", "--count", "print only a count of matching processes")};

namespace pgrep_pipeline {

struct Config {
  bool full = false;
  bool ignore_case = false;
  bool list_name = false;
  bool list_full = false;
  bool exact = false;
  bool count = false;
  std::wstring pattern;
};

auto matches(const Win32ProcessInfo& proc, const Config& cfg) -> bool {
  std::wstring haystack = cfg.full ? proc.command_line : proc.name;
  std::wstring pattern = cfg.pattern;
  if (!cfg.full) {
    haystack = win32_basename_without_exe(haystack);
  }
  if (cfg.ignore_case) {
    haystack = ascii_lower_copy(haystack);
    pattern = ascii_lower_copy(pattern);
  }

  if (cfg.exact) return haystack == pattern;
  return haystack.find(pattern) != std::wstring::npos;
}

auto build_config(const CommandContext<PGREP_OPTIONS.size()>& ctx)
    -> std::optional<Config> {
  Config cfg;
  cfg.full = ctx.get<bool>("-f", false) || ctx.get<bool>("--full", false);
  cfg.ignore_case =
      ctx.get<bool>("-i", false) || ctx.get<bool>("--ignore-case", false);
  cfg.list_name =
      ctx.get<bool>("-l", false) || ctx.get<bool>("--list-name", false);
  cfg.list_full =
      ctx.get<bool>("-a", false) || ctx.get<bool>("--list-full", false);
  cfg.exact = ctx.get<bool>("-x", false) || ctx.get<bool>("--exact", false);
  cfg.count = ctx.get<bool>("-c", false) || ctx.get<bool>("--count", false);

  if (ctx.positionals.size() != 1) return std::nullopt;
  cfg.pattern = utf8_to_wstring(std::string(ctx.positionals[0]));
  return cfg;
}

auto run(const Config& cfg) -> int {
  auto processes = enumerate_win32_processes();
  const DWORD self = GetCurrentProcessId();
  std::vector<const Win32ProcessInfo*> matched;
  for (const auto& proc : processes) {
    if (proc.pid == self) continue;
    if (matches(proc, cfg)) matched.push_back(&proc);
  }

  if (cfg.count) {
    safePrintLn(std::to_string(matched.size()));
    return matched.empty() ? 1 : 0;
  }

  for (const auto* proc : matched) {
    if (cfg.list_full) {
      safePrintLn(std::to_wstring(proc->pid) + L" " + proc->command_line);
    } else if (cfg.list_name) {
      safePrintLn(std::to_wstring(proc->pid) + L" " + proc->name);
    } else {
      safePrintLn(std::to_wstring(proc->pid));
    }
  }

  return matched.empty() ? 1 : 0;
}

}  // namespace pgrep_pipeline

REGISTER_COMMAND(pgrep, "pgrep", "pgrep [OPTION]... PATTERN",
                 "Look up processes by name or command line.\n"
                 "The current pgrep process is excluded from matches.",
                 "  pgrep explorer\n"
                 "  pgrep -af powershell",
                 "ps(1), pkill(1), pidof(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", PGREP_OPTIONS) {
  using namespace pgrep_pipeline;

  auto cfg = build_config(ctx);
  if (!cfg) {
    safeErrorPrintLn("pgrep: exactly one PATTERN is required");
    safeErrorPrintLn("Try 'pgrep --help' for more information.");
    return 2;
  }
  return run(*cfg);
}

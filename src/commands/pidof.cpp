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
 *  - File: pidof.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for pidof.
/// @Version: 0.1.0
/// @License: MIT

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr PIDOF_OPTIONS =
    // [EXT]
    std::array{OPTION("-x", "--exact", "exact match only")};

namespace pidof_pipeline {

auto process_matches(const Win32ProcessInfo& proc, std::wstring_view wanted,
                     bool exact) -> bool {
  auto name = win32_basename_without_exe(proc.name);
  if (ascii_iequals(name, wanted) || ascii_iequals(proc.name, wanted)) {
    return true;
  }
  if (exact) return false;

  auto command = win32_basename_without_exe(proc.command_line);
  return ascii_iequals(command, wanted) ||
         ascii_contains_ci(proc.command_line, wanted);
}

auto run(const CommandContext<PIDOF_OPTIONS.size()>& ctx) -> int {
  if (ctx.positionals.empty()) {
    safeErrorPrintLn("pidof: missing program name");
    safeErrorPrintLn("Try 'pidof --help' for more information.");
    return 1;
  }

  bool exact = ctx.get<bool>("-x", false) || ctx.get<bool>("--exact", false);
  auto processes = enumerate_win32_processes(exact);
  DWORD self = GetCurrentProcessId();
  std::vector<DWORD> pids;

  for (auto raw_name : ctx.positionals) {
    std::wstring wanted =
        win32_basename_without_exe(utf8_to_wstring(std::string(raw_name)));
    for (const auto& proc : processes) {
      if (proc.pid == self) continue;
      if (process_matches(proc, wanted, exact)) pids.push_back(proc.pid);
    }
  }

  if (pids.empty()) return 1;

  for (size_t i = 0; i < pids.size(); ++i) {
    if (i != 0) safePrint(" ");
    safePrint(std::to_string(pids[i]));
  }
  safePrintLn("");
  return 0;
}

}  // namespace pidof_pipeline

REGISTER_COMMAND(pidof, "pidof", "pidof [OPTION]... PROGRAM...",
                 "Find the process ID of a running program.",
                 "  pidof explorer\n"
                 "  pidof -x powershell",
                 "pgrep(1), ps(1), kill(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", PIDOF_OPTIONS) {
  return pidof_pipeline::run(ctx);
}

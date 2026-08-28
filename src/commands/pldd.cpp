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
 *  - File: pldd.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for pldd.
/// @Version: 0.1.0
/// @License: MIT

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr PLDD_OPTIONS = std::array{
    // [EXT]
    OPTION("-n", "--name", "print module basenames instead of paths")};

namespace pldd_pipeline {

auto run(const CommandContext<PLDD_OPTIONS.size()>& ctx) -> int {
  if (ctx.positionals.size() != 1) {
    safeErrorPrintLn("pldd: exactly one PID is required");
    safeErrorPrintLn("Try 'pldd --help' for more information.");
    return 1;
  }

  auto pid = win32_parse_pid(ctx.positionals.front());
  if (!pid) {
    safeErrorPrintLn("pldd: invalid process ID '" +
                     std::string(ctx.positionals.front()) + "'");
    return 1;
  }

  auto modules = enumerate_win32_modules(*pid);
  if (!modules) {
    safeErrorPrintLn("pldd: " + std::to_string(*pid) + ": " + modules.error());
    return 1;
  }

  bool names_only =
      ctx.get<bool>("-n", false) || ctx.get<bool>("--name", false);
  for (const auto& module : *modules) {
    safePrintLn(names_only ? module.name : module.path);
  }
  return 0;
}

}  // namespace pldd_pipeline

REGISTER_COMMAND(pldd, "pldd", "pldd [OPTION]... PID",
                 "List DLLs and executable modules loaded by a process.",
                 "  pldd 1234\n"
                 "  pldd --name 1234",
                 "lsof(1), ps(1), pgrep(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", PLDD_OPTIONS) {
  return pldd_pipeline::run(ctx);
}

/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr RESET_OPTIONS = std::array{
    OPTION("-I", "", "no initialization strings", BOOL_TYPE),
    OPTION("-q", "", "display term only, do no changes", BOOL_TYPE),
    OPTION("-Q", "", "do not output control key settings", BOOL_TYPE),
    OPTION("-r", "", "display term on stderr", BOOL_TYPE),
    OPTION("-s", "", "output TERM set command", BOOL_TYPE),
    OPTION("-V", "", "print curses-version", BOOL_TYPE)};

REGISTER_COMMAND(reset, "reset", "reset [options] [terminal]",
                 "Reset terminal to its default state.",
                 "  reset\n  reset -q\n  reset -V", "tput(1), clear(1)",
                 "WinuxCmd", "Copyright © 2026 WinuxCmd", RESET_OPTIONS) {
  if (ctx.get<bool>("-V", false)) {
    safePrintLn("ncurses 6.6.20251230");
    return 0;
  }

  const bool quiet = ctx.get<bool>("-q", false);
  const bool initialize = ctx.get<bool>("-I", false);
  const bool shell_output = ctx.get<bool>("-s", false);
  const bool report_stderr = ctx.get<bool>("-r", false);

  if (shell_output) {
    safePrintLn("TERM=xterm;");
    return 0;
  }

  if (report_stderr && !ctx.positionals.empty()) {
    safeErrorPrintLn(std::string(ctx.positionals.front()));
  }

  if (!quiet) {
    safePrint("\033c");
  }

  if (initialize) {
    safePrint("\033[?25h");
    safePrint("\033[0m");
  }

  return 0;
}

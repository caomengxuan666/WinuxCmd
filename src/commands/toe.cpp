/*
 *  Copyright © 2026 WinuxCmd
 */
#include "core/command_macros.h"
#include "pch/pch.h"
import std;
import core;
import utils;
import container;

auto constexpr TOE_OPTIONS =
    std::array{OPTION("-a", "", "list all terminal databases", BOOL_TYPE),
               OPTION("-h", "", "list with a heading", BOOL_TYPE),
               OPTION("-s", "", "sort by terminal name", BOOL_TYPE),
               OPTION("-u", "", "write direct dependencies", STRING_TYPE),
               OPTION("-U", "", "write reverse dependencies", STRING_TYPE),
               OPTION("-v", "", "set verbosity level", OPTIONAL_INT_TYPE),
               OPTION("-V", "", "print version", BOOL_TYPE)};

REGISTER_COMMAND(toe, "toe", "toe [-ahsuUV] [-v n] [file...]",
                 "Table of terminfo entries.", "toe\ntoe -V", "infocmp",
                 "WinuxCmd", "Copyright © 2026 WinuxCmd", TOE_OPTIONS) {
  if (ctx.get<bool>("-V", false)) {
    safePrintLn("ncurses 6.6.20251230");
    return 0;
  }
  if (ctx.get<bool>("-h", false)) {
    safePrintLn("Terminfo Entries");
  }
  safePrintLn("windows-ansi Windows ANSI/VT console");
  safePrintLn("xterm        xterm terminal emulator");
  safePrintLn("vt100        DEC VT100");
  safePrintLn("ansi         ANSI/VT100 terminal");
  return 0;
}

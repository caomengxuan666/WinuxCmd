/*
 *  Copyright © 2026 WinuxCmd
 */
#include "core/command_macros.h"
#include "pch/pch.h"
import std;
import core;
import utils;
import container;

auto constexpr TIC_OPTIONS = std::array{
    OPTION("-V", "", "print version", BOOL_TYPE),
    OPTION("-a", "", "retain commented-out capabilities", BOOL_TYPE),
    OPTION("-c", "", "check only", BOOL_TYPE),
    OPTION("-o", "", "set output directory", STRING_TYPE),
    OPTION("-v", "", "set verbosity level", OPTIONAL_INT_TYPE),
    OPTION("-x", "", "treat unknown capabilities as user-defined", BOOL_TYPE)};

REGISTER_COMMAND(tic, "tic",
                 "tic [-e names] [-o dir] [-R name] [-v[n]] [-V] [-w[n]] "
                 "[-1aCDcfGgIKLNrsTtUx] source-file",
                 "Terminfo compiler.", "tic myterm.ti", "infocmp", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", TIC_OPTIONS) {
  if (ctx.get<bool>("-V", false)) {
    safePrintLn("ncurses 6.6.20251230");
    return 0;
  }
  if (ctx.positionals.empty()) {
    safeErrorPrintLn("tic: File name needed.  Usage:");
    safeErrorPrintLn(
        "\ttic [-e names] [-o dir] [-R name] [-v[n]] [-V] [-w[n]] "
        "[-1aCDcfGgIKLNrsTtUx] source-file");
    return 1;
  }
  safePrintLn("tic: terminfo compilation not supported on Windows");
  return 0;
}

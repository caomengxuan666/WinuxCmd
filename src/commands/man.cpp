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
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: man.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for man command.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "core/command_macros.h"
#include "pch/pch.h"
import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr MAN_OPTIONS = std::array{
    OPTION("-l", "--list", "list all available commands"),
};

REGISTER_COMMAND(man,
                 /* cmd_name */ "man",
                 /* cmd_synopsis */ "man [OPTION]... [COMMAND]...",
                 /* cmd_desc */ "Display manual page for a WinuxCmd command.",
                 /* examples */ "man ls\nman grep\nman --list",
                 /* see_also */ "help(1)",
                 /* author */ "WinuxCmd",
                 /* copyright */ "Copyright © 2026 WinuxCmd",
                 /* options */ MAN_OPTIONS) {
  bool list_mode = ctx.get<bool>("-l", false) || ctx.get<bool>("--list", false);

  if (list_mode) {
    auto commands = CommandRegistry::getAllCommands();
    std::string output = "Available commands:\n";
    for (const auto &[name, desc] : commands) {
      output += "  ";
      output.append(name.data(), name.size());
      output += "  - ";
      output.append(desc.data(), desc.size());
      output += "\n";
    }
    return winux::pager::page_text(
        output, winux::pager::Options{.title = "man --list"});
  }

  if (ctx.positionals.empty()) {
    safePrintLn("Usage: man [OPTION]... [COMMAND]...");
    safePrintLn("Display manual page for a WinuxCmd command.");
    safePrintLn("");
    safePrintLn("Options:");
    safePrintLn("  -l, --list    list all available commands");
    safePrintLn("");
    safePrintLn("Examples:");
    safePrintLn("  man ls        Show manual page for ls");
    safePrintLn("  man --list    List all commands");
    return 0;
  }

  std::string cmd_name(ctx.positionals[0]);
  auto man_page = CommandRegistry::getManPage(cmd_name);

  if (man_page.empty()) {
    safePrintLn(L"man: no manual entry for " +
                std::wstring(cmd_name.begin(), cmd_name.end()));
    safePrintLn(L"Try 'man --list' to see available commands.");
    return 1;
  }

  if (!man_page.ends_with('\n')) man_page += "\n";
  return winux::pager::page_text(
      man_page, winux::pager::Options{.title = "man " + cmd_name});
}

/*
 *  Copyright © 2026 [caomengxuan666]
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
 *  - File: kill.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
/// @Description: Implementation for kill - terminate a process
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
//include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
import container;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;
using namespace std::string_view_literals;

/**
 * @brief KILL command options definition
 *
 * This array defines all the options supported by the kill command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -l, @a --list: list signal names [IMPLEMENTED]
 * - @a -s, @a --signal: send specified signal [TODO]
 * - @a -f, @a --force: force termination [IMPLEMENTED]
 */
auto constexpr KILL_OPTIONS = std::array{
    OPTION("-l", "--list", "list signal names"),
    OPTION("-s", "--signal", "send specified signal [TODO]", STRING_TYPE),
    OPTION("-f", "--force", "force termination")
};

// ======================================================
// Constants
// ======================================================
namespace kill_constants {
// Map signal names to Windows exit codes - using compile-time ConstexprMap
constexpr auto signal_map = make_constexpr_map(std::to_array<std::pair<std::string_view, int>>({
    {"0"sv, 0},
    {"HUP"sv, 1},
    {"INT"sv, 2},
    {"QUIT"sv, 3},
    {"ILL"sv, 4},
    {"TRAP"sv, 5},
    {"ABRT"sv, 6},
    {"BUS"sv, 7},
    {"FPE"sv, 8},
    {"KILL"sv, 9},
    {"USR1"sv, 10},
    {"SEGV"sv, 11},
    {"USR2"sv, 12},
    {"PIPE"sv, 13},
    {"ALRM"sv, 14},
    {"TERM"sv, 15},
    {"STKFLT"sv, 16},
    {"CHLD"sv, 17},
    {"CONT"sv, 18},
    {"STOP"sv, 19},
    {"TSTP"sv, 20},
    {"TTIN"sv, 21},
    {"TTOU"sv, 22},
    {"URG"sv, 23},
    {"XCPU"sv, 24},
    {"XFSZ"sv, 25},
    {"VTALRM"sv, 26},
    {"PROF"sv, 27},
    {"WINCH"sv, 28},
    {"IO"sv, 29},
    {"PWR"sv, 30},
    {"SYS"sv, 31},
}));
}  // namespace kill_constants

// ======================================================
// Pipeline components
// ======================================================
namespace kill_pipeline {
namespace cp = core::pipeline;

/**
 * @brief List available signal names
 */
auto list_signals() -> void {
  safePrintLn(L"Signal    Value");
  safePrintLn(L"------    -----");
  
  // Print signals in order - use SmallVector for stack-allocated storage
  SmallVector<std::pair<std::string, int>, 32> signals = {
    {"HUP", 1}, {"INT", 2}, {"QUIT", 3}, {"ILL", 4}, {"TRAP", 5},
    {"ABRT", 6}, {"BUS", 7}, {"FPE", 8}, {"KILL", 9}, {"USR1", 10},
    {"SEGV", 11}, {"USR2", 12}, {"PIPE", 13}, {"ALRM", 14}, {"TERM", 15},
    {"STKFLT", 16}, {"CHLD", 17}, {"CONT", 18}, {"STOP", 19}, {"TSTP", 20},
    {"TTIN", 21}, {"TTOU", 22}, {"URG", 23}, {"XCPU", 24}, {"XFSZ", 25},
    {"VTALRM", 26}, {"PROF", 27}, {"WINCH", 28}, {"IO", 29}, {"PWR", 30},
    {"SYS", 31}
  };
  
  for (size_t i = 0; i < signals.size(); ++i) {
    const auto& [name, value] = signals[i];
    char buf[32];
    snprintf(buf, sizeof(buf), "%-10s %6d\n", name.c_str(), value);
    safePrint(buf);
  }
}

/**
 * @brief Terminate a process by PID
 * @param pid Process ID
 * @param force Force termination
 * @return True if successful
 */
auto terminate_process(uint32_t pid, bool force) -> bool {
  // Try to open the process with appropriate permissions
  DWORD access = PROCESS_TERMINATE;
  if (force) {
    access |= PROCESS_QUERY_INFORMATION;
  }
  
  HANDLE hProcess = OpenProcess(access, FALSE, pid);
  if (hProcess == nullptr) {
    DWORD error = GetLastError();
    if (error == ERROR_ACCESS_DENIED) {
      safeErrorPrint("kill: operation not permitted\n");
    } else if (error == ERROR_INVALID_PARAMETER) {
      safeErrorPrint("kill: process does not exist\n");
    } else {
      safeErrorPrint("kill: failed to open process: ");
      char buf[32];
      snprintf(buf, sizeof(buf), "%d\n", error);
      safeErrorPrint(buf);
    }
    return false;
  }
  
  // Terminate the process
  if (!TerminateProcess(hProcess, 1)) {
    DWORD error = GetLastError();
    safeErrorPrint("kill: failed to terminate process: ");
    char buf[32];
    snprintf(buf, sizeof(buf), "%d\n", error);
    safeErrorPrint(buf);
    CloseHandle(hProcess);
    return false;
  }
  
  CloseHandle(hProcess);
  return true;
}

/**
 * @brief Parse signal name or number
 * @param signal Signal name or number
 * @return Exit code or nullopt if invalid
 */
auto parse_signal(const std::string& signal) -> std::optional<int> {
  // Try as number first
  try {
    return std::stoi(signal);
  } catch (...) {
    // Try as signal name using ConstexprMap
    std::string_view signal_sv(signal);
    auto result = kill_constants::signal_map.get_or(signal_sv, -1);
    if (result != -1) {
      return result;
    }
  }
  
  return std::nullopt;
}

/**
 * @brief Process kill command
 * @param ctx Command context
 * @return Result with success status
 */
auto process_kill(const CommandContext<KILL_OPTIONS.size()>& ctx)
    -> cp::Result<bool> {
  bool list = ctx.get<bool>("--list", false) || ctx.get<bool>("-l", false);
  bool force = ctx.get<bool>("--force", false) || ctx.get<bool>("-f", false);
  
  // List signals mode
  if (list) {
    if (ctx.positionals.empty()) {
      list_signals();
      return true;
    } else {
      // List signal for each argument
      for (const auto& arg : ctx.positionals) {
        auto signal = parse_signal(std::string(arg));
        if (signal) {
          char buf[32];
          snprintf(buf, sizeof(buf), "%d\n", *signal);
          safePrint(buf);
        } else {
          safeErrorPrint("kill: invalid signal name: ");
          safeErrorPrint(arg);
          safeErrorPrintLn(L"");
        }
      }
      return true;
    }
  }
  
  // Kill mode - need at least one PID
  if (ctx.positionals.empty()) {
    safeErrorPrint("kill: missing operand\n");
    safeErrorPrint("usage: kill [-l] [-s signal] pid ...\n");
    return false;
  }
  
  bool all_ok = true;
  
  for (const auto& arg : ctx.positionals) {
    try {
      uint32_t pid = std::stoul(std::string(arg));
      
      // Don't allow killing critical system processes
      if (pid < 8) {
        safeErrorPrint("kill: cannot kill critical system process: ");
        safeErrorPrint(arg);
        safeErrorPrintLn(L"");
        all_ok = false;
        continue;
      }
      
      if (terminate_process(pid, force)) {
        safePrint("Killed process: ");
        safePrintLn(arg);
      } else {
        all_ok = false;
      }
    } catch (...) {
      safeErrorPrint("kill: invalid process ID: ");
      safeErrorPrint(arg);
      safeErrorPrintLn(L"");
      all_ok = false;
    }
  }

  return all_ok;
}

}  // namespace kill_pipeline

REGISTER_COMMAND(kill,
                 /* name */
                 "kill",

                 /* synopsis */
                 "terminate a process",

                 /* description */
                 "The kill command sends a signal to a process, causing it to terminate.\n\n"
                 "On Windows, the available signals are mapped to exit codes. The default\n"
                 "signal is 1 (HUP), but the most commonly used signal is 9 (KILL).\n\n"
                 "Note: On Windows, you may need administrator privileges to terminate\n"
                 "certain processes.",

                 /* examples */
                 "  kill 1234\n"
                 "  kill -l\n"
                 "  kill -9 5678",

                 /* see_also */
                 "ps(1)",

                 /* author */
                 "caomengxuan666",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 KILL_OPTIONS
) {
  using namespace kill_pipeline;

  auto result = process_kill(ctx);
  if (!result) {
    cp::report_error(result, L"kill");
    return 1;
  }

  return *result ? 0 : 1;
}
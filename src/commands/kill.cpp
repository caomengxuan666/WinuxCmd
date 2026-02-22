/*
<<<<<<< HEAD
 *  Copyright ? 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
<<<<<<< HEAD
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
=======
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
 *
 *  - File: kill.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
<<<<<<< HEAD

/// @contributors:
///   - @contributor1 arookieofc 2128194521@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implementation for kill.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright ? 2026 WinuxCmd
=======
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
/// @Description: Implementation for kill - terminate a process
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

#include "pch/pch.h"
//include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
<<<<<<< HEAD
=======
import container;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;
using namespace std::string_view_literals;
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

/**
 * @brief KILL command options definition
 *
 * This array defines all the options supported by the kill command.
 * Each option is described with its short form, long form, and description.
<<<<<<< HEAD
 *
 * @par Options:
 *
 * - @a -s, @a --signal: Specify the signal to send [IMPLEMENTED]
 * - @a -l, @a --list: List signal names [IMPLEMENTED]
 * - @a -L, @a --table: List signal names in a table [IMPLEMENTED]
 * - @a -9: Send SIGKILL (force kill) [IMPLEMENTED]
 * - @a -15: Send SIGTERM (graceful termination) [IMPLEMENTED]
 */

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

// ======================================================
// Options (constexpr)
// ======================================================

auto constexpr KILL_OPTIONS = std::array{
    OPTION("-s", "--signal", "specify the signal to send", STRING_TYPE),
    OPTION("-l", "--list", "list signal names"),
    OPTION("-L", "--table", "list signal names in a table"),
    OPTION("-9", "", "send SIGKILL (force kill)"),
    OPTION("-15", "", "send SIGTERM (graceful termination)")};

// ======================================================
// Signal definitions (Unix-like signals mapped to Windows)
// ======================================================
namespace kill_constants {
struct SignalInfo {
  int number;
  std::string_view name;
  std::string_view description;
};

// Common Unix signals
constexpr std::array<SignalInfo, 15> SIGNALS = {{
    {1, "HUP", "Hangup"},
    {2, "INT", "Interrupt"},
    {3, "QUIT", "Quit"},
    {6, "ABRT", "Abort"},
    {9, "KILL", "Kill (cannot be caught or ignored)"},
    {11, "SEGV", "Segmentation fault"},
    {13, "PIPE", "Broken pipe"},
    {14, "ALRM", "Alarm clock"},
    {15, "TERM", "Termination"},
    {17, "STOP", "Stop (cannot be caught or ignored)"},
    {18, "TSTP", "Terminal stop"},
    {19, "CONT", "Continue"},
    {20, "CHLD", "Child status changed"},
    {21, "TTIN", "Background read from tty"},
    {22, "TTOU", "Background write to tty"},
}};

// Get signal number by name
auto get_signal_by_name(const std::string& name) -> std::optional<int> {
  std::string upper_name = name;
  std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(),
                 ::toupper);

  // Remove SIG prefix if present
  if (upper_name.starts_with("SIG")) {
    upper_name = upper_name.substr(3);
  }

  for (const auto& sig : SIGNALS) {
    if (sig.name == upper_name) {
      return sig.number;
    }
  }
  return std::nullopt;
}

// Get signal info by number
auto get_signal_info(int signal_number) -> std::optional<SignalInfo> {
  for (const auto& sig : SIGNALS) {
    if (sig.number == signal_number) {
      return sig;
    }
  }
  return std::nullopt;
}

=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
}  // namespace kill_constants

// ======================================================
// Pipeline components
// ======================================================
namespace kill_pipeline {
namespace cp = core::pipeline;

<<<<<<< HEAD
// ----------------------------------------------
// 1. List signals
// ----------------------------------------------
auto list_signals(bool table_format) -> cp::Result<bool> {
  if (table_format) {
    safePrint("Signal  Name    Description\n");
    safePrint("------  ------  -----------\n");
    for (const auto& sig : kill_constants::SIGNALS) {
      char buffer[256];
      snprintf(buffer, sizeof(buffer), "%-7d %-7.*s %.*s\n", sig.number,
               static_cast<int>(sig.name.size()), sig.name.data(),
               static_cast<int>(sig.description.size()), sig.description.data());
      safePrint(buffer);
    }
  } else {
    for (const auto& sig : kill_constants::SIGNALS) {
      safePrint(sig.name);
      safePrint(" ");
    }
    safePrint("\n");
  }
  return true;
}

// ----------------------------------------------
// 2. Parse signal argument
// ----------------------------------------------
auto parse_signal(const std::string& signal_arg) -> cp::Result<int> {
  // Try to parse as number first
  try {
    int signal_num = std::stoi(signal_arg);
    if (signal_num < 0 || signal_num > 64) {
      return std::unexpected("invalid signal number");
    }
    return signal_num;
  } catch (...) {
    // Try to parse as signal name
    auto signal_num = kill_constants::get_signal_by_name(signal_arg);
    if (!signal_num) {
      return std::unexpected("unknown signal: " + signal_arg);
    }
    return *signal_num;
  }
}

// ----------------------------------------------
// 3. Terminate process
// ----------------------------------------------
auto terminate_process(DWORD pid, int signal, bool verbose)
    -> cp::Result<bool> {
  // Open process with necessary access rights
  DWORD access_rights = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION;
  HANDLE hProcess = OpenProcess(access_rights, FALSE, pid);

  if (hProcess == NULL) {
    DWORD error = GetLastError();
    if (error == ERROR_ACCESS_DENIED) {
      return std::unexpected("permission denied");
    } else if (error == ERROR_INVALID_PARAMETER) {
      return std::unexpected("no such process");
    } else {
      return std::unexpected("cannot open process");
    }
  }

  // Check if process is still running
  DWORD exitCode;
  if (GetExitCodeProcess(hProcess, &exitCode)) {
    if (exitCode != STILL_ACTIVE) {
      CloseHandle(hProcess);
      return std::unexpected("process already terminated");
    }
  }

  bool success = false;
  
  // Handle different signals
  if (signal == 0) {
    // Signal 0: just check if process exists
    success = true;
  } else if (signal == 9) {
    // SIGKILL: force termination
    success = TerminateProcess(hProcess, 1) != 0;
  } else if (signal == 15 || signal == 2) {
    // SIGTERM or SIGINT: graceful termination
    // Try to find and close main window first
    struct EnumData {
      DWORD targetPid;
      HWND foundHwnd;
    };
    
    EnumData enumData = {pid, NULL};
    
    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
          EnumData* pData = reinterpret_cast<EnumData*>(lParam);
          DWORD windowPid;
          GetWindowThreadProcessId(hwnd, &windowPid);
          if (windowPid == pData->targetPid && IsWindowVisible(hwnd)) {
            pData->foundHwnd = hwnd;
            return FALSE;  // Stop enumeration
          }
          return TRUE;  // Continue enumeration
        },
        reinterpret_cast<LPARAM>(&enumData));

    if (enumData.foundHwnd != NULL) {
      // Send WM_CLOSE message for graceful shutdown
      PostMessage(enumData.foundHwnd, WM_CLOSE, 0, 0);
      
      // Wait up to 5 seconds for process to exit
      DWORD wait_result = WaitForSingleObject(hProcess, 5000);
      if (wait_result == WAIT_OBJECT_0) {
        success = true;
      } else {
        // If still running, force terminate
        success = TerminateProcess(hProcess, 1) != 0;
      }
    } else {
      // No window found, force terminate
      success = TerminateProcess(hProcess, 1) != 0;
    }
  } else {
    // Other signals: just terminate
    success = TerminateProcess(hProcess, 1) != 0;
  }

  CloseHandle(hProcess);

  if (!success) {
    return std::unexpected("failed to terminate process");
  }

  if (verbose) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Terminated process %lu\n", pid);
    safePrint(buffer);
  }

  return true;
}

// ----------------------------------------------
// 4. Parse PID list
// ----------------------------------------------
auto parse_pids(const std::vector<std::string>& pid_args)
    -> cp::Result<std::vector<DWORD>> {
  std::vector<DWORD> pids;
  
  for (const auto& pid_str : pid_args) {
    try {
      long long pid_value = std::stoll(pid_str);
      if (pid_value <= 0 || pid_value > UINT32_MAX) {
        return std::unexpected("invalid PID: " + pid_str);
      }
      pids.push_back(static_cast<DWORD>(pid_value));
    } catch (...) {
      return std::unexpected("invalid PID: " + pid_str);
    }
  }

  if (pids.empty()) {
    return std::unexpected("no process ID specified");
  }

  return pids;
}

// ----------------------------------------------
// 5. Process command
// ----------------------------------------------
template <size_t N>
auto process_command(const CommandContext<N>& ctx) -> cp::Result<bool> {
  bool list = ctx.get<bool>("--list", false);
  list |= ctx.get<bool>("-l", false);
  bool table = ctx.get<bool>("--table", false);
  table |= ctx.get<bool>("-L", false);

  // Handle list/table options
  if (list || table) {
    return list_signals(table);
  }

  // Parse signal
  int signal = 15;  // Default: SIGTERM
  
  if (ctx.get<bool>("-9", false)) {
    signal = 9;
  } else if (ctx.get<bool>("-15", false)) {
    signal = 15;
  } else {
    std::string signal_str = ctx.get<std::string>("--signal", "");
    if (signal_str.empty()) {
      signal_str = ctx.get<std::string>("-s", "");
    }
    
    if (!signal_str.empty()) {
      auto signal_result = parse_signal(signal_str);
      if (!signal_result) {
        return std::unexpected(signal_result.error());
      }
      signal = *signal_result;
    }
  }

  // Parse PIDs from positional arguments
  std::vector<std::string> pid_args;
  for (auto arg : ctx.positionals) {
    pid_args.push_back(std::string(arg));
  }

  auto pids_result = parse_pids(pid_args);
  if (!pids_result) {
    return std::unexpected(pids_result.error());
  }

  auto pids = *pids_result;
  bool all_success = true;

  // Terminate each process
  for (DWORD pid : pids) {
    auto result = terminate_process(pid, signal, false);
    if (!result) {
      safeErrorPrint("kill: (");
      safeErrorPrint(std::to_string(pid));
      safeErrorPrint(") - ");
      safeErrorPrint(result.error());
      safeErrorPrint("\n");
      all_success = false;
    }
  }

  return all_success;
=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
}

}  // namespace kill_pipeline

<<<<<<< HEAD
// ======================================================
// Command registration
// ======================================================

REGISTER_COMMAND(
    kill,
    /* name */
    "kill",

    /* synopsis */
    "send a signal to a process",

    /* description */
    "Send signals to processes, or list signals.\n"
    "\n"
    "The default signal for kill is TERM. Use -l or -L to list available "
    "signals.\n"
    "Particularly useful signals include HUP, INT, KILL, STOP, CONT, and 0.\n"
    "Alternate signals may be specified in three ways: -9, -SIGKILL or "
    "-KILL.\n"
    "\n"
    "Note: On Windows, most signals are mapped to process termination,\n"
    "except SIGTERM which attempts graceful shutdown first.",

    /* examples */
    "  kill 1234                Kill process 1234 with SIGTERM\n"
    "  kill -9 1234             Force kill process 1234\n"
    "  kill -s KILL 1234        Same as kill -9 1234\n"
    "  kill -l                  List all signal names\n"
    "  kill -L                  List signals in a table format",

    /* see also */
    "ps(1), pkill(1), killall(1)",

    /* author */
    "WinuxCmd",

    /* copyright */
    "Copyright ? 2026 WinuxCmd",

    /* options */
    KILL_OPTIONS) {
  using namespace kill_pipeline;
  using namespace core::pipeline;

  auto result = process_command(ctx);
  if (!result) {
    report_error(result, L"kill");
=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
    return 1;
  }

  return *result ? 0 : 1;
<<<<<<< HEAD
}
=======
}
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

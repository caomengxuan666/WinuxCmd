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
 *  - File: renice.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for renice.
/// @Version: 0.1.0
/// @License: MIT

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr RENICE_OPTIONS =
    std::array{OPTION("-n", "--priority", "set niceness value", STRING_TYPE),
               OPTION("-p", "--pid", "interpret operands as process IDs")};

namespace renice_pipeline {

auto parse_int(std::string_view text) -> std::optional<int> {
  int value = 0;
  auto [ptr, ec] =
      std::from_chars(text.data(), text.data() + text.size(), value);
  if (ec != std::errc() || ptr != text.data() + text.size())
    return std::nullopt;
  return value;
}

auto run(const CommandContext<RENICE_OPTIONS.size()>& ctx) -> int {
  std::string priority_text = ctx.get<std::string>("-n", "");
  if (priority_text.empty()) {
    priority_text = ctx.get<std::string>("--priority", "");
  }

  std::span<const std::string_view> operands(ctx.positionals.data(),
                                             ctx.positionals.size());
  if (priority_text.empty() && !operands.empty()) {
    priority_text = std::string(operands.front());
    operands = operands.subspan(1);
  }

  auto niceness = parse_int(priority_text);
  if (!niceness) {
    safeErrorPrintLn("renice: invalid priority '" + priority_text + "'");
    return 1;
  }
  *niceness = std::clamp(*niceness, -20, 19);

  if (operands.empty()) {
    safeErrorPrintLn("renice: missing process ID");
    return 1;
  }

  DWORD priority_class = win32_priority_class_for_niceness(*niceness);
  int status = 0;

  for (auto operand : operands) {
    auto pid = win32_parse_pid(operand);
    if (!pid) {
      safeErrorPrintLn("renice: invalid process ID '" + std::string(operand) +
                       "'");
      status = 1;
      continue;
    }

    UniqueHandle process(
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_INFORMATION,
                    FALSE, *pid));
    if (!process) {
      DWORD error = GetLastError();
      safeErrorPrintLn("renice: failed to get priority for " +
                       std::to_string(*pid) + ": " +
                       win32_posix_error_text(error));
      status = 1;
      continue;
    }

    int old_nice =
        win32_niceness_from_priority_class(GetPriorityClass(process.get()));
    if (!SetPriorityClass(process.get(), priority_class)) {
      DWORD error = GetLastError();
      safeErrorPrintLn("renice: failed to set priority for " +
                       std::to_string(*pid) + ": " +
                       win32_posix_error_text(error));
      status = 1;
      continue;
    }

    safePrintLn(std::to_string(*pid) + ": old priority " +
                std::to_string(old_nice) + ", new priority " +
                std::to_string(*niceness));
  }

  return status;
}

}  // namespace renice_pipeline

REGISTER_COMMAND(renice, "renice", "renice [-n] PRIORITY [-p] PID...",
                 "Alter the scheduling priority of running processes.\n"
                 "Windows priority classes are mapped to Unix-like niceness "
                 "values from -20 to 19.",
                 "  renice 10 -p 1234\n"
                 "  renice -n 0 1234",
                 "nice(1), ps(1), pgrep(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", RENICE_OPTIONS) {
  return renice_pipeline::run(ctx);
}

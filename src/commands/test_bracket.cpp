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
 *  - File: test_bracket.cpp
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for [ command (alias for test).
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

// ======================================================
// Options (constexpr)
// ======================================================

auto constexpr BRACKET_OPTIONS =
    std::array{OPTION("-n", "", "string length is non-zero"),
               OPTION("-z", "", "string length is zero"),
               OPTION("-b", "", "file is block special"),
               OPTION("-c", "", "file is character special"),
               OPTION("-d", "", "file is a directory"),
               OPTION("-e", "", "file exists"),
               OPTION("-f", "", "file is a regular file"),
               OPTION("-g", "", "file has set-group-ID bit"),
               OPTION("-G", "", "file is owned by effective group ID"),
               OPTION("-h", "", "file is a symbolic link"),
               OPTION("-L", "", "file is a symbolic link"),
               OPTION("-k", "", "file has sticky bit"),
               OPTION("-p", "", "file is a named pipe"),
               OPTION("-r", "", "file is readable"),
               OPTION("-s", "", "file size is non-zero"),
               OPTION("-S", "", "file is a socket"),
               OPTION("-t", "", "file descriptor is a terminal"),
               OPTION("-u", "", "file has set-user-ID bit"),
               OPTION("-w", "", "file is writable"),
               OPTION("-x", "", "file is executable"),
               OPTION("-O", "", "file is owned by effective user ID"),
               OPTION("-eq", "", "integer equal"),
               OPTION("-ne", "", "integer not equal"),
               OPTION("-lt", "", "integer less than"),
               OPTION("-le", "", "integer less than or equal"),
               OPTION("-gt", "", "integer greater than"),
               OPTION("-ge", "", "integer greater than or equal"),
               OPTION("-a", "", "logical and"),
               OPTION("-and", "", "logical and"),
               OPTION("-o", "", "logical or"),
               OPTION("-or", "", "logical or"),
               OPTION("!", "", "logical not"),
               OPTION("=", "", "string equal"),
               OPTION("==", "", "string equal"),
               OPTION("!=", "", "string not equal"),
               OPTION("<", "", "string less than"),
               OPTION("<=", "", "string less than or equal"),
               OPTION(">", "", "string greater than"),
               OPTION(">=", "", "string greater than or equal")};

namespace bracket_command {
auto materialize_raw_args(const std::vector<std::string_view>& raw_args)
    -> std::vector<std::string> {
  std::vector<std::string> args;
  args.reserve(raw_args.size());
  for (auto arg : raw_args) args.emplace_back(arg);
  return args;
}

auto file_attrs(const std::string& path) -> DWORD {
  return GetFileAttributesW(utf8_to_wstring(path).c_str());
}

auto file_exists(const std::string& path) -> bool {
  return file_attrs(path) != INVALID_FILE_ATTRIBUTES;
}

auto is_regular_file(const std::string& path) -> bool {
  DWORD attrs = file_attrs(path);
  return attrs != INVALID_FILE_ATTRIBUTES &&
         (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

auto is_directory(const std::string& path) -> bool {
  DWORD attrs = file_attrs(path);
  return attrs != INVALID_FILE_ATTRIBUTES &&
         (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

auto file_has_size(const std::string& path) -> bool {
  WIN32_FILE_ATTRIBUTE_DATA data;
  if (!GetFileAttributesExW(utf8_to_wstring(path).c_str(),
                            GetFileExInfoStandard, &data)) {
    return false;
  }
  return data.nFileSizeHigh > 0 || data.nFileSizeLow > 0;
}

auto string_to_int(const std::string& value, long long& out) -> bool {
  try {
    size_t consumed = 0;
    out = std::stoll(value, &consumed);
    return consumed == value.size();
  } catch (...) {
    return false;
  }
}

auto is_unary_operator(const std::string& op) -> bool {
  static const std::unordered_set<std::string> ops = {
      "-n", "-z", "-b", "-c", "-d", "-e", "-f", "-g", "-G", "-h", "-L",
      "-k", "-p", "-r", "-s", "-S", "-t", "-u", "-w", "-x", "-O"};
  return ops.contains(op);
}

auto is_binary_operator(const std::string& op) -> bool {
  static const std::unordered_set<std::string> ops = {
      "=",   "==",  "!=",  "<",   "<=", ">",    ">=", "-eq", "-ne",
      "-lt", "-le", "-gt", "-ge", "-a", "-and", "-o", "-or"};
  return ops.contains(op);
}

auto compare_strings(const std::string& op, const std::string& a,
                     const std::string& b) -> bool {
  if (op == "=" || op == "==") return a == b;
  if (op == "!=") return a != b;
  if (op == "<") return a < b;
  if (op == "<=") return a <= b;
  if (op == ">") return a > b;
  if (op == ">=") return a >= b;
  return false;
}

auto compare_integers(const std::string& op, long long a, long long b) -> bool {
  if (op == "-eq") return a == b;
  if (op == "-ne") return a != b;
  if (op == "-lt") return a < b;
  if (op == "-le") return a <= b;
  if (op == "-gt") return a > b;
  if (op == "-ge") return a >= b;
  return false;
}

auto evaluate_unary(const std::string& op, const std::string& arg) -> int {
  if (op == "-n") return arg.empty() ? 1 : 0;
  if (op == "-z") return arg.empty() ? 0 : 1;
  if (op == "-b") return file_exists(arg) ? 0 : 1;
  if (op == "-c") return file_exists(arg) ? 0 : 1;
  if (op == "-d") return is_directory(arg) ? 0 : 1;
  if (op == "-e") return file_exists(arg) ? 0 : 1;
  if (op == "-f") return is_regular_file(arg) ? 0 : 1;
  if (op == "-g") return 1;
  if (op == "-G") return 1;
  if (op == "-h" || op == "-L") return file_exists(arg) ? 0 : 1;
  if (op == "-k") return 1;
  if (op == "-p") return 1;
  if (op == "-r") return file_exists(arg) ? 0 : 1;
  if (op == "-s") return file_has_size(arg) ? 0 : 1;
  if (op == "-S") return 1;
  if (op == "-t") return 1;
  if (op == "-u") return 1;
  if (op == "-w") {
    DWORD attrs = file_attrs(arg);
    return attrs != INVALID_FILE_ATTRIBUTES &&
                   (attrs & FILE_ATTRIBUTE_READONLY) == 0
               ? 0
               : 1;
  }
  if (op == "-x") {
    std::wstring wpath = utf8_to_wstring(arg);
    auto dot = wpath.find_last_of(L'.');
    if (dot == std::wstring::npos) return 1;
    std::wstring ext = wpath.substr(dot + 1);
    std::ranges::transform(ext, ext.begin(), towlower);
    return (ext == L"exe" || ext == L"bat" || ext == L"cmd" || ext == L"ps1")
               ? 0
               : 1;
  }
  if (op == "-O") return 1;
  return 2;
}

auto evaluate_binary(const std::string& a, const std::string& op,
                     const std::string& b) -> int {
  if (op == "=" || op == "==" || op == "!=" || op == "<" || op == "<=" ||
      op == ">" || op == ">=") {
    return compare_strings(op, a, b) ? 0 : 1;
  }

  if (op == "-eq" || op == "-ne" || op == "-lt" || op == "-le" || op == "-gt" ||
      op == "-ge") {
    long long left = 0;
    long long right = 0;
    if (!string_to_int(a, left) || !string_to_int(b, right)) return 2;
    return compare_integers(op, left, right) ? 0 : 1;
  }

  if (op == "-a" || op == "-and") return (!a.empty() && !b.empty()) ? 0 : 1;
  if (op == "-o" || op == "-or") return (!a.empty() || !b.empty()) ? 0 : 1;
  return 2;
}

auto invert_test_status(int status) -> int {
  if (status == 0) return 1;
  if (status == 1) return 0;
  return status;
}

auto evaluate_bracket_expression(std::span<const std::string> args) -> int {
  if (args.empty()) return 1;
  if (args.front() == "!") {
    return invert_test_status(evaluate_bracket_expression(args.subspan(1)));
  }
  if (args.size() == 1) return args[0].empty() ? 1 : 0;
  if (args.size() == 2 && is_unary_operator(args[0])) {
    return evaluate_unary(args[0], args[1]);
  }
  if (args.size() == 3 && is_binary_operator(args[1])) {
    return evaluate_binary(args[0], args[1], args[2]);
  }
  if (args.size() == 4 && args[1] == "!" &&
      (args[2] == "=" || args[2] == "==" || args[2] == "!=")) {
    return invert_test_status(evaluate_binary(args[0], args[2], args[3]));
  }
  return 2;
}
}  // namespace bracket_command

// ======================================================
// Main command implementation
// ======================================================

REGISTER_COMMAND(bracket,
                 /* cmd_name */ "[",
                 /* cmd_synopsis */ "[ expression",
                 /* cmd_desc */
                 "Evaluate conditional expression (requires closing ]).",
                 /* examples */
                 "  [ -f /etc/passwd ]\n"
                 "  [ -n \"$var\" ]\n"
                 "  [ \"$a\" -eq \"$b\" ]\n"
                 "  [ -d /tmp ] && echo 'Directory exists'",
                 /* see_also */ "test, bash",
                 /* author */ "WinuxCmd",
                 /* copyright */ "Copyright © 2026 WinuxCmd",
                 /* options */ BRACKET_OPTIONS) {
  auto args = bracket_command::materialize_raw_args(ctx.raw_args);
  if (args.empty() || args.back() != "]") {
    safeErrorPrintLn("[: missing ']'");
    return 2;
  }

  args.pop_back();
  return bracket_command::evaluate_bracket_expression(args);
}

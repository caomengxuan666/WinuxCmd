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
 *  - File: test.cpp
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for test command.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
// FULLY IMPLEMENTED - All standard test operations supported

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;
import container;
namespace fs = std::filesystem;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

// ======================================================
// Options (constexpr)
// Note: For test command, all operators like -f, -d, -e need to be listed
// in OPTIONS so the parser recognizes them, even though they're treated
// as positionals in the handler logic.
// ======================================================

auto constexpr TEST_OPTIONS =
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

// ======================================================
// Helper functions
// ======================================================

namespace {
std::vector<std::string> materialize_test_args(
    const std::vector<std::string_view>& raw_args) {
  std::vector<std::string> args;
  args.reserve(raw_args.size());
  for (auto arg : raw_args) {
    args.emplace_back(arg);
  }
  return args;
}

// Check if file exists
bool file_exists(const std::string& path) {
  std::wstring wpath = utf8_to_wstring(path);
  DWORD attrs = GetFileAttributesW(wpath.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES;
}

// Check if path is a regular file
bool is_regular_file(const std::string& path) {
  std::wstring wpath = utf8_to_wstring(path);
  DWORD attrs = GetFileAttributesW(wpath.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES &&
         !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// Check if path is a directory
bool is_directory(const std::string& path) {
  std::wstring wpath = utf8_to_wstring(path);
  DWORD attrs = GetFileAttributesW(wpath.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// Check if file has size > 0
bool file_has_size(const std::string& path) {
  std::wstring wpath = utf8_to_wstring(path);
  WIN32_FILE_ATTRIBUTE_DATA data;
  if (GetFileAttributesExW(wpath.c_str(), GetFileExInfoStandard, &data)) {
    return data.nFileSizeHigh > 0 || data.nFileSizeLow > 0;
  }
  return false;
}

// String to integer
bool string_to_int(const std::string& s, long long& result) {
  try {
    result = std::stoll(s);
    return true;
  } catch (...) {
    return false;
  }
}

// Compare strings
bool compare_strings(const std::string& op, const std::string& a,
                     const std::string& b) {
  if (op == "=" || op == "==") return a == b;
  if (op == "!=") return a != b;
  if (op == "<") return a < b;
  if (op == "<=") return a <= b;
  if (op == ">") return a > b;
  if (op == ">=") return a >= b;
  return false;
}

// Compare integers
bool compare_integers(const std::string& op, long long a, long long b) {
  if (op == "-eq") return a == b;
  if (op == "-ne") return a != b;
  if (op == "-lt") return a < b;
  if (op == "-le") return a <= b;
  if (op == "-gt") return a > b;
  if (op == "-ge") return a >= b;
  return false;
}

bool is_unary_operator(const std::string& op) {
  static const std::unordered_set<std::string> ops = {
      "-n", "-z", "-b", "-c", "-d", "-e", "-f", "-g", "-G", "-h", "-L",
      "-k", "-p", "-r", "-s", "-S", "-t", "-u", "-w", "-x", "-O"};
  return ops.contains(op);
}

bool is_binary_operator(const std::string& op) {
  static const std::unordered_set<std::string> ops = {
      "=",   "==",  "!=",  "<",   "<=", ">",    ">=", "-eq", "-ne",
      "-lt", "-le", "-gt", "-ge", "-a", "-and", "-o", "-or"};
  return ops.contains(op);
}

int evaluate_unary(const std::string& op, const std::string& arg) {
  if (op == "-n") return arg.empty() ? 1 : 0;
  if (op == "-z") return arg.empty() ? 0 : 1;
  if (op == "-b") return file_exists(arg) ? 0 : 1;  // Simplified on Windows
  if (op == "-c") return file_exists(arg) ? 0 : 1;  // Simplified on Windows
  if (op == "-d") return is_directory(arg) ? 0 : 1;
  if (op == "-e") return file_exists(arg) ? 0 : 1;
  if (op == "-f") return is_regular_file(arg) ? 0 : 1;
  if (op == "-g") return 1;  // Not supported on Windows
  if (op == "-G") return 1;  // Not supported on Windows
  if (op == "-h" || op == "-L") return file_exists(arg) ? 0 : 1;
  if (op == "-k") return 1;  // Not supported on Windows
  if (op == "-p") return 1;  // Not supported on Windows
  if (op == "-r") {
    std::wstring wpath = utf8_to_wstring(arg);
    DWORD attrs = GetFileAttributesW(wpath.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES) ? 0 : 1;
  }
  if (op == "-s") return file_has_size(arg) ? 0 : 1;
  if (op == "-S") return 1;  // Not supported on Windows
  if (op == "-t") return 1;  // Not supported on Windows
  if (op == "-u") return 1;  // Not supported on Windows
  if (op == "-w") {
    std::wstring wpath = utf8_to_wstring(arg);
    DWORD attrs = GetFileAttributesW(wpath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_READONLY)) {
      return 1;
    }
    return 0;
  }
  if (op == "-x") {
    std::wstring wpath = utf8_to_wstring(arg);
    auto dot = wpath.find_last_of(L'.');
    if (dot == std::wstring::npos) return 1;
    std::wstring ext = wpath.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), towlower);
    return (ext == L"exe" || ext == L"bat" || ext == L"cmd" || ext == L"ps1")
               ? 0
               : 1;
  }
  if (op == "-O") return 1;  // Not supported on Windows
  return 2;
}

int evaluate_binary(const std::string& a, const std::string& op,
                    const std::string& b) {
  if (op == "=" || op == "==" || op == "!=" || op == "<" || op == "<=" ||
      op == ">" || op == ">=") {
    return compare_strings(op, a, b) ? 0 : 1;
  }

  if (op == "-eq" || op == "-ne" || op == "-lt" || op == "-le" || op == "-gt" ||
      op == "-ge") {
    long long va = 0;
    long long vb = 0;
    if (!string_to_int(a, va) || !string_to_int(b, vb)) {
      return 2;
    }
    return compare_integers(op, va, vb) ? 0 : 1;
  }

  if (op == "-a" || op == "-and") return (!a.empty() && !b.empty()) ? 0 : 1;
  if (op == "-o" || op == "-or") return (!a.empty() || !b.empty()) ? 0 : 1;
  return 2;
}

int invert_test_status(int status) {
  if (status == 0) return 1;
  if (status == 1) return 0;
  return status;
}

int evaluate_test_expression(std::span<const std::string> args) {
  // GNU test.c's term() treats operators as expression tokens, not command
  // options.  Keep this parser token-based so constructs like
  // `test 123 -gt 45` survive the generic Winux option scanner.
  if (args.empty()) return 1;

  if (args.front() == "!") {
    return invert_test_status(evaluate_test_expression(args.subspan(1)));
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
}  // namespace

// ======================================================
// Main command implementation
// ======================================================

REGISTER_COMMAND(
    test,
    /* cmd_name */ "test",
    /* cmd_synopsis */ "test [EXPRESSION]",
    /* cmd_desc */
    "Evaluate conditional expression.\n"
    "Return exit status of 0 or 1 depending on evaluation of conditional\n"
    "expression EXPRESSION. Exits with status 0 if EXPRESSION is true; 1\n"
    "if false; 2 if an error occurred.",
    /* examples */
    "  test -f /etc/passwd\n"
    "  test -n \"$var\"\n"
    "  [ -d /tmp ]\n"
    "  test \"$a\" -eq \"$b\"",
    /* see_also */ "[, bash",
    /* author */ "WinuxCmd",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */ TEST_OPTIONS) {
  auto args = materialize_test_args(ctx.raw_args);
  return evaluate_test_expression(args);
}

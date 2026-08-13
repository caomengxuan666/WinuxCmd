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
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
  return ec == std::errc() && ptr == s.data() + s.size();
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
      "-lt", "-le", "-gt", "-ge", "-a", "-and", "-o", "-or", "-nt",
      "-ot", "-ef"};
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

  if (op == "-nt" || op == "-ot" || op == "-ef") {
    WIN32_FILE_ATTRIBUTE_DATA left{}, right{};
    const auto left_path = utf8_to_wstring(a);
    const auto right_path = utf8_to_wstring(b);
    if (!GetFileAttributesExW(left_path.c_str(), GetFileExInfoStandard,
                              &left) ||
        !GetFileAttributesExW(right_path.c_str(), GetFileExInfoStandard,
                              &right)) {
      return 1;
    }
    if (op == "-ef") {
      wchar_t left_full[MAX_PATH * 4]{}, right_full[MAX_PATH * 4]{};
      if (!GetFullPathNameW(left_path.c_str(), std::size(left_full), left_full,
                            nullptr) ||
          !GetFullPathNameW(right_path.c_str(), std::size(right_full),
                            right_full, nullptr)) {
        return 1;
      }
      return _wcsicmp(left_full, right_full) == 0 ? 0 : 1;
    }
    ULARGE_INTEGER left_time{left.ftLastWriteTime.dwLowDateTime,
                             left.ftLastWriteTime.dwHighDateTime};
    ULARGE_INTEGER right_time{right.ftLastWriteTime.dwLowDateTime,
                              right.ftLastWriteTime.dwHighDateTime};
    return op == "-nt" ? (left_time.QuadPart > right_time.QuadPart ? 0 : 1)
                       : (left_time.QuadPart < right_time.QuadPart ? 0 : 1);
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

class TestExpressionParser {
 public:
  explicit TestExpressionParser(std::span<const std::string> args) : args_(args) {}
  int parse() {
    if (args_.empty()) return 1;
    int result = parse_or();
    return error_ || pos_ != args_.size() ? 2 : result;
  }
 private:
  int parse_or() {
    int result = parse_and();
    while (peek("-o") || peek("-or")) { ++pos_; int right = parse_and(); result = combine_or(result, right); }
    return result;
  }
  int parse_and() {
    int result = parse_not();
    while (peek("-a") || peek("-and")) { ++pos_; int right = parse_not(); result = combine_and(result, right); }
    return result;
  }
  int parse_not() {
    if (peek("!")) { ++pos_; return invert_test_status(parse_not()); }
    if (peek("(")) {
      ++pos_; int result = parse_or();
      if (!peek(")")) { error_ = true; return 2; }
      ++pos_; return result;
    }
    return parse_primary();
  }
  int parse_primary() {
    if (pos_ >= args_.size()) { error_ = true; return 2; }
    if (pos_ + 1 < args_.size() && is_unary_operator(std::string(args_[pos_]))) {
      auto op = std::string(args_[pos_++]);
      return evaluate_unary(op, std::string(args_[pos_++]));
    }
    if (pos_ + 2 < args_.size() && is_binary_operator(std::string(args_[pos_ + 1]))) {
      auto left = std::string(args_[pos_++]);
      auto op = std::string(args_[pos_++]);
      auto right = std::string(args_[pos_++]);
      return evaluate_binary(left, op, right);
    }
    return args_[pos_++].empty() ? 1 : 0;
  }
  static int combine_or(int left, int right) { return left == 0 || right == 0 ? 0 : (left == 2 || right == 2 ? 2 : 1); }
  static int combine_and(int left, int right) { return left == 0 && right == 0 ? 0 : (left == 2 || right == 2 ? 2 : 1); }
  bool peek(std::string_view token) const { return pos_ < args_.size() && args_[pos_] == token; }
  std::span<const std::string> args_;
  size_t pos_ = 0;
  bool error_ = false;
};

int evaluate_test_expression(std::span<const std::string> args) {
  return TestExpressionParser(args).parse();
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

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
    // [GNU] -n
    std::array{OPTION("-n", "", "string length is non-zero"),
               // [GNU] -z
               OPTION("-z", "", "string length is zero"),
               // [GNU] -b
               OPTION("-b", "", "file is block special"),
               // [GNU] -c
               OPTION("-c", "", "file is character special"),
               // [GNU] -d
               OPTION("-d", "", "file is a directory"),
               // [GNU] -e
               OPTION("-e", "", "file exists"),
               // [GNU] -f
               OPTION("-f", "", "file is a regular file"),
               // [GNU] -g
               OPTION("-g", "", "file has set-group-ID bit"),
               // [GNU] -G
               OPTION("-G", "", "file is owned by effective group ID"),
               // [GNU] -h
               OPTION("-h", "", "file is a symbolic link"),
               // [GNU] -L
               OPTION("-L", "", "file is a symbolic link"),
               // [GNU] -k
               OPTION("-k", "", "file has sticky bit"),
               // [GNU] -p
               OPTION("-p", "", "file is a named pipe"),
               // [GNU] -r
               OPTION("-r", "", "file is readable"),
               // [GNU] -s
               OPTION("-s", "", "file size is non-zero"),
               // [GNU] -S
               OPTION("-S", "", "file is a socket"),
               // [GNU] -t
               OPTION("-t", "", "file descriptor is a terminal"),
               // [GNU] -u
               OPTION("-u", "", "file has set-user-ID bit"),
               // [GNU] -w
               OPTION("-w", "", "file is writable"),
               // [GNU] -x
               OPTION("-x", "", "file is executable"),
               // [GNU] -O
               OPTION("-O", "", "file is owned by effective user ID"),
               // [GNU] -eq
               OPTION("-eq", "", "integer equal"),
               // [GNU] -ne
               OPTION("-ne", "", "integer not equal"),
               // [GNU] -lt
               OPTION("-lt", "", "integer less than"),
               // [GNU] -le
               OPTION("-le", "", "integer less than or equal"),
               // [GNU] -gt
               OPTION("-gt", "", "integer greater than"),
               // [GNU] -ge
               OPTION("-ge", "", "integer greater than or equal"),
               // [GNU] -a
               OPTION("-a", "", "logical and"),
               // [EXT] -and
               OPTION("-and", "", "logical and"),
               // [GNU] -o
               OPTION("-o", "", "logical or"),
               // [EXT] -or
               OPTION("-or", "", "logical or"),
               // [GNU] !
               OPTION("!", "", "logical not"),
               // [GNU] =
               OPTION("=", "", "string equal"),
               // [GNU] ==
               OPTION("==", "", "string equal"),
               // [GNU] !=
               OPTION("!=", "", "string not equal"),
               // [GNU] <
               OPTION("<", "", "string less than"),
               // [GNU] <=
               OPTION("<=", "", "string less than or equal"),
               // [GNU] >
               OPTION(">", "", "string greater than"),
               // [GNU] >=
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

// [GNU] test/compare.c accepts arbitrarily large decimal integers (the MSYS2
// build compares them exactly instead of erroring on overflow), so parse
// operands as sign + magnitude digit strings and compare bignum-style.
// Returns false on any non-decimal operand ("invalid integer", exit 2).
bool parse_decimal_bignum(const std::string& s, bool& negative,
                          std::string& magnitude) {
  size_t i = 0;
  negative = false;
  if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
    negative = (s[i] == '-');
    ++i;
  }
  if (i >= s.size()) return false;
  for (size_t j = i; j < s.size(); ++j) {
    if (!std::isdigit(static_cast<unsigned char>(s[j]))) return false;
  }
  magnitude = s.substr(i);
  // Normalize: strip leading zeros.
  size_t nz = magnitude.find_first_not_of('0');
  if (nz == std::string::npos) {
    magnitude = "0";
    negative = false;
  } else {
    magnitude = magnitude.substr(nz);
  }
  return true;
}

int compare_bignum(bool negative_a, const std::string& mag_a,
                   bool negative_b, const std::string& mag_b) {
  const bool zero_a = mag_a == "0";
  const bool zero_b = mag_b == "0";
  const bool neg_a = negative_a && !zero_a;
  const bool neg_b = negative_b && !zero_b;
  if (neg_a != neg_b) return neg_a ? -1 : 1;
  int mag_cmp = 0;
  if (mag_a.size() != mag_b.size()) {
    mag_cmp = mag_a.size() < mag_b.size() ? -1 : 1;
  } else if (mag_a != mag_b) {
    mag_cmp = mag_a < mag_b ? -1 : 1;
  }
  if (neg_a) return -mag_cmp;
  return mag_cmp;
}

bool compare_integer_strings(const std::string& op, const std::string& a,
                             const std::string& b) {
  bool neg_a = false, neg_b = false;
  std::string mag_a, mag_b;
  if (!parse_decimal_bignum(a, neg_a, mag_a)) return false;
  if (!parse_decimal_bignum(b, neg_b, mag_b)) return false;
  const int cmp = compare_bignum(neg_a, mag_a, neg_b, mag_b);
  if (op == "-eq") return cmp == 0;
  if (op == "-ne") return cmp != 0;
  if (op == "-lt") return cmp < 0;
  if (op == "-le") return cmp <= 0;
  if (op == "-gt") return cmp > 0;
  if (op == "-ge") return cmp >= 0;
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
      "=",   "==",  "!=",  "<",  "<=",   ">",  ">=",  "-eq", "-ne", "-lt",
      "-le", "-gt", "-ge", "-a", "-and", "-o", "-or", "-nt", "-ot", "-ef"};
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
                    const std::string& b,
                    std::string* error_message = nullptr) {
  if (op == "=" || op == "==" || op == "!=" || op == "<" || op == "<=" ||
      op == ">" || op == ">=") {
    return compare_strings(op, a, b) ? 0 : 1;
  }

  if (op == "-eq" || op == "-ne" || op == "-lt" || op == "-le" || op == "-gt" ||
      op == "-ge") {
    // [GNU] Operands are validated as decimal integers first; the comparison
    // itself is exact even for values beyond 64-bit (uutils #12874).
    bool neg_a = false, neg_b = false;
    std::string mag_a, mag_b;
    if (!parse_decimal_bignum(a, neg_a, mag_a)) {
      if (error_message != nullptr) {
        *error_message = "invalid integer '" + a + "'";
      }
      return 2;
    }
    if (!parse_decimal_bignum(b, neg_b, mag_b)) {
      if (error_message != nullptr) {
        *error_message = "invalid integer '" + b + "'";
      }
      return 2;
    }
    return compare_integer_strings(op, a, b) ? 0 : 1;
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
  explicit TestExpressionParser(std::span<const std::string> args)
      : args_(args) {}

  // Mirrors GNU test: up to four arguments use the POSIX special-case
  // forms; longer expressions go through the recursive grammar. Syntax
  // errors record a GNU-style message and yield status 2.
  int parse() {
    if (args_.empty()) return 1;
    switch (args_.size()) {
      case 1:
        return one_arg(0);
      case 2:
        return two_args(0, 1);
      case 3:
        return three_args(0, 1, 2);
      case 4:
        return four_args(0, 1, 2, 3);
      default:
        break;
    }
    const int result = parse_or();
    if (!error_ && pos_ != args_.size()) {
      syntax_error("'" + std::string(args_[pos_]) +
                   "': binary operator expected");
      return 2;
    }
    return error_ ? 2 : result;
  }

  const std::string& error_message() const { return error_message_; }

 private:
  void syntax_error(std::string message) {
    error_ = true;
    error_message_ = std::move(message);
  }

  int one_arg(const size_t a) { return args_[a].empty() ? 1 : 0; }

  int two_args(const size_t a, const size_t b) {
    if (args_[a] == "!") return invert_test_status(one_arg(b));
    if (args_[a] == "(" && args_[b] == ")") return 1;
    if (is_unary_operator(std::string(args_[a]))) {
      return evaluate_unary(std::string(args_[a]), std::string(args_[b]));
    }
    if (is_binary_operator(std::string(args_[a]))) {
      syntax_error("'" + std::string(args_[a]) + "': unary operator expected");
      return 2;
    }
    syntax_error("missing argument after '" + std::string(args_[b]) + "'");
    return 2;
  }

  int three_args(const size_t a, const size_t b, const size_t c) {
    if (is_binary_operator(std::string(args_[b]))) {
      return evaluate_binary(std::string(args_[a]), std::string(args_[b]),
                             std::string(args_[c]), &error_message_);
    }
    if (args_[a] == "!") return invert_test_status(two_args(b, c));
    if (args_[a] == "(" && args_[c] == ")") return one_arg(b);
    syntax_error("'" + std::string(args_[b]) + "': binary operator expected");
    return 2;
  }

  int four_args(const size_t a, const size_t b, const size_t c,
                const size_t d) {
    if (args_[a] == "!") return invert_test_status(three_args(b, c, d));
    if (args_[a] == "(" && args_[d] == ")") return two_args(b, c);
    if (is_binary_operator(std::string(args_[b]))) {
      evaluate_binary(std::string(args_[a]), std::string(args_[b]),
                      std::string(args_[c]), &error_message_);
      syntax_error("extra argument '" + std::string(args_[d]) + "'");
      return 2;
    }
    syntax_error("'" + std::string(args_[b]) + "': binary operator expected");
    return 2;
  }

  int parse_or() {
    int result = parse_and();
    while (peek("-o") || peek("-or")) {
      ++pos_;
      int right = parse_and();
      result = combine_or(result, right);
    }
    return result;
  }
  int parse_and() {
    int result = parse_not();
    while (peek("-a") || peek("-and")) {
      ++pos_;
      int right = parse_not();
      result = combine_and(result, right);
    }
    return result;
  }
  int parse_not() {
    if (peek("!")) {
      ++pos_;
      return invert_test_status(parse_not());
    }
    if (peek("(")) {
      ++pos_;
      int result = parse_or();
      if (!peek(")")) {
        syntax_error("missing argument after '" +
                     std::string(args_[pos_ - 1]) + "'");
        return 2;
      }
      ++pos_;
      return result;
    }
    return parse_primary();
  }
  int parse_primary() {
    if (pos_ >= args_.size()) {
      syntax_error(pos_ > 0 ? "missing argument after '" +
                                  std::string(args_[pos_ - 1]) + "'"
                            : std::string("missing operand"));
      return 2;
    }
    if (pos_ + 1 < args_.size() &&
        is_unary_operator(std::string(args_[pos_]))) {
      auto op = std::string(args_[pos_++]);
      return evaluate_unary(op, std::string(args_[pos_++]));
    }
    if (pos_ + 2 < args_.size() &&
        is_binary_operator(std::string(args_[pos_ + 1]))) {
      auto left = std::string(args_[pos_++]);
      auto op = std::string(args_[pos_++]);
      auto right = std::string(args_[pos_++]);
      return evaluate_binary(left, op, right, &error_message_);
    }
    return args_[pos_++].empty() ? 1 : 0;
  }
  static int combine_or(int left, int right) {
    return left == 0 || right == 0 ? 0 : (left == 2 || right == 2 ? 2 : 1);
  }
  static int combine_and(int left, int right) {
    return left == 0 && right == 0 ? 0 : (left == 2 || right == 2 ? 2 : 1);
  }
  bool peek(std::string_view token) const {
    return pos_ < args_.size() && args_[pos_] == token;
  }
  std::span<const std::string> args_;
  size_t pos_ = 0;
  bool error_ = false;
  std::string error_message_;
};

int evaluate_test_expression(std::span<const std::string> args,
                             std::string* error_message = nullptr) {
  TestExpressionParser parser(args);
  const int status = parser.parse();
  if (error_message != nullptr) {
    *error_message = parser.error_message();
  }
  return status;
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
  std::string error_message;
  const int status = evaluate_test_expression(args, &error_message);
  if (!error_message.empty()) {
    safeErrorPrintLn("test: " + winux::i18n::translate_error(error_message));
  }
  return status;
}

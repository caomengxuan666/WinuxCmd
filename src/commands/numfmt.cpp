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
 *  - File: numfmt.cpp
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for numfmt command.
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

auto constexpr NUMFMT_OPTIONS = std::array{
    // [GNU]
    OPTION("-d", "--delimiter",
           "use X instead of whitespace for field delimiter", STRING_TYPE),
    // [GNU]
    OPTION("", "--from", "autoconvert from X", STRING_TYPE),
    // [GNU]
    OPTION("", "--to", "autoconvert to X", STRING_TYPE),
    // [GNU]
    OPTION("", "--round", "use METHOD for rounding", STRING_TYPE),
    // [GNU]
    OPTION("", "--padding", "pad numbers to width N", INT_TYPE),
    // [EXT]
    OPTION("", "--pad", "pad numbers to width N", INT_TYPE),
    // [GNU]
    OPTION("", "--suffix", "add STRING after formatted numbers", STRING_TYPE),
    // [GNU]
    OPTION("", "--field", "replace numbers in field N", INT_TYPE),
    // [GNU]
    OPTION("-f", "--format", "use printf style floating-point FORMAT",
           STRING_TYPE),
    // [GNU] --header[=N]: optional value; a bare --header means N=1 and the
    // next argument stays an input operand (uutils #13272)
    OPTION("", "--header", "print the first N header lines unchanged",
           OPTIONAL_INT_TYPE),
    // [GNU]
    OPTION("", "--grouping", "group digits with locale thousands separator",
           BOOL_TYPE),
    // [GNU]
    OPTION("", "--invalid",
           "set policy for invalid values: 'abort' (default), 'warn', 'ignore'",
           STRING_TYPE),
    // [GNU]
    OPTION("", "--debug", "print conversion diagnostics", BOOL_TYPE),
    // [GNU] --from-unit: multiply input numbers by UNIT
    OPTION("", "--from-unit", "multiply input numbers by UNIT", STRING_TYPE),
    // [GNU] --to-unit: divide numbers by UNIT before formatting
    OPTION("", "--to-unit", "divide numbers by UNIT before formatting",
           STRING_TYPE),
    // [GNU] --unit-separator: use STRING between number and unit
    OPTION("", "--unit-separator", "use STRING between number and unit",
           STRING_TYPE),
    // [GNU] --zero-terminated: line delimiter is NUL, not newline
    OPTION("-z", "--zero-terminated", "line delimiter is NUL, not newline",
           BOOL_TYPE),
    // [GNU] -M: use IEC units (powers of 1024)
    OPTION("-M", "", "use IEC units (powers of 1024)", BOOL_TYPE),
    // [GNU] -l: use locale grouping
    OPTION("-l", "", "use locale grouping", BOOL_TYPE)};

// ======================================================
// Helper functions
// ======================================================

namespace {
// Apply rounding mode to a double value
double apply_rounding(double value, const std::string& mode) {
  if (mode.empty() || mode == "nearest") {
    // Default: round half away from zero
    return (value >= 0) ? std::floor(value + 0.5) : std::ceil(value - 0.5);
  } else if (mode == "up") {
    return std::ceil(value);
  } else if (mode == "down") {
    return std::floor(value);
  } else if (mode == "from-zero") {
    return (value >= 0) ? std::ceil(value) : std::floor(value);
  } else if (mode == "towards-zero") {
    return std::trunc(value);
  }
  // Unknown mode, default to nearest
  return (value >= 0) ? std::floor(value + 0.5) : std::ceil(value - 0.5);
}

// Parse number with SI suffixes (K, M, G, T, P)
enum class NumParseStatus { Ok, InvalidNumber, InvalidSuffix };

NumParseStatus parse_number_ex(const std::string& s, long long& result,
                               const std::string& round_mode = "",
                               std::string from = "",
                               double* raw_value = nullptr) {
  size_t i = 0;
  if (i < s.size() && (s[i] == '-' || s[i] == '+')) ++i;
  const size_t num_start = i;
  while (i < s.size() &&
         (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.')) {
    ++i;
  }
  const std::string num_str = s.substr(0, i);
  const std::string suffix_text = s.substr(i);
  if (num_str.empty() ||
      num_str.find_first_of("0123456789") == std::string::npos) {
    return NumParseStatus::InvalidNumber;
  }

  // Multiplier lookup; anything that is not exactly one recognized suffix
  // character (or empty) is rejected the way GNU reports it.
  double multiplier = 1.0;
  if (!suffix_text.empty()) {
    if (suffix_text.size() != 1) return NumParseStatus::InvalidSuffix;
    const char suffix = static_cast<char>(std::toupper(
        static_cast<unsigned char>(suffix_text[0])));
    const bool si = from == "si" || from == "auto";
    const double base = si ? 1000.0 : 1024.0;
    switch (suffix) {
      case 'K':
        multiplier = base;
        break;
      case 'M':
        multiplier = base * base;
        break;
      case 'G':
        multiplier = base * base * base;
        break;
      case 'T':
        multiplier = std::pow(base, 4);
        break;
      case 'P':
        multiplier = std::pow(base, 5);
        break;
      case 'E':
        multiplier = std::pow(base, 6);
        break;
      default:
        return NumParseStatus::InvalidSuffix;
    }
  }

  try {
    double num = std::stod(num_str);
    if (raw_value != nullptr) *raw_value = num * multiplier;
    if (suffix_text.empty() &&
        (num > 9.2233720368547748e18 || num < -9.2233720368547748e18)) {
      // Keep the exact value for the "value too large to be printed"
      // diagnostic; callers that scale (--to) can still use the double.
      result = 0;
      return NumParseStatus::Ok;
    }
    num *= multiplier;
    // Apply rounding mode when converting from human-readable
    result = static_cast<long long>(apply_rounding(num, round_mode));
    return NumParseStatus::Ok;
  } catch (...) {
    return NumParseStatus::InvalidNumber;
  }
}

bool parse_number(const std::string& s, long long& result,
                  const std::string& round_mode = "", std::string from = "") {
  return parse_number_ex(s, result, round_mode, std::move(from)) ==
         NumParseStatus::Ok;
}

bool parse_number_value(const std::string& s, double& result,
                        std::string from = "") {
  std::string num_str;
  char suffix = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    if (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.' ||
        ((s[i] == '-' || s[i] == '+') && num_str.empty())) {
      num_str += s[i];
    } else {
      suffix =
          static_cast<char>(std::toupper(static_cast<unsigned char>(s[i])));
      break;
    }
  }
  try {
    result = std::stod(num_str);
    const double base = (from == "si" || from == "auto") ? 1000.0 : 1024.0;
    switch (suffix) {
      case 'K':
        result *= base;
        break;
      case 'M':
        result *= std::pow(base, 2);
        break;
      case 'G':
        result *= std::pow(base, 3);
        break;
      case 'T':
        result *= std::pow(base, 4);
        break;
      case 'P':
        result *= std::pow(base, 5);
        break;
      case 0:
        break;
      default:
        return false;
    }
    return true;
  } catch (...) {
    return false;
  }
}

// Scale value by base repeatedly and format the magnitude; returns the
// numeric text and the selected suffix separately so a caller can reformat
// the number with a user --format.
std::pair<std::string, std::string> scale_to(double value, double base,
                                             std::string_view suffixes) {
  size_t index = 0;
  while (std::abs(value) >= base && index < suffixes.size()) {
    value /= base;
    ++index;
  }
  char buffer[64];
  if (index == 0) {
    // [GNU] Unscaled values print without a fractional part ("500").
    snprintf(buffer, sizeof(buffer), "%.0f", value);
  } else {
    snprintf(buffer, sizeof(buffer), "%.1f", value);
  }
  return {buffer, index == 0 ? std::string() : std::string(1, suffixes[index - 1])};
}
}  // namespace

// ======================================================
// Main command implementation
// ======================================================

REGISTER_COMMAND(
    numfmt,
    /* cmd_name */ "numfmt",
    /* cmd_synopsis */ "numfmt [OPTION]... [NUMBER]...",
    /* cmd_desc */
    "Convert numbers from/to human-readable strings.\n"
    "Convert numbers to/from human-readable strings (e.g., 1K, 1M).\n"
    "If no number is specified, read from standard input.",
    /* examples */
    "  numfmt --to=si 1024\n"
    "  echo 1M | numfmt --from=si\n"
    "  numfmt --to=iec --padding=10 1024",
    /* see_also */ "human-readable, bytesize",
    /* author */ "WinuxCmd",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */ NUMFMT_OPTIONS) {
  std::string to_unit = ctx.get<std::string>("--to", "");
  const std::string from_unit_value = ctx.get<std::string>("--from-unit", "");
  const std::string to_unit_value = ctx.get<std::string>("--to-unit", "");
  const std::string unit_separator =
      ctx.get<std::string>("--unit-separator", "");
  const bool zero_terminated =
      ctx.get<bool>("--zero-terminated", false) || ctx.get<bool>("-z", false);
  const bool iec_short = ctx.get<bool>("-M", false);
  // [GNU] --to accepts only none/si/iec/iec-i ("auto" is --from-only);
  // uutils #11662.
  if (!to_unit.empty() && to_unit != "none" && to_unit != "si" &&
      to_unit != "iec" && to_unit != "iec-i") {
    safeErrorPrint("numfmt: invalid argument '" + to_unit + "' for '--to'\n");
    safeErrorPrint(::winux::i18n::format(
        "command.numfmt.error.valid_to_args",
        "Valid arguments are:\n  - 'none'\n  - 'si'\n  - 'iec'\n  - 'iec-i'\n"));
    return 1;
  }
  bool to_si = to_unit == "si";
  bool to_iec = to_unit == "iec";
  bool to_iec_i = to_unit == "iec-i";
  std::string from_unit = ctx.get<std::string>("--from", "");
  std::string format_str = ctx.get<std::string>("--format", "");
  if (format_str.empty()) {
    format_str = ctx.get<std::string>("-f", "");
  }
  std::string delimiter = ctx.get<std::string>("--delimiter", "");
  if (delimiter.empty()) {
    delimiter = ctx.get<std::string>("-d", "");
  }
  int header = 0;
  header = ctx.get<int>("--header", 0);
  if (header < 0) {
    // bare --header (no =N): GNU defaults to one header line
    header = 1;
  }
  bool grouping = ctx.get<bool>("--grouping", false);
  const bool locale_grouping = grouping || ctx.get<bool>("-l", false);
  std::string invalid_policy = ctx.get<std::string>("--invalid", "abort");
  bool debug = ctx.get<bool>("--debug", false);
  std::string round_mode = ctx.get<std::string>("--round", "");
  int padding = ctx.get<int>("--padding", ctx.get<int>("--pad", 0));
  std::string suffix = ctx.get<std::string>("--suffix", "");
  int selected_field = ctx.get<int>("--field", 0);

  auto debug_log = [&](const std::string& msg) {
    if (debug) {
      safeErrorPrint("numfmt: debug: " + msg + "\n");
    }
  };

  auto parse_unit_scale = [](const std::string& text) -> double {
    if (text.empty()) return 1.0;
    try {
      size_t used = 0;
      double value = std::stod(text, &used);
      if (used == text.size()) return value;
      if (used + 1 == text.size()) {
        const char suffix = static_cast<char>(
            std::toupper(static_cast<unsigned char>(text[used])));
        switch (suffix) {
          case 'K':
            return value * 1000.0;
          case 'M':
            return value * 1000000.0;
          case 'G':
            return value * 1000000000.0;
          case 'T':
            return value * 1000000000000.0;
          case 'P':
            return value * 1000000000000000.0;
          default:
            return 0.0;
        }
      }
    } catch (...) {
    }
    return 0.0;
  };
  const double from_scale = parse_unit_scale(from_unit_value);
  const double to_scale = parse_unit_scale(to_unit_value);
  if (from_scale <= 0.0 || to_scale <= 0.0) {
    safeErrorPrintLn("numfmt: invalid unit value");
    return 1;
  }
  if (iec_short && to_unit.empty()) {
    to_unit = "iec-i";
    to_iec_i = true;
  }

  auto add_grouping = [](const std::string& s) -> std::string {
    // Add thousands separator commas
    bool negative = !s.empty() && s[0] == '-';
    std::string digits = negative ? s.substr(1) : s;
    std::string result;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
      if (count > 0 && count % 3 == 0) result = "," + result;
      result = digits[i] + result;
      ++count;
    }
    return negative ? "-" + result : result;
  };

  auto apply_format = [&](long long num) -> std::string {
    std::string result;
    if (!format_str.empty()) {
      // Simple printf-style format support
      char buf[256];
      snprintf(buf, sizeof(buf), format_str.c_str(), static_cast<double>(num));
      result = buf;
    } else {
      result = std::to_string(num);
    }
    if (locale_grouping) {
      result = add_grouping(result);
    }
    if (padding > 0 && static_cast<int>(result.size()) < padding) {
      result = std::string(padding - result.size(), ' ') + result;
    }
    return result;
  };

  bool had_invalid = false;
  // [GNU] numfmt.c keeps formatted numbers in a 128-byte buffer; anything
  // longer fails with "failed to prepare value '%f' for printing"
  // (uutils #12596).
  constexpr size_t kMaxNumericLength = 126;
  bool had_prepare_error = false;
  auto prepare_padded = [&](double value, const std::string& formatted)
      -> std::optional<std::string> {
    if (formatted.size() > kMaxNumericLength) {
      char value_text[64];
      snprintf(value_text, sizeof(value_text), "%f", value);
      safeErrorPrint(std::string("numfmt: failed to prepare value '") +
                     value_text + "' for printing\n");
      had_prepare_error = true;
      return std::nullopt;
    }
    return formatted;
  };

  auto process_number = [&](const std::string& s) -> std::optional<std::string> {
    long long num = 0;
    double raw_value = 0.0;
    const auto status =
        parse_number_ex(s, num, round_mode, from_unit, &raw_value);
    if (status != NumParseStatus::Ok) {
      if (debug) {
        debug_log("failed to parse input '" + s + "'");
      }
      const bool suffix_issue = status == NumParseStatus::InvalidSuffix;
      const char* problem = suffix_issue ? "invalid suffix in input"
                                         : "invalid number";
      if (invalid_policy == "warn") {
        safeErrorPrint(std::string("numfmt: ") + problem + ": '" + s + "'\n");
        had_invalid = true;
        return s;
      }
      if (invalid_policy == "abort") {
        safeErrorPrint(std::string("numfmt: ") + problem + ": '" + s + "'\n");
        had_invalid = true;
        return std::nullopt;
      }
      // "ignore" - return as-is
      return s;
    }

    // [GNU] Raw (no --to, no --format) printing requires the value to fit
    // in intmax_t; larger values error out (uutils #11654).
    const bool raw_print = !to_si && !to_iec && !to_iec_i && format_str.empty();
    if (raw_print &&
        (raw_value > 9223372036854775807.0 ||
         raw_value < -9223372036854775808.0)) {
      char printed[64];
      snprintf(printed, sizeof(printed), "%g", raw_value);
      safeErrorPrint(std::string("numfmt: value too large to be printed: '") +
                     printed + "' (consider using --to)\n");
      had_invalid = true;
      return std::nullopt;
    }

    if (from_scale != 1.0 || to_scale != 1.0) {
      num = static_cast<long long>(
          std::llround(static_cast<double>(num) * from_scale / to_scale));
    }

    // Apply --from conversion
    if (!from_unit.empty()) {
      if (from_unit == "si") {
        // parse_number already handles SI suffixes
      } else if (from_unit == "iec") {
        // parse_number already handles IEC suffixes
      }
    }

    if (to_si || to_iec || to_iec_i) {
      // With --to, GNU computes in floating point even for values that do
      // not fit intmax_t ("12345678901234567890 --to=si" -> "12.3E").
      const bool out_of_intmax =
          raw_value > 9223372036854775807.0 ||
          raw_value < -9223372036854775808.0;
      double scaled = out_of_intmax ? raw_value : static_cast<double>(num);
      std::string formatted;
      std::string unit_suffix;
      if (to_iec_i) {
        std::tie(formatted, unit_suffix) = scale_to(scaled, 1024.0, "KiMiGiTiPi");
      } else if (to_si) {
        // [GNU] SI suffixes are upper case (5.0K, 123.5M); uutils #7221.
        std::tie(formatted, unit_suffix) = scale_to(scaled, 1000.0, "KMGTPE");
      } else {
        std::tie(formatted, unit_suffix) = scale_to(scaled, 1024.0, "KMGTPE");
      }
      std::optional<std::string> prepared;
      if (!format_str.empty()) {
        char buf[512];
        snprintf(buf, sizeof(buf), format_str.c_str(), scaled);
        prepared = prepare_padded(scaled, buf);
      } else {
        prepared = prepare_padded(scaled, formatted);
      }
      if (!prepared) {
        return std::nullopt;
      }
      if (debug) {
        debug_log("converted '" + s + "' -> '" + *prepared + suffix + "'");
      }
      return *prepared + (unit_separator.empty() ? "" : unit_separator) +
             unit_suffix + suffix;
    }
    if (!format_str.empty()) {
      double value = 0.0;
      if (parse_number_value(s, value, from_unit)) {
        char buf[512];
        snprintf(buf, sizeof(buf), format_str.c_str(), value);
        auto prepared = prepare_padded(value, buf);
        if (!prepared) {
          return std::nullopt;
        }
        if (debug) {
          debug_log("formatted '" + s + "' -> '" + *prepared + suffix + "'");
        }
        return *prepared + suffix;
      }
    }
    auto applied = apply_format(num) +
                   (unit_separator.empty() ? "" : unit_separator) + suffix;
    if (debug) {
      debug_log("formatted '" + s + "' -> '" + applied + "'");
    }
    return applied;
  };

  int line_num = 0;
  auto process_line = [&](const std::string& line) -> bool {
    if (line_num < header) {
      safePrint(line);
      safePrint(zero_terminated ? std::string(1, '\0') : std::string("\n"));
      ++line_num;
      return true;
    }
    if (!delimiter.empty()) {
      // Split by delimiter, process each field
      std::istringstream ss(line);
      std::string field_text;
      int field_number = 1;
      bool first = true;
      while (std::getline(ss, field_text, delimiter[0])) {
        if (!first) safePrint(delimiter);
        if (selected_field == 0 || selected_field == field_number) {
          auto processed = process_number(field_text);
          if (!processed) return false;
          safePrint(*processed);
        } else {
          safePrint(field_text);
        }
        first = false;
        ++field_number;
      }
      safePrint(zero_terminated ? std::string(1, '\0') : std::string("\n"));
    } else {
      auto processed = process_number(line);
      if (!processed) return false;
      safePrint(*processed);
      safePrint(zero_terminated ? std::string(1, '\0') : std::string("\n"));
    }
    ++line_num;
    return true;
  };

  // [GNU] Aborting on invalid input exits 2; a prepare failure exits 1.
  // NOTE: evaluated at return time — had_prepare_error is set during
  // processing, so it must not be captured before the processing loop.
  if (!ctx.positionals.empty()) {
    for (const auto& arg : ctx.positionals) {
      if (!process_line(std::string(arg))) return had_prepare_error ? 1 : 2;
    }
  } else {
    std::string input;
    input.assign(std::istreambuf_iterator<char>(std::cin),
                 std::istreambuf_iterator<char>());
    std::istringstream iss(input);
    std::string line;
    const char delimiter_char = zero_terminated ? '\0' : '\n';
    while (std::getline(iss, line, delimiter_char)) {
      if (!process_line(line)) return had_prepare_error ? 1 : 2;
    }
  }

  return had_invalid ? 1 : 0;
}

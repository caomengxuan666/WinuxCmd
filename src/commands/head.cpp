/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: head.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

#include "pch/pch.h"
// include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

/**
 * @brief HEAD command options definition
 *
 * This array defines all the options supported by the head command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 *
 * - @a -c, @a --bytes: Print the first NUM bytes of each file; with the leading
 * '-', print all but the last NUM bytes [IMPLEMENTED]
 * - @a -n, @a --lines: Print the first NUM lines instead of the first 10; with
 * the leading '-', print all but the last NUM lines [IMPLEMENTED]
 * - @a -q, @a --quiet: Never print headers giving file names for multiple files
 * [IMPLEMENTED]
 * - @a --silent: Never print headers giving file names for multiple files
 * [IMPLEMENTED]
 * - @a -v, @a --verbose: Always print headers giving file names for multiple
 * files [IMPLEMENTED]
 * - @a -z, @a --zero-terminated: Line delimiter is NUL, not newline
 * [IMPLEMENTED]
 */
auto constexpr HEAD_OPTIONS = std::array{
    OPTION("-c", "--bytes",
           "print the first NUM bytes of each file; with the leading '-',\n"
           "print all but the last NUM bytes",
           STRING_TYPE),
    OPTION("-n", "--lines",
           "print the first NUM lines instead of the first 10; with the\n"
           "leading '-', print all but the last NUM lines",
           STRING_TYPE),
    OPTION("-q", "--quiet",
           "never print headers giving file names for multiple files"),
    OPTION("", "--silent",
           "never print headers giving file names for multiple files"),
    OPTION("-v", "--verbose",
           "always print headers giving file names for multiple files"),
    OPTION("-z", "--zero-terminated", "line delimiter is NUL, not newline")};

namespace head_pipeline {
namespace cp = core::pipeline;

struct CountSpec {
  std::uintmax_t value = 10;
  bool all_but_last = false;
};

struct HeadConfig {
  bool by_bytes = false;
  CountSpec spec;
  bool quiet = false;
  bool verbose = false;
  char delimiter = '\n';
};

auto read_all(std::istream& in) -> std::string {
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

auto parse_uint(std::string_view text) -> std::optional<std::uintmax_t> {
  if (text.empty()) return std::nullopt;
  std::uintmax_t value = 0;
  auto [ptr, ec] =
      std::from_chars(text.data(), text.data() + text.size(), value);
  if (ec != std::errc() || ptr != text.data() + text.size()) {
    return std::nullopt;
  }
  return value;
}

auto parse_count_spec(std::string spec_text, std::string_view opt_name)
    -> cp::Result<CountSpec> {
  if (spec_text.empty()) {
    return std::unexpected("invalid number of " + std::string(opt_name));
  }

  CountSpec spec;
  if (spec_text[0] == '-') {
    spec.all_but_last = true;
    spec_text = spec_text.substr(1);  // Avoid modifying original string
    if (spec_text.empty()) {
      return std::unexpected("invalid number of " + std::string(opt_name));
    }
  }

  auto parsed = parse_uint(spec_text);
  if (!parsed.has_value()) {
    return std::unexpected("invalid number of " + std::string(opt_name));
  }
  spec.value = *parsed;
  return spec;
}

auto split_records(const std::string& data, char delimiter)
    -> std::vector<std::pair<size_t, size_t>> {
  std::vector<std::pair<size_t, size_t>> records;
  records.reserve(data.size() / 10);  // Estimate: assume ~10 chars per record
  size_t start = 0;
  for (size_t i = 0; i < data.size(); ++i) {
    if (data[i] == delimiter) {
      records.emplace_back(start, i + 1);
      start = i + 1;
    }
  }
  if (start < data.size()) {
    records.emplace_back(start, data.size());
  }
  return records;
}

auto print_ranges(const std::string& data,
                  const std::vector<std::pair<size_t, size_t>>& ranges,
                  size_t first, size_t last) -> void {
  // Reserve buffer for output to avoid multiple small allocations
  std::string out;
  out.reserve((last - first) * 80);  // Assume ~80 chars per record
  for (size_t i = first; i < last; ++i) {
    const auto [begin, end] = ranges[i];
    out.append(data.data() + begin, end - begin);
  }
  safePrint(out);
}

auto output_head(const std::string& data, const HeadConfig& config) -> void {
  if (config.by_bytes) {
    size_t keep = static_cast<size_t>(config.spec.value);
    if (config.spec.all_but_last) {
      if (keep >= data.size()) return;
      safePrint(std::string_view(data.data(), data.size() - keep));
      return;
    }

    size_t count = std::min(keep, data.size());
    if (count > 0) {
      safePrint(std::string_view(data.data(), count));
    }
    return;
  }

  auto ranges = split_records(data, config.delimiter);
  if (ranges.empty()) return;

  size_t total = ranges.size();
  size_t count = static_cast<size_t>(config.spec.value);

  if (config.spec.all_but_last) {
    if (count >= total) return;
    print_ranges(data, ranges, 0, total - count);
    return;
  }

  size_t take = std::min(count, total);
  print_ranges(data, ranges, 0, take);
}

auto read_input(std::string_view path) -> cp::Result<std::string> {
  if (path == "-") return read_all(std::cin);

  std::ifstream file(std::string(path), std::ios::binary);
  if (!file.is_open()) {
    return std::unexpected("cannot open '" + std::string(path) + "'");
  }

  return read_all(file);
}

template <size_t N>
auto build_config(const CommandContext<N>& ctx) -> cp::Result<HeadConfig> {
  HeadConfig config;
  config.quiet =
      ctx.get<bool>("--quiet", false) || ctx.get<bool>("--silent", false);
  config.verbose = ctx.get<bool>("--verbose", false);
  config.delimiter = ctx.get<bool>("--zero-terminated", false) ? '\0' : '\n';

  const std::string bytes_arg = ctx.get<std::string>("--bytes", "");
  const std::string bytes_short = ctx.get<std::string>("-c", "");
  const std::string lines_arg = ctx.get<std::string>("--lines", "");
  const std::string lines_short = ctx.get<std::string>("-n", "");

  std::string bytes_spec = bytes_arg.empty() ? bytes_short : bytes_arg;
  std::string lines_spec = lines_arg.empty() ? lines_short : lines_arg;

  if (!bytes_spec.empty()) {
    auto spec = parse_count_spec(bytes_spec, "bytes");
    if (!spec) return std::unexpected(spec.error());
    config.by_bytes = true;
    config.spec = *spec;
    return config;
  }

  if (!lines_spec.empty()) {
    auto spec = parse_count_spec(lines_spec, "lines");
    if (!spec) return std::unexpected(spec.error());
    config.spec = *spec;
  }

  return config;
}

}  // namespace head_pipeline

REGISTER_COMMAND(
    head, "head", "head [OPTION]... [FILE]...",
    "Print the first 10 lines of each FILE to standard output.\n"
    "With more than one FILE, precede each with a header giving the file "
    "name.\n"
    "\n"
    "With no FILE, or when FILE is -, read standard input.",
    "  head file.txt\n"
    "  head -n 20 file.txt\n"
    "  head -c 64 file.txt\n"
    "  head -n -5 file.txt\n"
    "  head -v a.txt b.txt",
    "tail(1), cat(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd", HEAD_OPTIONS) {
  using namespace head_pipeline;

  auto config_result = build_config(ctx);
  if (!config_result) {
    cp::report_error(config_result, L"head");
    return 1;
  }
  auto config = *config_result;

  // Use SmallVector for files (max 64 files) - all stack-allocated
  SmallVector<std::string, 64> files{};
  for (auto p : ctx.positionals) files.push_back(std::string(p));
  if (files.empty()) files.push_back("-");

  bool any_error = false;
  bool first_print = true;
  bool multi = files.size() > 1;

  for (size_t i = 0; i < files.size(); ++i) {
    const auto& file = files[i];
    auto data_result = read_input(file);
    if (!data_result) {
      safeErrorPrint("head: ");
      safeErrorPrint(data_result.error());
      safeErrorPrint("\n");
      any_error = true;
      continue;
    }

    bool show_header =
        (config.verbose || (multi && !config.quiet)) && file != "-";
    if (show_header) {
      if (!first_print) safePrint("\n");
      safePrint("==> ");
      safePrint(file);
      safePrint(" <==\n");
    }

    output_head(*data_result, config);
    first_print = false;
  }

  return any_error ? 1 : 0;
}

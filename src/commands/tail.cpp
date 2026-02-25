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
 *  - File: tail.cpp
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
 * @brief TAIL command options definition
 *
 * This array defines all the options supported by the tail command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 *
 * - @a -c, @a --bytes: Output the last NUM bytes; or use -c +NUM to output
 *   starting with byte NUM of each file [IMPLEMENTED]
 * - @a -f, @a --follow: Output appended data as the file grows [NOT SUPPORT]
 * - @a -F: Same as --follow=name --retry [NOT SUPPORT]
 * - @a -n, @a --lines: Output the last NUM lines, instead of the last 10; or
 *   use -n +NUM to skip NUM-1 lines at the start [IMPLEMENTED]
 * - @a --max-unchanged-stats: With --follow=name, reopen a FILE which has not
 * changed size after N iterations to see if it has been renamed [NOT SUPPORT]
 * - @a --pid: With -f, terminate after process ID, PID dies [NOT SUPPORT]
 * - @a -q, @a --quiet: Never output headers giving file names [IMPLEMENTED]
 * - @a --silent: Never output headers giving file names [IMPLEMENTED]
 * - @a --retry: Keep trying to open a file if it is inaccessible [NOT SUPPORT]
 * - @a -s, @a --sleep-interval: With -f, sleep for approximately N seconds
 * between iterations [NOT SUPPORT]
 * - @a -v, @a --verbose: Always output headers giving file names [IMPLEMENTED]
 * - @a -z, @a --zero-terminated: Line delimiter is NUL, not newline
 * [IMPLEMENTED]
 */
auto constexpr TAIL_OPTIONS = std::array{
    OPTION("-c", "--bytes",
           "output the last NUM bytes; or use -c +NUM to output\n"
           "starting with byte NUM of each file",
           STRING_TYPE),
    OPTION("-f", "--follow",
           "output appended data as the file grows [NOT SUPPORT]"),
    OPTION("-F", "", "same as --follow=name --retry [NOT SUPPORT]"),
    OPTION("-n", "--lines",
           "output the last NUM lines, instead of the last 10; or\n"
           "use -n +NUM to skip NUM-1 lines at the start",
           STRING_TYPE),
    OPTION("", "--max-unchanged-stats",
           "with --follow=name, reopen a FILE which has not changed\n"
           "size after N iterations to see if it has been renamed\n"
           "[NOT SUPPORT]",
           INT_TYPE),
    OPTION("", "--pid",
           "with -f, terminate after process ID, PID dies [NOT SUPPORT]",
           INT_TYPE),
    OPTION("-q", "--quiet", "never output headers giving file names"),
    OPTION("", "--silent", "never output headers giving file names"),
    OPTION("", "--retry",
           "keep trying to open a file if it is inaccessible [NOT SUPPORT]"),
    OPTION("-s", "--sleep-interval",
           "with -f, sleep for approximately N seconds between iterations\n"
           "[NOT SUPPORT]",
           STRING_TYPE),
    OPTION("-v", "--verbose", "always output headers giving file names"),
    OPTION("-z", "--zero-terminated", "line delimiter is NUL, not newline")};

namespace tail_pipeline {
namespace cp = core::pipeline;

struct CountSpec {
  std::uintmax_t value = 10;
  bool from_start = false;
};

struct TailConfig {
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

auto read_input(std::string_view path) -> cp::Result<std::string> {
  if (path == "-") return read_all(std::cin);

  std::ifstream file(std::string(path), std::ios::binary);
  if (!file.is_open()) {
    return std::unexpected("cannot open '" + std::string(path) + "'");
  }
  return read_all(file);
}

auto suffix_multiplier(std::string_view suffix)
    -> std::optional<std::uintmax_t> {
  static constexpr auto kMultipliers =
      make_constexpr_map<std::string_view, std::uintmax_t>(
          std::array<std::pair<std::string_view, std::uintmax_t>, 25>{
              std::pair{std::string_view{"b"}, 512ULL},
              std::pair{std::string_view{"kB"}, 1000ULL},
              std::pair{std::string_view{"K"}, 1024ULL},
              std::pair{std::string_view{"KiB"}, 1024ULL},
              std::pair{std::string_view{"MB"}, 1000ULL * 1000ULL},
              std::pair{std::string_view{"M"}, 1024ULL * 1024ULL},
              std::pair{std::string_view{"MiB"}, 1024ULL * 1024ULL},
              std::pair{std::string_view{"GB"}, 1000ULL * 1000ULL * 1000ULL},
              std::pair{std::string_view{"G"}, 1024ULL * 1024ULL * 1024ULL},
              std::pair{std::string_view{"GiB"}, 1024ULL * 1024ULL * 1024ULL},
              std::pair{std::string_view{"TB"},
                        1000ULL * 1000ULL * 1000ULL * 1000ULL},
              std::pair{std::string_view{"T"},
                        1024ULL * 1024ULL * 1024ULL * 1024ULL},
              std::pair{std::string_view{"TiB"},
                        1024ULL * 1024ULL * 1024ULL * 1024ULL},
              std::pair{std::string_view{"PB"},
                        1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL},
              std::pair{std::string_view{"P"},
                        1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL},
              std::pair{std::string_view{"PiB"},
                        1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL},
              std::pair{
                  std::string_view{"EB"},
                  1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL * 1000ULL},
              std::pair{std::string_view{"E"}, 1024ULL * 1024ULL * 1024ULL *
                                                   1024ULL * 1024ULL * 1024ULL},
              std::pair{
                  std::string_view{"EiB"},
                  1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL},
              std::pair{std::string_view{"Z"}, 1024ULL * 1024ULL * 1024ULL *
                                                   1024ULL * 1024ULL * 1024ULL *
                                                   1024ULL},
              std::pair{std::string_view{"Y"}, 1024ULL * 1024ULL * 1024ULL *
                                                   1024ULL * 1024ULL * 1024ULL *
                                                   1024ULL * 1024ULL},
              std::pair{std::string_view{"R"}, 0ULL},
              std::pair{std::string_view{"Q"}, 0ULL},
              std::pair{std::string_view{"ZB"}, 0ULL},
              std::pair{std::string_view{"YB"}, 0ULL}});

  if (suffix.empty()) return 1;

  if (auto it = kMultipliers.find(suffix); it != kMultipliers.end()) {
    auto mult = it->second;
    if (mult == 0ULL) return std::nullopt;
    return mult;
  }
  if (suffix == "RB" || suffix == "QB") {
    return std::nullopt;
  }

  return std::nullopt;
}

auto parse_numeric_with_suffix(std::string_view text)
    -> std::optional<std::uintmax_t> {
  if (text.empty()) return std::nullopt;

  size_t i = 0;
  while (i < text.size() && std::isdigit(static_cast<unsigned char>(text[i]))) {
    ++i;
  }
  if (i == 0) return std::nullopt;

  std::uintmax_t base = 0;
  auto [ptr, ec] = std::from_chars(text.data(), text.data() + i, base);
  if (ec != std::errc() || ptr != text.data() + i) return std::nullopt;

  auto mult = suffix_multiplier(text.substr(i));
  if (!mult.has_value()) return std::nullopt;

  if (*mult != 0 &&
      base > (std::numeric_limits<std::uintmax_t>::max() / *mult)) {
    return std::nullopt;
  }

  return base * *mult;
}

auto parse_count_spec(std::string spec_text, std::string_view opt_name)
    -> cp::Result<CountSpec> {
  if (spec_text.empty()) {
    return std::unexpected("invalid number of " + std::string(opt_name));
  }

  CountSpec spec;
  if (spec_text[0] == '+') {
    spec.from_start = true;
    spec_text = spec_text.substr(1);  // Avoid modifying original string
  }

  auto parsed = parse_numeric_with_suffix(spec_text);
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

auto output_tail(const std::string& data, const TailConfig& config) -> void {
  if (config.by_bytes) {
    size_t n = static_cast<size_t>(config.spec.value);
    if (config.spec.from_start) {
      size_t offset = n > 0 ? n - 1 : 0;
      if (offset >= data.size()) return;
      safePrint(std::string_view(data.data() + offset, data.size() - offset));
      return;
    }

    if (n >= data.size()) {
      if (!data.empty()) safePrint(std::string_view(data.data(), data.size()));
      return;
    }

    safePrint(std::string_view(data.data() + (data.size() - n), n));
    return;
  }

  auto ranges = split_records(data, config.delimiter);
  if (ranges.empty()) return;

  size_t total = ranges.size();
  size_t n = static_cast<size_t>(config.spec.value);

  if (config.spec.from_start) {
    size_t start = n > 0 ? n - 1 : 0;
    if (start >= total) return;
    print_ranges(data, ranges, start, total);
    return;
  }

  if (n >= total) {
    print_ranges(data, ranges, 0, total);
    return;
  }

  print_ranges(data, ranges, total - n, total);
}

template <size_t N>
auto check_unsupported(const CommandContext<N>& ctx) -> cp::Result<void> {
  if (ctx.get<bool>("--follow", false) || ctx.get<bool>("-f", false)) {
    return std::unexpected("--follow is [NOT SUPPORT] on this platform");
  }
  if (ctx.get<bool>("-F", false)) {
    return std::unexpected("-F is [NOT SUPPORT] on this platform");
  }
  if (ctx.get<int>("--max-unchanged-stats", -1) >= 0) {
    return std::unexpected("--max-unchanged-stats is [NOT SUPPORT]");
  }
  if (ctx.get<int>("--pid", -1) >= 0) {
    return std::unexpected("--pid is [NOT SUPPORT]");
  }
  if (ctx.get<bool>("--retry", false)) {
    return std::unexpected("--retry is [NOT SUPPORT]");
  }
  if (!ctx.get<std::string>("--sleep-interval", "").empty() ||
      !ctx.get<std::string>("-s", "").empty()) {
    return std::unexpected("--sleep-interval is [NOT SUPPORT]");
  }
  return {};
}

template <size_t N>
auto build_config(const CommandContext<N>& ctx) -> cp::Result<TailConfig> {
  TailConfig config;
  config.quiet =
      ctx.get<bool>("--quiet", false) || ctx.get<bool>("--silent", false);
  config.verbose = ctx.get<bool>("--verbose", false);
  config.delimiter = ctx.get<bool>("--zero-terminated", false) ? '\0' : '\n';

  auto unsupported = check_unsupported(ctx);
  if (!unsupported) return std::unexpected(unsupported.error());

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

}  // namespace tail_pipeline

REGISTER_COMMAND(
    tail, "tail", "tail [OPTION]... [FILE]...",
    "Print the last 10 lines of each FILE to standard output.\n"
    "With more than one FILE, precede each with a header giving the file "
    "name.\n"
    "\n"
    "With no FILE, or when FILE is -, read standard input.",
    "  tail file.txt\n"
    "  tail -n 20 file.txt\n"
    "  tail -n +5 file.txt\n"
    "  tail -c 64 file.txt\n"
    "  tail -v a.txt b.txt",
    "head(1), cat(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd", TAIL_OPTIONS) {
  using namespace tail_pipeline;

  auto config_result = build_config(ctx);
  if (!config_result) {
    cp::report_error(config_result, L"tail");
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
      safeErrorPrint("tail: ");
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

    output_tail(*data_result, config);
    first_print = false;
  }

  return any_error ? 1 : 0;
}

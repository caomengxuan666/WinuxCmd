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
 *  - File: sort.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

/// @Author: WinuxCmd
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implemention for sort.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr SORT_OPTIONS = std::array{
    OPTION("-b", "--ignore-leading-blanks",
           "ignore leading blanks"),
    OPTION("-d", "--dictionary-order",
           "consider only blanks and alphanumeric characters [NOT SUPPORT]"),
    OPTION("-f", "--ignore-case", "fold lower case to upper case"),
    OPTION("-g", "--general-numeric-sort",
           "compare according to general numerical value [NOT SUPPORT]"),
    OPTION("-i", "--ignore-nonprinting",
           "consider only printable characters [NOT SUPPORT]"),
    OPTION("-h", "--human-numeric-sort",
           "compare human readable numbers [NOT SUPPORT]"),
    OPTION("-M", "--month-sort",
           "compare as month names [NOT SUPPORT]"),
    OPTION("-m", "--merge", "merge already sorted files [NOT SUPPORT]"),
    OPTION("-n", "--numeric-sort", "compare according to string numerical value"),
    OPTION("-R", "--random-sort", "shuffle [NOT SUPPORT]"),
    OPTION("-r", "--reverse", "reverse the result of comparisons"),
    OPTION("-s", "--stable",
           "stabilize sort by disabling last-resort comparison [NOT SUPPORT]"),
    OPTION("-u", "--unique", "output only the first of equal runs"),
    OPTION("-z", "--zero-terminated", "line delimiter is NUL, not newline"),
    OPTION("-o", "--output", "write result to FILE instead of standard output",
           STRING_TYPE),
    OPTION("-t", "--field-separator",
           "use SEP instead of non-blank to blank transition", STRING_TYPE),
    OPTION("-k", "--key",
           "sort via a key; KEYDEF has form F[.C][,F[.C]]", STRING_TYPE)};

namespace sort_pipeline {
namespace cp = core::pipeline;

struct KeySpec {
  size_t start_field = 1;
  bool enabled = false;
};

struct Config {
  bool ignore_leading_blanks = false;
  bool ignore_case = false;
  bool numeric_sort = false;
  bool reverse = false;
  bool unique = false;
  char delimiter = '\n';
  std::optional<char> field_separator;
  std::string output_file;
  KeySpec key;
  std::vector<std::string> files;
};

auto read_all(std::istream& in) -> std::string {
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

auto read_source(std::string_view path) -> cp::Result<std::string> {
  if (path == "-") return read_all(std::cin);

  std::ifstream in(std::string(path), std::ios::binary);
  if (!in.is_open()) {
    return std::unexpected("cannot open '" + std::string(path) + "'");
  }
  return read_all(in);
}

auto split_records(std::string_view content, char delimiter)
    -> std::vector<std::string> {
  std::vector<std::string> out;
  size_t start = 0;
  for (size_t i = 0; i < content.size(); ++i) {
    if (content[i] == delimiter) {
      out.emplace_back(content.substr(start, i - start));
      start = i + 1;
    }
  }
  if (start < content.size()) {
    out.emplace_back(content.substr(start));
  }
  return out;
}

auto to_lower_ascii(std::string_view s) -> std::string {
  std::string out;
  out.reserve(s.size());
  for (unsigned char c : s) {
    out.push_back(static_cast<char>(std::tolower(c)));
  }
  return out;
}

auto ltrim_ascii(std::string_view s) -> std::string_view {
  size_t i = 0;
  while (i < s.size() &&
         std::isspace(static_cast<unsigned char>(s[i])) != 0) {
    ++i;
  }
  return s.substr(i);
}

auto parse_key_spec(std::string_view text) -> cp::Result<KeySpec> {
  KeySpec key;
  if (text.empty()) return key;

  auto comma = text.find(',');
  auto first = text.substr(0, comma);
  auto dot = first.find('.');
  if (dot != std::string_view::npos) {
    first = first.substr(0, dot);
  }

  if (first.empty()) {
    return std::unexpected("invalid key spec");
  }

  size_t value = 0;
  auto [ptr, ec] =
      std::from_chars(first.data(), first.data() + first.size(), value);
  if (ec != std::errc() || ptr != first.data() + first.size() || value == 0) {
    return std::unexpected("invalid key spec");
  }

  key.enabled = true;
  key.start_field = value;
  return key;
}

auto get_field_by_whitespace(std::string_view line, size_t index)
    -> std::string_view {
  size_t field = 0;
  size_t i = 0;
  while (i < line.size()) {
    while (i < line.size() &&
           std::isspace(static_cast<unsigned char>(line[i])) != 0) {
      ++i;
    }
    if (i >= line.size()) break;

    size_t start = i;
    while (i < line.size() &&
           std::isspace(static_cast<unsigned char>(line[i])) == 0) {
      ++i;
    }
    ++field;
    if (field == index) return line.substr(start, i - start);
  }
  return {};
}

auto get_field_by_separator(std::string_view line, size_t index, char sep)
    -> std::string_view {
  size_t field = 1;
  size_t start = 0;
  while (true) {
    size_t pos = line.find(sep, start);
    if (field == index) {
      if (pos == std::string_view::npos) return line.substr(start);
      return line.substr(start, pos - start);
    }
    if (pos == std::string_view::npos) break;
    start = pos + 1;
    ++field;
  }
  return {};
}

auto extract_key(std::string_view line, const Config& cfg) -> std::string_view {
  if (!cfg.key.enabled) return line;

  std::string_view key;
  if (cfg.field_separator.has_value()) {
    key = get_field_by_separator(line, cfg.key.start_field, *cfg.field_separator);
  } else {
    key = get_field_by_whitespace(line, cfg.key.start_field);
  }

  if (cfg.ignore_leading_blanks) key = ltrim_ascii(key);
  return key;
}

auto parse_double_strict(std::string_view s) -> std::optional<double> {
  auto trimmed = ltrim_ascii(s);
  if (trimmed.empty()) return std::nullopt;

  std::string local(trimmed);
  char* end = nullptr;
  errno = 0;
  const double value = std::strtod(local.c_str(), &end);
  if (end == local.c_str() || *end != '\0' || errno == ERANGE) {
    return std::nullopt;
  }
  return value;
}

auto compare_records(std::string_view a, std::string_view b, const Config& cfg)
    -> int {
  auto key_a = extract_key(a, cfg);
  auto key_b = extract_key(b, cfg);

  if (cfg.numeric_sort) {
    auto n_a = parse_double_strict(key_a);
    auto n_b = parse_double_strict(key_b);
    if (n_a.has_value() && n_b.has_value()) {
      if (*n_a < *n_b) return -1;
      if (*n_a > *n_b) return 1;
    }
  }

  std::string left = std::string(key_a);
  std::string right = std::string(key_b);
  if (cfg.ignore_case) {
    left = to_lower_ascii(left);
    right = to_lower_ascii(right);
  }

  if (left < right) return -1;
  if (left > right) return 1;

  if (a < b) return -1;
  if (a > b) return 1;
  return 0;
}

auto is_unsupported_used(const CommandContext<SORT_OPTIONS.size()>& ctx)
    -> std::optional<std::string_view> {
  if (ctx.get<bool>("--dictionary-order", false) || ctx.get<bool>("-d", false))
    return "--dictionary-order is [NOT SUPPORT]";
  if (ctx.get<bool>("--general-numeric-sort", false) ||
      ctx.get<bool>("-g", false))
    return "--general-numeric-sort is [NOT SUPPORT]";
  if (ctx.get<bool>("--ignore-nonprinting", false) || ctx.get<bool>("-i", false))
    return "--ignore-nonprinting is [NOT SUPPORT]";
  if (ctx.get<bool>("--human-numeric-sort", false) || ctx.get<bool>("-h", false))
    return "--human-numeric-sort is [NOT SUPPORT]";
  if (ctx.get<bool>("--month-sort", false) || ctx.get<bool>("-M", false))
    return "--month-sort is [NOT SUPPORT]";
  if (ctx.get<bool>("--merge", false) || ctx.get<bool>("-m", false))
    return "--merge is [NOT SUPPORT]";
  if (ctx.get<bool>("--random-sort", false) || ctx.get<bool>("-R", false))
    return "--random-sort is [NOT SUPPORT]";
  if (ctx.get<bool>("--stable", false) || ctx.get<bool>("-s", false))
    return "--stable is [NOT SUPPORT]";
  return std::nullopt;
}

auto build_config(const CommandContext<SORT_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;

  cfg.ignore_leading_blanks = ctx.get<bool>("--ignore-leading-blanks", false) ||
                              ctx.get<bool>("-b", false);
  cfg.ignore_case =
      ctx.get<bool>("--ignore-case", false) || ctx.get<bool>("-f", false);
  cfg.numeric_sort =
      ctx.get<bool>("--numeric-sort", false) || ctx.get<bool>("-n", false);
  cfg.reverse = ctx.get<bool>("--reverse", false) || ctx.get<bool>("-r", false);
  cfg.unique = ctx.get<bool>("--unique", false) || ctx.get<bool>("-u", false);
  cfg.delimiter =
      (ctx.get<bool>("--zero-terminated", false) || ctx.get<bool>("-z", false))
          ? '\0'
          : '\n';

  cfg.output_file = ctx.get<std::string>("--output", "");
  if (cfg.output_file.empty()) cfg.output_file = ctx.get<std::string>("-o", "");

  std::string sep = ctx.get<std::string>("--field-separator", "");
  if (sep.empty()) sep = ctx.get<std::string>("-t", "");
  if (!sep.empty()) {
    if (sep.size() != 1) {
      return std::unexpected("field separator must be a single character");
    }
    cfg.field_separator = sep[0];
  }

  std::string key_text = ctx.get<std::string>("--key", "");
  if (key_text.empty()) key_text = ctx.get<std::string>("-k", "");
  auto key = parse_key_spec(key_text);
  if (!key) return std::unexpected(key.error());
  cfg.key = *key;

  for (auto p : ctx.positionals) cfg.files.emplace_back(p);
  if (cfg.files.empty()) cfg.files.emplace_back("-");

  return cfg;
}

auto run(const Config& cfg) -> int {
  std::vector<std::string> records;
  for (const auto& file : cfg.files) {
    auto content = read_source(file);
    if (!content) {
      cp::report_error(content, L"sort");
      return 1;
    }
    auto chunk = split_records(*content, cfg.delimiter);
    records.insert(records.end(), std::make_move_iterator(chunk.begin()),
                   std::make_move_iterator(chunk.end()));
  }

  std::stable_sort(records.begin(), records.end(), [&](const auto& a, const auto& b) {
    int cmp = compare_records(a, b, cfg);
    if (cfg.reverse) return cmp > 0;
    return cmp < 0;
  });

  if (cfg.unique) {
    std::vector<std::string> unique_records;
    unique_records.reserve(records.size());
    for (const auto& rec : records) {
      if (unique_records.empty()) {
        unique_records.push_back(rec);
        continue;
      }
      if (compare_records(unique_records.back(), rec, cfg) != 0) {
        unique_records.push_back(rec);
      }
    }
    records = std::move(unique_records);
  }

  std::ostream* out = &std::cout;
  std::ofstream file_out;
  if (!cfg.output_file.empty()) {
    file_out.open(cfg.output_file, std::ios::binary | std::ios::trunc);
    if (!file_out.is_open()) {
      cp::report_custom_error(L"sort", L"cannot open output file");
      return 1;
    }
    out = &file_out;
  }

  for (const auto& rec : records) {
    (*out) << rec;
    (*out) << cfg.delimiter;
  }
  out->flush();
  return 0;
}

}  // namespace sort_pipeline

REGISTER_COMMAND(sort, "sort", "sort [OPTION]... [FILE]...",
                 "Sort lines of text files.\n"
                 "With no FILE, or when FILE is -, read standard input.",
                 "  sort a.txt\n"
                 "  sort -n -r data.txt\n"
                 "  sort -u -k 1 names.txt",
                 "uniq(1), grep(1), head(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", SORT_OPTIONS) {
  using namespace sort_pipeline;

  if (auto unsupported = is_unsupported_used(ctx); unsupported.has_value()) {
    cp::report_custom_error(L"sort", utf8_to_wstring(*unsupported));
    return 2;
  }

  auto cfg = build_config(ctx);
  if (!cfg) {
    cp::report_error(cfg, L"sort");
    return 1;
  }

  return run(*cfg);
}

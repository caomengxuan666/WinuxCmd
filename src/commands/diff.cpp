/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions.
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
 *  - File: diff.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implementation for diff.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "core/command_macros.h"
#include "pch/pch.h"

#pragma comment(lib, "advapi32.lib")
import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

/**
 * @brief DIFF command options definition
 *
 * This array defines all the options supported by the diff command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -q, @a --brief: Report only when files differ [IMPLEMENTED]
 * - @a -u, @a --unified: Output unified diff format [IMPLEMENTED]
 * - @a -y, @a --side-by-side: Output in two columns [IMPLEMENTED]
 * - @a -w, @a --ignore-all-space: Ignore all white space [IMPLEMENTED]
 * - @a
 * -B, @a --ignore-blank-lines: Ignore changes whose lines are all blank [NOT
 * SUPPORT]
 */
// [GNU] -a, --text: treat all files as text
// [GNU] -b, --ignore-space-change: ignore changes in amount of white space
// [GNU] --binary: read and write data as binary
// [GNU] --color: colorize the output
// [GNU] -d, --minimal: try hard to find a smaller set of changes
// [GNU] --diff-program=PROGRAM: use PROGRAM to compare files
// [GNU] -e, --ed: output an ed script
// [GNU] --exclude=PATTERN: exclude files that match PATTERN
// [GNU] --exclude-from=FILE: exclude files that match any pattern in FILE
// [GNU] --exclude-dir=PATTERN: exclude directories that match PATTERN
// [GNU] -f, --forward-ed: output an ed script for current changes
// [GNU] -i, --ignore-case: ignore case when comparing files
// [GNU] --ignore-file-name-case: ignore case when comparing file names
// [GNU] -I, --ignore-matching-lines=NUM: ignore changes that match NUM lines
// [GNU] -l, --paginate: pass output through pr
// [GNU] -n, --rcs: output an RCS-format diff
// [GNU] -N, --new-file: treat absent files as empty
// [GNU] --no-dereference: don't follow symbolic links
// [GNU] -p, --show-c-function: show which C function each change is in
// [GNU] --show-function-line=REGEXP: show the most recent line matching REGEXP
// [GNU] -r, --recursive: recursively compare any subdirectories found
// [GNU] --report-identical-files: report when two files are the same
// [GNU] -s, --report-identical-files: report when two files are the same
// [GNU] --strip-trailing-cr: strip trailing carriage return on input
// [GNU] -S, --starting-file=FILE: start with FILE when comparing directories
// [GNU] --suppress-blank-empty: suppress empty common lines
// [GNU] --suppress-common-lines: suppress common lines in side-by-side format
// [GNU] -t, --expand-tabs: expand tabs to spaces in output
// [GNU] -T, --initial-tab: tab stop every NUM output lines
// [GNU] --tabsize=NUM: tab stop every NUM (default 8) print positions
// [GNU] --unidirectional-new-file: treat absent first files as empty
// [GNU] -W, --width=NUM: output at most NUM (default 130) columns
// [GNU] -x, --exclude=PAT: exclude files that match PAT
// [GNU] -X, --exclude-from=FILE: exclude files that match any pattern in FILE
// [GNU] -Z, --strip-trailing-cr: strip trailing carriage return on input
auto constexpr DIFF_OPTIONS = std::array{
    // [GNU]
    OPTION("-q", "--brief", "report only when files differ"),
    // [GNU]
    OPTION("-u", "--unified", "output NUM (default 3) lines of unified context",
           OPTIONAL_INT_TYPE),
    // [GNU]
    OPTION("-U", "", "output NUM lines of unified context", INT_TYPE),
    // [GNU]
    OPTION("-c", "--context", "output NUM (default 3) lines of copied context",
           OPTIONAL_INT_TYPE),
    // [GNU]
    OPTION("-C", "", "output NUM lines of copied context", INT_TYPE),
    // [GNU]
    OPTION("", "--label", "use LABEL instead of file name", STRING_TYPE),
    // [GNU]
    OPTION("-y", "--side-by-side", "output in two columns"),
    // [GNU]
    OPTION("-w", "--ignore-all-space", "ignore all white space"),
    // [GNU]
    OPTION("-B", "--ignore-blank-lines",
           "ignore changes whose lines are all blank"),
    // [GNU]
    OPTION("-a", "--text", "treat all files as text"),
    // [GNU]
    OPTION("-b", "--ignore-space-change",
           "ignore changes in amount of white space"),
    // [GNU]
    OPTION("", "--binary", "read and write data as binary"),
    // [GNU]
    OPTION("", "--color", "colorize the output"),
    // [GNU]
    OPTION("-d", "--minimal", "try hard to find a smaller set of changes"),
    // [GNU]
    OPTION("", "--diff-program", "use PROGRAM to compare files", STRING_TYPE),
    // [GNU]
    OPTION("-e", "--ed", "output an ed script"),
    // [GNU]
    OPTION("", "--exclude", "exclude files that match PATTERN", STRING_TYPE),
    // [GNU]
    OPTION("", "--exclude-from", "exclude files matching pattern in FILE",
           STRING_TYPE),
    // [GNU]
    OPTION("", "--exclude-dir", "exclude directories matching PATTERN",
           STRING_TYPE),
    // [GNU]
    OPTION("-f", "--forward-ed", "output an ed script for current changes"),
    // [GNU]
    OPTION("-i", "--ignore-case", "ignore case when comparing files"),
    // [GNU]
    OPTION("", "--ignore-file-name-case", "ignore case for file names"),
    // [GNU]
    OPTION("-I", "--ignore-matching-lines", "ignore changes matching NUM lines",
           STRING_TYPE),
    // [GNU]
    OPTION("-l", "--paginate", "pass output through pr"),
    // [GNU]
    OPTION("-n", "--rcs", "output an RCS-format diff"),
    // [GNU]
    OPTION("-N", "--new-file", "treat absent files as empty"),
    // [GNU]
    OPTION("", "--no-dereference", "don't follow symbolic links"),
    // [GNU]
    OPTION("-p", "--show-c-function",
           "show which C function each change is in"),
    // [GNU]
    OPTION("", "--show-function-line", "show most recent line matching REGEXP",
           STRING_TYPE),
    // [GNU]
    OPTION("-r", "--recursive", "recursively compare subdirectories"),
    // [GNU]
    OPTION("", "--report-identical-files",
           "report when two files are the same"),
    // [GNU]
    OPTION("-s", "", "report when two files are the same"),
    // [GNU]
    OPTION("", "--strip-trailing-cr",
           "strip trailing carriage return on input"),
    // [GNU]
    OPTION("-S", "--starting-file", "start with FILE when comparing dirs",
           STRING_TYPE),
    // [GNU]
    OPTION("", "--suppress-blank-empty", "suppress empty common lines"),
    // [GNU]
    OPTION("", "--suppress-common-lines",
           "suppress common lines in side-by-side"),
    // [GNU]
    OPTION("-t", "--expand-tabs", "expand tabs to spaces in output"),
    // [GNU]
    OPTION("-T", "--initial-tab", "tab stop every NUM output lines"),
    // [GNU]
    OPTION("", "--tabsize", "tab stop every NUM print positions", INT_TYPE),
    // [GNU]
    OPTION("", "--unidirectional-new-file",
           "treat absent first files as empty"),
    // [GNU]
    OPTION("-W", "--width", "output at most NUM columns", INT_TYPE),
    // [GNU]
    OPTION("-x", "", "exclude files that match PAT", STRING_TYPE),
    // [GNU]
    OPTION("-X", "", "exclude files matching pattern in FILE", STRING_TYPE),
    // [GNU]
    OPTION("-Z", "", "strip trailing carriage return on input")};

namespace diff_pipeline {
namespace cp = core::pipeline;

auto resolve_files(const CommandContext<DIFF_OPTIONS.size()> &ctx)
    -> cp::Result<std::vector<std::string>> {
  std::vector<std::string> files;

  for (const auto &positional : ctx.positionals) {
    std::string file_arg = std::string(positional);
    if (contains_wildcard(file_arg)) {
      auto glob_result = glob_expand(file_arg);
      if (glob_result.expanded && !glob_result.files.empty()) {
        for (const auto &file : glob_result.files) {
          files.push_back(wstring_to_utf8(file));
        }
        continue;
      }
    }

    files.push_back(file_arg);
  }

  if (files.empty()) {
    return std::unexpected("missing operand");
  }
  if (files.size() < 2) {
    return std::unexpected("missing operand after '" + files.back() + "'");
  }
  if (files.size() > 2) {
    return std::unexpected("extra operand '" + files[2] + "'");
  }

  return files;
}

/**
 * @brief Edit operation type for diff
 */
enum class EditType { KEEP, DEL, INS };

/**
 * @brief Edit operation
 */
struct Edit {
  EditType type;
  size_t line1_index;  // Line index in file1 (for DEL/KEEP)
  size_t line2_index;  // Line index in file2 (for INS/KEEP)
};

/**
 * @brief Quick path: check if files are identical
 * @param lines1 Lines from first file
 * @param lines2 Lines from second file
 * @return true if files are identical
 */
auto is_identical(const std::vector<std::string> &lines1,
                  const std::vector<std::string> &lines2) -> bool {
  if (lines1.size() != lines2.size()) {
    return false;
  }
  return lines1 == lines2;
}

/**
 * @brief Compute LCS with hash optimization for fast comparison
 * @param lines1 Lines from first file
 * @param lines2 Lines from second file
 * @return LCS matrix
 */
auto compute_lcs_optimized(const std::vector<std::string> &lines1,
                           const std::vector<std::string> &lines2)
    -> std::vector<std::vector<size_t>> {
  size_t m = lines1.size();
  size_t n = lines2.size();

  // Fast path: if files are identical, no need to compute
  if (is_identical(lines1, lines2)) {
    std::vector<std::vector<size_t>> lcs(m + 1, std::vector<size_t>(n + 1, 0));
    for (size_t i = 0; i <= m; ++i) {
      lcs[i][i] = i;
    }
    return lcs;
  }

  // Precompute hashes for fast comparison
  std::vector<size_t> hash1;
  std::vector<size_t> hash2;
  hash1.reserve(m);
  hash2.reserve(n);

  for (const auto &line : lines1) {
    hash1.push_back(std::hash<std::string>{}(line));
  }
  for (const auto &line : lines2) {
    hash2.push_back(std::hash<std::string>{}(line));
  }

  // Create LCS matrix (needed for backtracking)
  std::vector<std::vector<size_t>> lcs(m + 1, std::vector<size_t>(n + 1, 0));

  for (size_t i = 1; i <= m; ++i) {
    for (size_t j = 1; j <= n; ++j) {
      // Compare hashes first, then confirm with string comparison
      if (hash1[i - 1] == hash2[j - 1] && lines1[i - 1] == lines2[j - 1]) {
        lcs[i][j] = lcs[i - 1][j - 1] + 1;
      } else {
        lcs[i][j] = std::max(lcs[i - 1][j], lcs[i][j - 1]);
      }
    }
  }

  return lcs;
}

/**
 * @brief Backtrack LCS matrix to find edit operations
 * @param lcs LCS matrix
 * @param lines1 Lines from first file
 * @param lines2 Lines from second file
 * @return Vector of edit operations
 */
auto backtrack_lcs(const std::vector<std::vector<size_t>> &lcs,
                   const std::vector<std::string> &lines1,
                   const std::vector<std::string> &lines2)
    -> std::vector<Edit> {
  std::vector<Edit> edits;
  size_t i = lines1.size();
  size_t j = lines2.size();

  while (i > 0 || j > 0) {
    if (i > 0 && j > 0 && lines1[i - 1] == lines2[j - 1]) {
      edits.push_back({EditType::KEEP, i - 1, j - 1});
      --i;
      --j;
    } else if (j > 0 && (i == 0 || lcs[i][j - 1] >= lcs[i - 1][j])) {
      edits.push_back({EditType::INS, i, j - 1});
      --j;
    } else {
      edits.push_back({EditType::DEL, i - 1, j});
      --i;
    }
  }

  std::reverse(edits.begin(), edits.end());
  return edits;
}

/**
 * @brief Compute diff using optimized LCS algorithm
 * @param lines1 Lines from first file
 * @param lines2 Lines from second file
 * @return Vector of edit operations
 */
auto compute_diff(const std::vector<std::string> &lines1,
                  const std::vector<std::string> &lines2) -> std::vector<Edit> {
  // Fast path: identical files
  if (is_identical(lines1, lines2)) {
    return {};
  }

  auto lcs = compute_lcs_optimized(lines1, lines2);
  return backtrack_lcs(lcs, lines1, lines2);
}

/**
 * @brief Read file into lines
 * @param path File path
 * @return Result with vector of lines
 */
auto read_file_lines_result(const std::string &path)
    -> cp::Result<std::vector<std::string>> {
  auto diff_input_open_error = [](std::string_view file_path) -> std::string {
    std::error_code ec;
    auto status =
        std::filesystem::status(std::filesystem::u8path(file_path), ec);
    if (!ec && status.type() == std::filesystem::file_type::directory) {
      return std::string(file_path) + ": Is a directory";
    }
    return "cannot open '" + std::string(file_path) +
           "' for reading: No such file or directory";
  };

  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    return std::unexpected(diff_input_open_error(path));
  }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }

  return lines;
}

auto normalize_line_for_compare(const std::string &line, bool ignore_all_space)
    -> std::string {
  if (!ignore_all_space) return line;
  std::string normalized;
  normalized.reserve(line.size());
  for (char ch : line) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
      normalized.push_back(ch);
    }
  }
  return normalized;
}

auto normalize_lines_for_compare(const std::vector<std::string> &lines,
                                 bool ignore_all_space)
    -> std::vector<std::string> {
  if (!ignore_all_space) return lines;
  std::vector<std::string> normalized;
  normalized.reserve(lines.size());
  for (const auto &line : lines) {
    normalized.push_back(normalize_line_for_compare(line, true));
  }
  return normalized;
}

auto strip_trailing_cr_lines(const std::vector<std::string> &lines)
    -> std::vector<std::string> {
  std::vector<std::string> result;
  result.reserve(lines.size());
  for (const auto &line : lines) {
    if (!line.empty() && line.back() == '\r') {
      result.emplace_back(line.substr(0, line.size() - 1));
    } else {
      result.push_back(line);
    }
  }
  return result;
}

auto filter_blank_lines(const std::vector<std::string> &lines)
    -> std::vector<std::string> {
  std::vector<std::string> result;
  for (const auto &line : lines) {
    bool all_space = true;
    for (char ch : line) {
      if (!std::isspace(static_cast<unsigned char>(ch))) {
        all_space = false;
        break;
      }
    }
    if (!all_space) {
      result.push_back(line);
    }
  }
  return result;
}

auto normalize_space_change(const std::string &line) -> std::string {
  std::string result;
  result.reserve(line.size());
  bool in_space = false;
  for (char ch : line) {
    if (std::isspace(static_cast<unsigned char>(ch))) {
      if (!in_space) {
        result.push_back(' ');
        in_space = true;
      }
    } else {
      result.push_back(ch);
      in_space = false;
    }
  }
  return result;
}

auto normalize_lines_space_change(const std::vector<std::string> &lines)
    -> std::vector<std::string> {
  std::vector<std::string> result;
  result.reserve(lines.size());
  for (const auto &line : lines) {
    result.push_back(normalize_space_change(line));
  }
  return result;
}

auto normalize_lines_case(const std::vector<std::string> &lines)
    -> std::vector<std::string> {
  std::vector<std::string> result;
  result.reserve(lines.size());
  for (const auto &line : lines) {
    std::string lowered;
    lowered.reserve(line.size());
    for (char ch : line) {
      lowered.push_back(
          static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    result.push_back(std::move(lowered));
  }
  return result;
}

auto filter_matching_lines(const std::vector<std::string> &lines,
                           const std::string &pattern)
    -> std::vector<std::string> {
  std::vector<std::string> result;
  for (const auto &line : lines) {
    if (line.find(pattern) == std::string::npos) {
      result.push_back(line);
    }
  }
  return result;
}

auto expand_tabs_in_line(const std::string &line, int tabsize) -> std::string {
  std::string result;
  int col = 0;
  for (char ch : line) {
    if (ch == '\t') {
      int spaces = tabsize - (col % tabsize);
      result.append(spaces, ' ');
      col += spaces;
    } else {
      result.push_back(ch);
      ++col;
    }
  }
  return result;
}

auto expand_tabs_lines(const std::vector<std::string> &lines, int tabsize)
    -> std::vector<std::string> {
  std::vector<std::string> result;
  result.reserve(lines.size());
  for (const auto &line : lines) {
    result.push_back(expand_tabs_in_line(line, tabsize));
  }
  return result;
}

/**
 * @brief Compare two files
 * @param path1 First file path
 * @param path2 Second file path
 * @param brief If true, only report if files differ
 * @return Result with true if files are equal
 */
auto compare_files(const std::string &path1, const std::string &path2,
                   bool brief, bool ignore_all_space) -> cp::Result<bool> {
  auto lines1_result = read_file_lines_result(path1);
  if (!lines1_result) {
    return std::unexpected(lines1_result.error());
  }

  auto lines2_result = read_file_lines_result(path2);
  if (!lines2_result) {
    return std::unexpected(lines2_result.error());
  }

  auto &lines1 = lines1_result.value();
  auto &lines2 = lines2_result.value();
  auto compare_lines1 = normalize_lines_for_compare(lines1, ignore_all_space);
  auto compare_lines2 = normalize_lines_for_compare(lines2, ignore_all_space);

  // Quick check: compare line counts
  if (compare_lines1.size() != compare_lines2.size()) {
    if (brief) {
      safePrint("Files ");
      safePrint(path1);
      safePrint(" and ");
      safePrint(path2);
      safePrint(" differ\n");
    }
    return false;
  }

  // Check if all lines are equal
  bool equal = true;
  for (size_t i = 0; i < compare_lines1.size(); ++i) {
    if (compare_lines1[i] != compare_lines2[i]) {
      equal = false;
      break;
    }
  }

  if (!equal && brief) {
    safePrint("Files ");
    safePrint(path1);
    safePrint(" and ");
    safePrint(path2);
    safePrint(" differ\n");
  }

  return equal;
}

auto filetime_ticks(FILETIME ft) -> uint64_t {
  ULARGE_INTEGER value{};
  value.LowPart = ft.dwLowDateTime;
  value.HighPart = ft.dwHighDateTime;
  return value.QuadPart;
}

auto format_unified_timestamp(const std::string &path) -> std::string {
  WIN32_FILE_ATTRIBUTE_DATA data{};
  if (!GetFileAttributesExW(utf8_to_wstring(path).c_str(),
                            GetFileExInfoStandard, &data)) {
    return "";
  }

  FILETIME utc_ft = data.ftLastWriteTime;
  FILETIME local_ft{};
  if (!FileTimeToLocalFileTime(&utc_ft, &local_ft)) {
    local_ft = utc_ft;
  }

  SYSTEMTIME st{};
  FileTimeToSystemTime(&local_ft, &st);

  int64_t offset_ticks = static_cast<int64_t>(filetime_ticks(local_ft)) -
                         static_cast<int64_t>(filetime_ticks(utc_ft));
  int offset_minutes = static_cast<int>(offset_ticks / (10'000'000LL * 60LL));
  char sign = '+';
  if (offset_minutes < 0) {
    sign = '-';
    offset_minutes = -offset_minutes;
  }

  uint64_t fractional_ns = (filetime_ticks(local_ft) % 10'000'000ULL) * 100ULL;
  char buf[96]{};
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%09llu %c%02d%02d",
           st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
           static_cast<unsigned long long>(fractional_ns), sign,
           offset_minutes / 60, offset_minutes % 60);
  return std::string(buf);
}

auto format_unified_range(size_t start_line, size_t count) -> std::string {
  std::string out = std::to_string(start_line);
  if (count != 1) {
    out += ",";
    out += std::to_string(count);
  }
  return out;
}

auto format_context_range(size_t start_line, size_t count) -> std::string {
  if (count == 1) return std::to_string(start_line);
  if (count == 0) return std::to_string(start_line) + ",0";
  return std::to_string(start_line) + "," +
         std::to_string(start_line + count - 1);
}

struct DiffHunk {
  size_t edit_start = 0;
  size_t edit_end = 0;
  size_t file1_start = 0;
  size_t file1_end = 0;
  size_t file2_start = 0;
  size_t file2_end = 0;
};

auto build_diff_hunks(const std::vector<Edit> &edits, size_t line1_count,
                      size_t line2_count, int context)
    -> std::vector<DiffHunk> {
  std::vector<DiffHunk> hunks;
  if (edits.empty()) return hunks;

  auto context_lines = static_cast<size_t>(std::max(context, 0));
  std::vector<std::pair<size_t, size_t>> groups;
  size_t hunk_start = 0;
  for (size_t i = 1; i < edits.size(); ++i) {
    size_t prev_line1 = edits[i - 1].line1_index;
    size_t curr_line1 = edits[i].line1_index;
    size_t prev_line2 = edits[i - 1].line2_index;
    size_t curr_line2 = edits[i].line2_index;

    size_t distance = std::max((curr_line1 > prev_line1 + context_lines)
                                   ? curr_line1 - prev_line1 - context_lines
                                   : 0,
                               (curr_line2 > prev_line2 + context_lines)
                                   ? curr_line2 - prev_line2 - context_lines
                                   : 0);

    if (distance > context_lines * 2) {
      groups.push_back({hunk_start, i});
      hunk_start = i;
    }
  }
  groups.push_back({hunk_start, edits.size()});

  for (auto [group_start, group_end] : groups) {
    DiffHunk hunk;
    hunk.edit_start = group_start;
    hunk.edit_end = group_end;
    hunk.file1_start = line1_count;
    hunk.file1_end = 0;
    hunk.file2_start = line2_count;
    hunk.file2_end = 0;

    for (size_t i = group_start; i < group_end; ++i) {
      const auto &edit = edits[i];
      if (edit.type == EditType::DEL) {
        hunk.file1_start = std::min(hunk.file1_start, edit.line1_index);
        hunk.file1_end = std::max(hunk.file1_end, edit.line1_index + 1);
      } else if (edit.type == EditType::INS) {
        hunk.file2_start = std::min(hunk.file2_start, edit.line2_index);
        hunk.file2_end = std::max(hunk.file2_end, edit.line2_index + 1);
      }
    }

    if (hunk.file1_start == line1_count) {
      hunk.file1_start = edits[group_start].line1_index;
      hunk.file1_end = hunk.file1_start;
    }
    if (hunk.file2_start == line2_count) {
      hunk.file2_start = edits[group_start].line2_index;
      hunk.file2_end = hunk.file2_start;
    }

    hunk.file1_start =
        static_cast<size_t>(std::max(static_cast<ptrdiff_t>(hunk.file1_start) -
                                         static_cast<ptrdiff_t>(context_lines),
                                     static_cast<ptrdiff_t>(0)));
    hunk.file1_end = std::min(hunk.file1_end + context_lines, line1_count);
    hunk.file2_start =
        static_cast<size_t>(std::max(static_cast<ptrdiff_t>(hunk.file2_start) -
                                         static_cast<ptrdiff_t>(context_lines),
                                     static_cast<ptrdiff_t>(0)));
    hunk.file2_end = std::min(hunk.file2_end + context_lines, line2_count);

    hunks.push_back(hunk);
  }

  return hunks;
}

struct DiffLabel {
  std::string text;
  bool custom = false;
};

void output_diff_file_header(std::string_view prefix, const DiffLabel &label,
                             const std::string &path) {
  safePrint(std::string(prefix));
  safePrint(label.text);
  if (!label.custom) {
    auto timestamp = format_unified_timestamp(path);
    if (!timestamp.empty()) {
      safePrint("\t");
      safePrint(timestamp);
    }
  }
  safePrint("\n");
}

/**
 * @brief Output unified diff format using LCS
 * @param path1 First file path
 * @param path2 Second file path
 * @param lines1 Lines from first file
 * @param lines2 Lines from second file
 * @param context Number of context lines
 */
auto output_unified_diff(const std::string &path1, const std::string &path2,
                         const std::vector<std::string> &compare_lines1,
                         const std::vector<std::string> &compare_lines2,
                         const std::vector<std::string> &lines1,
                         const std::vector<std::string> &lines2, int context,
                         const DiffLabel &label1, const DiffLabel &label2)
    -> void {
  auto edits = compute_diff(compare_lines1, compare_lines2);
  auto hunks = build_diff_hunks(edits, lines1.size(), lines2.size(), context);
  if (hunks.empty()) return;

  output_diff_file_header("--- ", label1, path1);
  output_diff_file_header("+++ ", label2, path2);

  for (const auto &hunk : hunks) {
    safePrint("@@ -");
    safePrint(format_unified_range(hunk.file1_start + 1,
                                   hunk.file1_end - hunk.file1_start));
    safePrint(" +");
    safePrint(format_unified_range(hunk.file2_start + 1,
                                   hunk.file2_end - hunk.file2_start));
    safePrint(" @@\n");

    size_t i1 = hunk.file1_start;
    size_t i2 = hunk.file2_start;
    for (size_t i = hunk.edit_start; i < hunk.edit_end; ++i) {
      const auto &edit = edits[i];

      if (edit.type == EditType::KEEP) {
        if (edit.line1_index < hunk.file1_start ||
            edit.line1_index >= hunk.file1_end ||
            edit.line2_index < hunk.file2_start ||
            edit.line2_index >= hunk.file2_end) {
          continue;
        }
        while (i1 < edit.line1_index && i1 < hunk.file1_end) {
          safePrint(" ");
          safePrint(lines1[i1]);
          safePrint("\n");
          ++i1;
          ++i2;
        }
        safePrint(" ");
        safePrint(lines1[edit.line1_index]);
        safePrint("\n");
        ++i1;
        ++i2;
      } else if (edit.type == EditType::DEL) {
        if (edit.line1_index < hunk.file1_start ||
            edit.line1_index >= hunk.file1_end) {
          continue;
        }
        while (i1 < edit.line1_index && i1 < hunk.file1_end) {
          safePrint(" ");
          safePrint(lines1[i1]);
          safePrint("\n");
          ++i1;
          ++i2;
        }
        safePrint("-");
        safePrint(lines1[edit.line1_index]);
        safePrint("\n");
        ++i1;
      } else {
        if (edit.line2_index < hunk.file2_start ||
            edit.line2_index >= hunk.file2_end) {
          continue;
        }
        while (i2 < edit.line2_index && i2 < hunk.file2_end) {
          safePrint(" ");
          safePrint(lines2[i2]);
          safePrint("\n");
          ++i1;
          ++i2;
        }
        safePrint("+");
        safePrint(lines2[edit.line2_index]);
        safePrint("\n");
        ++i2;
      }
    }

    while (i1 < hunk.file1_end && i2 < hunk.file2_end) {
      safePrint(" ");
      safePrint(lines1[i1]);
      safePrint("\n");
      ++i1;
      ++i2;
    }
  }
}

void output_context_old_section(const std::vector<Edit> &edits,
                                const std::vector<std::string> &lines1,
                                const DiffHunk &hunk, bool mixed_change) {
  size_t i1 = hunk.file1_start;
  for (size_t i = hunk.edit_start; i < hunk.edit_end; ++i) {
    const auto &edit = edits[i];
    if (edit.type == EditType::INS) continue;
    if (edit.type == EditType::KEEP) {
      if (edit.line1_index < hunk.file1_start ||
          edit.line1_index >= hunk.file1_end) {
        continue;
      }
      while (i1 < edit.line1_index && i1 < hunk.file1_end) {
        safePrint("  ");
        safePrint(lines1[i1]);
        safePrint("\n");
        ++i1;
      }
      safePrint("  ");
      safePrint(lines1[edit.line1_index]);
      safePrint("\n");
      ++i1;
      continue;
    }
    if (edit.line1_index < hunk.file1_start ||
        edit.line1_index >= hunk.file1_end) {
      continue;
    }
    while (i1 < edit.line1_index && i1 < hunk.file1_end) {
      safePrint("  ");
      safePrint(lines1[i1]);
      safePrint("\n");
      ++i1;
    }
    safePrint(mixed_change ? "! " : "- ");
    safePrint(lines1[edit.line1_index]);
    safePrint("\n");
    ++i1;
  }
  while (i1 < hunk.file1_end) {
    safePrint("  ");
    safePrint(lines1[i1]);
    safePrint("\n");
    ++i1;
  }
}

void output_context_new_section(const std::vector<Edit> &edits,
                                const std::vector<std::string> &lines2,
                                const DiffHunk &hunk, bool mixed_change) {
  size_t i2 = hunk.file2_start;
  for (size_t i = hunk.edit_start; i < hunk.edit_end; ++i) {
    const auto &edit = edits[i];
    if (edit.type == EditType::DEL) continue;
    if (edit.type == EditType::KEEP) {
      if (edit.line2_index < hunk.file2_start ||
          edit.line2_index >= hunk.file2_end) {
        continue;
      }
      while (i2 < edit.line2_index && i2 < hunk.file2_end) {
        safePrint("  ");
        safePrint(lines2[i2]);
        safePrint("\n");
        ++i2;
      }
      safePrint("  ");
      safePrint(lines2[edit.line2_index]);
      safePrint("\n");
      ++i2;
      continue;
    }
    if (edit.line2_index < hunk.file2_start ||
        edit.line2_index >= hunk.file2_end) {
      continue;
    }
    while (i2 < edit.line2_index && i2 < hunk.file2_end) {
      safePrint("  ");
      safePrint(lines2[i2]);
      safePrint("\n");
      ++i2;
    }
    safePrint(mixed_change ? "! " : "+ ");
    safePrint(lines2[edit.line2_index]);
    safePrint("\n");
    ++i2;
  }
  while (i2 < hunk.file2_end) {
    safePrint("  ");
    safePrint(lines2[i2]);
    safePrint("\n");
    ++i2;
  }
}

auto output_context_diff(const std::string &path1, const std::string &path2,
                         const std::vector<std::string> &compare_lines1,
                         const std::vector<std::string> &compare_lines2,
                         const std::vector<std::string> &lines1,
                         const std::vector<std::string> &lines2, int context,
                         const DiffLabel &label1, const DiffLabel &label2)
    -> void {
  auto edits = compute_diff(compare_lines1, compare_lines2);
  auto hunks = build_diff_hunks(edits, lines1.size(), lines2.size(), context);
  if (hunks.empty()) return;

  output_diff_file_header("*** ", label1, path1);
  output_diff_file_header("--- ", label2, path2);

  for (const auto &hunk : hunks) {
    bool has_delete = false;
    bool has_insert = false;
    for (size_t i = hunk.edit_start; i < hunk.edit_end; ++i) {
      has_delete = has_delete || edits[i].type == EditType::DEL;
      has_insert = has_insert || edits[i].type == EditType::INS;
    }
    const bool mixed_change = has_delete && has_insert;

    safePrint("***************\n");
    safePrint("*** ");
    safePrint(format_context_range(hunk.file1_start + 1,
                                   hunk.file1_end - hunk.file1_start));
    safePrint(" ****\n");
    output_context_old_section(edits, lines1, hunk, mixed_change);
    safePrint("--- ");
    safePrint(format_context_range(hunk.file2_start + 1,
                                   hunk.file2_end - hunk.file2_start));
    safePrint(" ----\n");
    output_context_new_section(edits, lines2, hunk, mixed_change);
  }
}

/**
 * @brief Output side-by-side diff format
 * @param path1 First file path
 * @param path2 Second file path
 * @param lines1 Lines from first file
 * @param lines2 Lines from second file
 */
auto output_side_by_side(const std::string &path1, const std::string &path2,
                         const std::vector<std::string> &compare_lines1,
                         const std::vector<std::string> &compare_lines2,
                         const std::vector<std::string> &lines1,
                         const std::vector<std::string> &lines2, int width,
                         bool suppress_common) -> void {
  auto edits = compute_diff(compare_lines1, compare_lines2);

  if (edits.empty()) {
    return;  // Files are identical
  }

  // -- Use caller-provided width, halved for each column [DIFFERS] --
  const size_t col_width = static_cast<size_t>(std::max(width / 2 - 1, 10));

  auto print_padded = [](const std::string &s, size_t width) {
    if (s.size() <= width) {
      safePrint(s);
      for (size_t i = s.size(); i < width; ++i) safePrint(" ");
    } else {
      safePrint(s.substr(0, width - 2));
      safePrint("..");
    }
  };

  size_t i1 = 0, i2 = 0;
  for (const auto &edit : edits) {
    if (edit.type == EditType::KEEP) {
      while (i1 < edit.line1_index) {
        // -- suppress-common-lines: skip unchanged lines [DIFFERS] --
        if (suppress_common) {
          ++i1;
          ++i2;
          continue;
        }
        print_padded(lines1[i1], col_width);
        safePrint("  ");
        print_padded(lines2[i2], col_width);
        safePrint("\n");
        ++i1;
        ++i2;
      }
      // -- suppress-common-lines: skip the kept line too [DIFFERS] --
      if (!suppress_common) {
        print_padded(lines1[edit.line1_index], col_width);
        safePrint("  ");
        print_padded(lines2[edit.line2_index], col_width);
        safePrint("\n");
      }
      ++i1;
      ++i2;
    } else if (edit.type == EditType::DEL) {
      print_padded(lines1[edit.line1_index], col_width);
      safePrint(" +");
      for (size_t p = 0; p < col_width; ++p) safePrint(" ");
      safePrint("\n");
      ++i1;
    } else {
      for (size_t p = 0; p < col_width; ++p) safePrint(" ");
      safePrint(" +");
      print_padded(lines2[edit.line2_index], col_width);
      safePrint("\n");
      ++i2;
    }
  }

  while (i1 < lines1.size()) {
    print_padded(lines1[i1], col_width);
    safePrint("  ");
    for (size_t p = 0; p < col_width; ++p) safePrint(" ");
    safePrint("\n");
    ++i1;
  }
  while (i2 < lines2.size()) {
    for (size_t p = 0; p < col_width; ++p) safePrint(" ");
    safePrint("  ");
    print_padded(lines2[i2], col_width);
    safePrint("\n");
    ++i2;
  }
}

}  // namespace diff_pipeline

REGISTER_COMMAND(
    diff, "diff", "compare files line by line",
    "Compare files line by line and report differences.\n"
    "\n"
    "This is a simplified implementation of the Unix diff utility.\n"
    "It supports basic comparison and unified diff output.",
    "  diff file1 file2         Compare two files\n"
    "  diff -q file1 file2      Only report if files differ\n"
    "  diff -u file1 file2      Show unified diff format",
    "cmp(1), patch(1)", "caomengxuan666", "Copyright © 2026 WinuxCmd",
    DIFF_OPTIONS) {
  using namespace diff_pipeline;

  // -- Boolean flags [DIFFERS] --
  bool brief = ctx.get<bool>("-q", false) || ctx.get<bool>("--brief", false);
  bool ignore_all_space =
      ctx.get<bool>("-w", false) || ctx.get<bool>("--ignore-all-space", false);
  bool ignore_blank_lines = ctx.has("-B") || ctx.has("--ignore-blank-lines");
  bool ignore_space_change = ctx.has("-b") || ctx.has("--ignore-space-change");
  bool ignore_case = ctx.has("-i") || ctx.has("--ignore-case");
  bool text_mode = ctx.has("-a") || ctx.has("--text");
  bool binary_mode = ctx.has("--binary");
  bool color_output = ctx.has("--color");
  bool strip_trailing_cr = ctx.has("--strip-trailing-cr") || ctx.has("-Z");
  bool report_identical = ctx.has("--report-identical-files") || ctx.has("-s");
  bool new_file = ctx.has("-N") || ctx.has("--new-file");
  bool suppress_common = ctx.has("--suppress-common-lines");
  bool suppress_blank_empty = ctx.has("--suppress-blank-empty");
  bool expand_tabs_opt = ctx.has("-t") || ctx.has("--expand-tabs");
  bool initial_tab = ctx.has("-T") || ctx.has("--initial-tab");
  bool no_dereference = ctx.has("--no-dereference");
  bool ignore_file_name_case = ctx.has("--ignore-file-name-case");
  bool unidirectional_new_file = ctx.has("--unidirectional-new-file");

  // -- Integer/width options [DIFFERS] --
  int tabsize = 8;
  if (ctx.has("--tabsize")) tabsize = ctx.get<int>("--tabsize", 8);
  int side_width = 130;
  if (ctx.has("-W")) side_width = ctx.get<int>("-W", 130);
  if (ctx.has("--width")) side_width = ctx.get<int>("--width", 130);

  // -- String options [DIFFERS] --
  std::string diff_program = ctx.get<std::string>("--diff-program", "");
  std::string exclude_pattern = ctx.get<std::string>("--exclude", "");
  std::string exclude_from = ctx.get<std::string>("--exclude-from", "");
  std::string exclude_dir_pattern = ctx.get<std::string>("--exclude-dir", "");
  std::string starting_file_str = ctx.get<std::string>("--starting-file", "");
  std::string ignore_matching =
      ctx.get<std::string>("--ignore-matching-lines", "");
  std::string show_function_line =
      ctx.get<std::string>("--show-function-line", "");

  // -- Reject options not implemented on Windows [DIFFERS] --
  if (ctx.has("-d") || ctx.has("--minimal")) {
    safeErrorPrintLn("diff: --minimal is not supported on Windows");
    return 1;
  }
  if (ctx.has("-e") || ctx.has("--ed")) {
    safeErrorPrintLn("diff: --ed is not supported on Windows");
    return 1;
  }
  if (ctx.has("-f") || ctx.has("--forward-ed")) {
    safeErrorPrintLn("diff: --forward-ed is not supported on Windows");
    return 1;
  }
  if (ctx.has("-n") || ctx.has("--rcs")) {
    safeErrorPrintLn("diff: --rcs is not supported on Windows");
    return 1;
  }
  if (ctx.has("-l") || ctx.has("--paginate")) {
    safeErrorPrintLn("diff: --paginate is not supported on Windows");
    return 1;
  }
  if (ctx.has("-p") || ctx.has("--show-c-function")) {
    safeErrorPrintLn("diff: --show-c-function is not supported on Windows");
    return 1;
  }
  if (!diff_program.empty()) {
    safeErrorPrintLn("diff: --diff-program is not supported on Windows");
    return 1;
  }
  if (ctx.has("-r") || ctx.has("--recursive")) {
    safeErrorPrintLn("diff: --recursive is not supported on Windows");
    return 1;
  }
  if (!exclude_pattern.empty() || ctx.has("-x")) {
    safeErrorPrintLn("diff: --exclude is not supported on Windows");
    return 1;
  }
  if (!exclude_from.empty() || ctx.has("-X")) {
    safeErrorPrintLn("diff: --exclude-from is not supported on Windows");
    return 1;
  }
  if (!exclude_dir_pattern.empty()) {
    safeErrorPrintLn("diff: --exclude-dir is not supported on Windows");
    return 1;
  }
  if (!starting_file_str.empty()) {
    safeErrorPrintLn("diff: --starting-file is not supported on Windows");
    return 1;
  }
  if (!show_function_line.empty()) {
    safeErrorPrintLn("diff: --show-function-line is not supported on Windows");
    return 1;
  }

  enum class OutputMode { Normal, Unified, Context, SideBySide };
  OutputMode output_mode = OutputMode::Normal;
  int context = 3;

  auto set_context = [&](int value, std::string_view option,
                         bool optional_value) -> bool {
    if (value < 0) {
      if (optional_value && value == -1) {
        context = 3;
        return true;
      }
      safeErrorPrint("diff: invalid context length '");
      safeErrorPrint(std::to_string(value));
      safeErrorPrint("'\n");
      safeErrorPrint("Try 'diff --help' for more information.\n");
      return false;
    }
    context = value;
    return true;
  };

  for (const auto &occurrence : ctx.options.occurrences()) {
    if (!ctx.metas || occurrence.index >= ctx.metas->size()) continue;
    const auto &meta = (*ctx.metas)[occurrence.index];
    if (meta.short_name == "-u" || meta.long_name == "--unified") {
      output_mode = OutputMode::Unified;
      auto value = std::get_if<int>(&occurrence.value);
      if (value && !set_context(*value, "--unified", true)) return 1;
    } else if (meta.short_name == "-U") {
      output_mode = OutputMode::Unified;
      auto value = std::get_if<int>(&occurrence.value);
      if (value && !set_context(*value, "-U", false)) return 1;
    } else if (meta.short_name == "-c" || meta.long_name == "--context") {
      output_mode = OutputMode::Context;
      auto value = std::get_if<int>(&occurrence.value);
      if (value && !set_context(*value, "--context", true)) return 1;
    } else if (meta.short_name == "-C") {
      output_mode = OutputMode::Context;
      auto value = std::get_if<int>(&occurrence.value);
      if (value && !set_context(*value, "-C", false)) return 1;
    } else if (meta.short_name == "-y" || meta.long_name == "--side-by-side") {
      output_mode = OutputMode::SideBySide;
    }
  }

  auto files_result = resolve_files(ctx);
  if (!files_result) {
    safeErrorPrint("diff: ");
    safeErrorPrintLn(files_result.error());
    safeErrorPrint("Try 'diff --help' for more information.\n");
    return 1;
  }

  std::string file1 = (*files_result)[0];
  std::string file2 = (*files_result)[1];

  DiffLabel label1{file1, false};
  DiffLabel label2{file2, false};
  auto labels = ctx.string_occurrences({"--label"});
  if (!labels.empty()) label1 = DiffLabel{labels[0].value, true};
  if (labels.size() > 1) label2 = DiffLabel{labels[1].value, true};

  if (brief) {
    auto result = compare_files(file1, file2, true, ignore_all_space);
    if (!result) {
      safeErrorPrint("diff: ");
      safeErrorPrint(result.error());
      safeErrorPrint("\n");
      return 1;
    }
    return result.value() ? 0 : 1;
  }

  // -- Read both files; -N/--new-file treats missing as empty [DIFFERS]
  auto lines1_result = read_file_lines_result(file1);
  if (!lines1_result) {
    if (new_file) {
      lines1_result = std::vector<std::string>{};
    } else {
      safeErrorPrint("diff: ");
      safeErrorPrint(lines1_result.error());
      safeErrorPrint("\n");
      return 1;
    }
  }

  auto lines2_result = read_file_lines_result(file2);
  if (!lines2_result) {
    if (new_file) {
      lines2_result = std::vector<std::string>{};
    } else {
      safeErrorPrint("diff: ");
      safeErrorPrint(lines2_result.error());
      safeErrorPrint("\n");
      return 1;
    }
  }

  auto &lines1 = lines1_result.value();
  auto &lines2 = lines2_result.value();

  // -- Apply transformations for comparison [DIFFERS] --
  auto cmp1 = lines1;
  auto cmp2 = lines2;

  if (strip_trailing_cr) {
    cmp1 = strip_trailing_cr_lines(cmp1);
    cmp2 = strip_trailing_cr_lines(cmp2);
  }
  if (ignore_blank_lines) {
    cmp1 = filter_blank_lines(cmp1);
    cmp2 = filter_blank_lines(cmp2);
  }
  if (expand_tabs_opt) {
    cmp1 = expand_tabs_lines(cmp1, tabsize);
    cmp2 = expand_tabs_lines(cmp2, tabsize);
  }
  cmp1 = normalize_lines_for_compare(cmp1, ignore_all_space);
  cmp2 = normalize_lines_for_compare(cmp2, ignore_all_space);
  if (ignore_space_change) {
    cmp1 = normalize_lines_space_change(cmp1);
    cmp2 = normalize_lines_space_change(cmp2);
  }
  if (ignore_case) {
    cmp1 = normalize_lines_case(cmp1);
    cmp2 = normalize_lines_case(cmp2);
  }
  if (!ignore_matching.empty()) {
    cmp1 = filter_matching_lines(cmp1, ignore_matching);
    cmp2 = filter_matching_lines(cmp2, ignore_matching);
  }
  auto &compare_lines1 = cmp1;
  auto &compare_lines2 = cmp2;

  if (compare_lines1 == compare_lines2) {
    if (report_identical) {
      safePrint("Files ");
      safePrint(file1);
      safePrint(" and ");
      safePrint(file2);
      safePrint(" are identical\n");
    }
    return 0;
  }

  if (output_mode == OutputMode::Unified) {
    output_unified_diff(file1, file2, compare_lines1, compare_lines2, lines1,
                        lines2, context, label1, label2);
  } else if (output_mode == OutputMode::Context) {
    output_context_diff(file1, file2, compare_lines1, compare_lines2, lines1,
                        lines2, context, label1, label2);
  } else if (output_mode == OutputMode::SideBySide) {
    // -- pass side_width and suppress_common [DIFFERS] --
    output_side_by_side(file1, file2, compare_lines1, compare_lines2, lines1,
                        lines2, side_width, suppress_common);
  } else {
    // Simple comparison using LCS
    auto edits = compute_diff(compare_lines1, compare_lines2);

    bool has_diffs = false;
    for (const auto &edit : edits) {
      if (edit.type == EditType::DEL) {
        safePrint("< ");
        safePrint(lines1[edit.line1_index]);
        safePrint("\n");
        has_diffs = true;
      } else if (edit.type == EditType::INS) {
        safePrint("> ");
        safePrint(lines2[edit.line2_index]);
        safePrint("\n");
        has_diffs = true;
      }
    }

    if (!has_diffs) {
      // Files are identical
      return 0;
    }
  }

  return 1;
}

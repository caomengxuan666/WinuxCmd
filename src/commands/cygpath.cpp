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
 *  - File: cygpath.cpp
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for cygpath command.
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

auto constexpr CYGPATH_OPTIONS = std::array{
    OPTION("-u", "--unix", "print Unix form of NAME"),
    OPTION("-w", "--windows", "print Windows form of NAME"),
    OPTION("-m", "--mixed", "print Windows form, with regular slashes"),
    OPTION("-p", "--path", "NAME is a PATH list"),
    OPTION("-t", "--type", "print TYPE form: dos, mixed, unix, or windows",
           STRING_TYPE),
    OPTION("-d", "--dos",
           "print DOS short form of NAMEs [accepted, not implemented]"),
    OPTION("-a", "--absolute",
           "output absolute path [accepted, partial lexical support]"),
    OPTION("-i", "--ignore", "ignore missing or empty arguments"),
    OPTION("-f", "--file", "read input paths from FILE [not implemented]",
           STRING_TYPE),
    OPTION("-o", "--option",
           "read options from FILE as well [accepted, not implemented]"),
    OPTION("-l", "--long-name",
           "print Windows long form [accepted, not implemented]"),
    OPTION("-s", "--short-name",
           "print DOS short form [accepted, not implemented]"),
    OPTION("-U", "--proc-cygdrive",
           "emit /proc/cygdrive paths [accepted, not implemented]"),
    OPTION("-C", "--codepage",
           "print Windows path in codepage CP [accepted, not implemented]",
           STRING_TYPE),
    OPTION("-M", "--mode", "report file text/binary mode [not implemented]"),
    OPTION("-A", "--allusers",
           "use All Users for special folders [not implemented]"),
    OPTION("-D", "--desktop", "output Desktop directory [not implemented]"),
    OPTION("-H", "--homeroot", "output Profiles directory [not implemented]"),
    OPTION("-O", "--mydocs", "output My Documents directory [not implemented]"),
    OPTION("-P", "--smprograms",
           "output Start Menu Programs directory [not implemented]"),
    OPTION("-S", "--sysdir", "output system directory [not implemented]"),
    OPTION("-W", "--windir", "output Windows directory [not implemented]"),
    OPTION("-F", "--folder", "output special folder ID [not implemented]",
           STRING_TYPE)};

// ======================================================
// Helper functions
// ======================================================

namespace {
enum class OutputMode { Unix, Windows, Mixed };

auto ascii_lower(char ch) -> char {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

auto ascii_upper(char ch) -> char {
  return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

auto is_drive_letter(char ch) -> bool {
  return std::isalpha(static_cast<unsigned char>(ch)) != 0;
}

auto has_windows_drive(std::string_view path) -> bool {
  return path.size() >= 2 && is_drive_letter(path[0]) && path[1] == ':';
}

auto replace_char(std::string value, char from, char to) -> std::string {
  std::ranges::replace(value, from, to);
  return value;
}

auto split_keep_empty(std::string_view value, char separator)
    -> std::vector<std::string> {
  std::vector<std::string> parts;
  size_t start = 0;
  while (start <= value.size()) {
    size_t pos = value.find(separator, start);
    if (pos == std::string_view::npos) {
      parts.emplace_back(value.substr(start));
      break;
    }
    parts.emplace_back(value.substr(start, pos - start));
    start = pos + 1;
  }
  return parts;
}

auto join_parts(const std::vector<std::string>& parts, char separator)
    -> std::string {
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i != 0) result.push_back(separator);
    result += parts[i];
  }
  return result;
}

auto remove_extended_prefix(std::string path) -> std::string {
  if (path.starts_with(R"(\\?\UNC\)")) {
    return R"(\\)" + path.substr(8);
  }
  if (path.starts_with(R"(\\?\)")) {
    return path.substr(4);
  }
  return path;
}

auto make_absolute_for_windows_input(std::string path) -> std::string {
  if (path.empty() || has_windows_drive(path) || path.starts_with("\\\\") ||
      path.starts_with("//")) {
    return path;
  }

  std::filesystem::path fs_path(utf8_to_wstring(path));
  std::error_code ec;
  auto absolute = std::filesystem::absolute(fs_path, ec);
  if (ec) return path;
  return wstring_to_utf8(absolute.wstring());
}

auto make_absolute_for_posix_input(std::string path) -> std::string {
  if (path.empty() || path.starts_with('/')) return path;
  return make_absolute_for_windows_input(std::move(path));
}

// Cygwin's source delegates conversion to cygwin_conv_path.  WinuxCmd does not
// link against the Cygwin runtime, so this mirrors the hot MSYS/Cygwin shapes:
// C:\x -> /c/x, /c/x -> C:\x, and UNC slashes are preserved.
auto windows_to_unix(std::string path) -> std::string {
  path = remove_extended_prefix(std::move(path));
  path = replace_char(std::move(path), '\\', '/');

  if (has_windows_drive(path)) {
    char drive = ascii_lower(path[0]);
    std::string tail = path.substr(2);
    if (!tail.empty() && tail.front() != '/') tail.insert(tail.begin(), '/');
    return "/" + std::string(1, drive) + tail;
  }

  return path;
}

auto unix_to_windows(std::string path, bool mixed) -> std::string {
  if (path.starts_with("/cygdrive/") && path.size() >= 11 &&
      is_drive_letter(path[10]) && (path.size() == 11 || path[11] == '/')) {
    std::string tail = path.substr(11);
    if (!tail.empty() && tail.front() != '/') tail.insert(tail.begin(), '/');
    path = std::string(1, ascii_upper(path[10])) + ":" + tail;
  } else if (path.size() >= 2 && path[0] == '/' && is_drive_letter(path[1]) &&
             (path.size() == 2 || path[2] == '/')) {
    std::string tail = path.substr(2);
    if (!tail.empty() && tail.front() != '/') tail.insert(tail.begin(), '/');
    path = std::string(1, ascii_upper(path[1])) + ":" + tail;
  }

  return mixed ? replace_char(std::move(path), '\\', '/')
               : replace_char(std::move(path), '/', '\\');
}

auto convert_one(std::string path, OutputMode mode, bool absolute)
    -> std::string {
  if (absolute) {
    path = (mode == OutputMode::Unix)
               ? make_absolute_for_windows_input(std::move(path))
               : make_absolute_for_posix_input(std::move(path));
  }

  switch (mode) {
    case OutputMode::Unix:
      return windows_to_unix(std::move(path));
    case OutputMode::Windows:
      return unix_to_windows(std::move(path), false);
    case OutputMode::Mixed:
      return unix_to_windows(std::move(path), true);
  }

  std::unreachable();
}

auto convert_path_list(std::string_view value, OutputMode mode, bool absolute)
    -> std::string {
  char input_separator = mode == OutputMode::Unix ? ';' : ':';
  char output_separator = mode == OutputMode::Unix ? ':' : ';';
  auto parts = split_keep_empty(value, input_separator);
  for (auto& part : parts) {
    if (!part.empty()) part = convert_one(std::move(part), mode, absolute);
  }
  return join_parts(parts, output_separator);
}

auto unsupported_options(const CommandContext<CYGPATH_OPTIONS.size()>& ctx)
    -> std::vector<std::string_view> {
  std::vector<std::string_view> names;
  for (const auto& occurrence : ctx.options.occurrences()) {
    const auto& meta = (*ctx.metas)[occurrence.index];
    auto short_name = meta.short_name;
    auto long_name = meta.long_name;

    if (short_name == "-d" || short_name == "-f" || short_name == "-o" ||
        short_name == "-l" || short_name == "-s" || short_name == "-U" ||
        short_name == "-C" || short_name == "-M" || short_name == "-A" ||
        short_name == "-D" || short_name == "-H" || short_name == "-O" ||
        short_name == "-P" || short_name == "-S" || short_name == "-W" ||
        short_name == "-F") {
      names.push_back(!long_name.empty() ? long_name : short_name);
    }
  }
  return names;
}
}  // namespace

// ======================================================
// Pipeline components
// ======================================================
namespace cygpath_pipeline {
namespace cp = core::pipeline;

struct Config {
  OutputMode mode = OutputMode::Unix;
  bool absolute = false;
  bool ignore = false;
  bool is_path = false;
  std::vector<std::string> paths;
};

auto build_config(const CommandContext<CYGPATH_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;

  auto unsupported = unsupported_options(ctx);
  if (!unsupported.empty()) {
    return std::unexpected("option '" + std::string(unsupported.front()) +
                           "' is accepted for compatibility but not "
                           "implemented yet");
  }

  size_t mode_count = 0;
  if (ctx.get<bool>("-u", false) || ctx.get<bool>("--unix", false)) {
    cfg.mode = OutputMode::Unix;
    ++mode_count;
  }
  if (ctx.get<bool>("-w", false) || ctx.get<bool>("--windows", false)) {
    cfg.mode = OutputMode::Windows;
    ++mode_count;
  }
  if (ctx.get<bool>("-m", false) || ctx.get<bool>("--mixed", false)) {
    cfg.mode = OutputMode::Mixed;
    ++mode_count;
  }

  std::string type = ctx.get<std::string>("-t", "");
  if (type.empty()) type = ctx.get<std::string>("--type", "");
  if (!type.empty()) {
    std::ranges::transform(type, type.begin(), ascii_lower);
    if (type == "unix") {
      cfg.mode = OutputMode::Unix;
    } else if (type == "windows") {
      cfg.mode = OutputMode::Windows;
    } else if (type == "mixed") {
      cfg.mode = OutputMode::Mixed;
    } else if (type == "dos") {
      return std::unexpected(
          "type 'dos' is accepted for compatibility but not implemented yet");
    } else {
      return std::unexpected("invalid type '" + type +
                             "'; expected dos, mixed, unix, or windows");
    }
    ++mode_count;
  }

  if (mode_count > 1) {
    return std::unexpected(
        "only one output type may be specified (-u, -w, -m, or -t)");
  }

  cfg.absolute =
      ctx.get<bool>("-a", false) || ctx.get<bool>("--absolute", false);
  cfg.ignore = ctx.get<bool>("-i", false) || ctx.get<bool>("--ignore", false);
  cfg.is_path = ctx.get<bool>("-p", false) || ctx.get<bool>("--path", false);

  if (ctx.positionals.empty()) {
    if (cfg.ignore) return cfg;
    return std::unexpected("missing operand");
  }

  for (const auto& path : ctx.positionals) {
    cfg.paths.push_back(std::string(path));
  }

  return cfg;
}

auto run(const Config& cfg) -> int {
  for (const auto& path : cfg.paths) {
    if (path.empty()) {
      if (cfg.ignore) continue;
      safeErrorPrintLn("cygpath: can't convert empty path");
      return 1;
    }

    auto result = cfg.is_path ? convert_path_list(path, cfg.mode, cfg.absolute)
                              : convert_one(path, cfg.mode, cfg.absolute);
    safePrintLn(result);
  }

  return 0;
}

}  // namespace cygpath_pipeline

// ======================================================
// Main command implementation
// ======================================================

REGISTER_COMMAND(cygpath,
                 /* name */
                 "cygpath",

                 /* synopsis */
                 "cygpath [OPTION] NAME...",

                 /* description */
                 "Convert path names between Windows and Unix formats.\n"
                 "Convert path names between Windows and Unix/Cygwin formats.\n"
                 "Default is to convert Windows paths to Unix paths.",

                 /* examples */
                 "  cygpath -u C:\\Users\\John\\Documents\n"
                 "  cygpath -w /home/user/file.txt\n"
                 "  cygpath -m /c/Users/John/Documents",

                 /* see_also */
                 "realpath(1), pwd(1)",

                 /* author */
                 "WinuxCmd",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 CYGPATH_OPTIONS) {
  using namespace cygpath_pipeline;
  using namespace core::pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    safeErrorPrint("cygpath: ");
    safeErrorPrintLn(cfg_result.error());
    return 1;
  }

  return run(*cfg_result);
}

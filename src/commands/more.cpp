/*
 *  Copyright © 2026 [caomengxuan666]
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
 *  - File: more.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for more.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
// include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
import container;

using cmd::meta::option_matches;
using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr MORE_OPTIONS = std::array{
    OPTION("-d", "--silent", "display help instead of ring bell", BOOL_TYPE),
    OPTION("-f", "--logical", "count logical lines, not screen lines",
           BOOL_TYPE),
    OPTION("-l", "--no-pause", "suppress pause after form feeds", BOOL_TYPE),
    OPTION("-c", "--print-over", "clear line ends before each page", BOOL_TYPE),
    OPTION("-p", "--clean-print",
           "do not scroll, clean screen and display text", BOOL_TYPE),
    OPTION("-e", "--exit-on-eof", "exit on end-of-file", BOOL_TYPE),
    OPTION("-s", "--squeeze", "squeeze multiple blank lines", BOOL_TYPE),
    OPTION("-u", "--plain",
           "accepted placeholder; underlining and bold are already plain",
           BOOL_TYPE),
    OPTION("-n", "--lines", "number of lines per screenful", INT_TYPE),
    OPTION("-NUM", "", "same as -n NUM", INT_TYPE)};

namespace more_pipeline {
namespace cp = core::pipeline;

struct Config {
  bool help_prompt = false;
  bool logical_lines = false;
  bool pause_form_feed = false;
  bool clear_screen = false;
  bool no_scroll = false;
  bool exit_on_eof = false;
  bool squeeze_blank = false;
  bool plain = false;
  size_t lines_per_page = 24;
  size_t start_line = 0;
  std::string search_pattern;
  SmallVector<std::string, 64> files;
};

auto build_config(const CommandContext<MORE_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;

  cfg.help_prompt =
      ctx.get<bool>("--silent", false) || ctx.get<bool>("-d", false);
  cfg.logical_lines =
      ctx.get<bool>("--logical", false) || ctx.get<bool>("-f", false);
  cfg.pause_form_feed =
      ctx.get<bool>("--no-pause", false) || ctx.get<bool>("-l", false);
  cfg.clear_screen =
      ctx.get<bool>("--print-over", false) || ctx.get<bool>("-c", false);
  cfg.no_scroll =
      ctx.get<bool>("--clean-print", false) || ctx.get<bool>("-p", false);
  cfg.exit_on_eof =
      ctx.get<bool>("--exit-on-eof", false) || ctx.get<bool>("-e", false);
  cfg.squeeze_blank =
      ctx.get<bool>("--squeeze", false) || ctx.get<bool>("-s", false);
  cfg.plain = ctx.get<bool>("--plain", false) || ctx.get<bool>("-u", false);

  for (const auto& occurrence : ctx.options.occurrences()) {
    if (!ctx.metas || occurrence.index >= MORE_OPTIONS.size()) continue;
    const auto& meta = (*ctx.metas)[occurrence.index];
    const auto* value = std::get_if<int>(&occurrence.value);
    if (!value) continue;
    if (option_matches(meta, "-n", "--lines") ||
        option_matches(meta, "-NUM", "")) {
      if (*value < 1) return std::unexpected("invalid line count");
      cfg.lines_per_page = static_cast<size_t>(*value);
    }
  }

  for (const auto& pos : ctx.positionals) {
    std::string arg(pos);
    if (arg.size() > 1 && arg[0] == '+') {
      if (arg[1] == '/') {
        cfg.search_pattern = arg.substr(2);
        continue;
      }
      bool digits_only = std::ranges::all_of(
          arg.begin() + 1, arg.end(),
          [](unsigned char ch) { return std::isdigit(ch) != 0; });
      if (digits_only) {
        try {
          unsigned long long line = std::stoull(arg.substr(1));
          cfg.start_line = line > 0 ? static_cast<size_t>(line - 1) : 0;
          continue;
        } catch (...) {
          return std::unexpected("invalid line number");
        }
      }
    }
    cfg.files.push_back(std::move(arg));
  }

  return cfg;
}

auto get_console_height() -> size_t {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return static_cast<size_t>(csbi.srWindow.Bottom - csbi.srWindow.Top);
  }
  return 24;  // Default
}

auto clear_console() -> void {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
  DWORD written;
  FillConsoleOutputCharacter(hConsole, ' ', cells, {0, 0}, &written);
  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cells, {0, 0},
                             &written);
  SetConsoleCursorPosition(hConsole, {0, 0});
}

auto copy_stream_to_stdout(std::istream& input) -> int {
  std::array<char, 64 * 1024> buffer{};
  while (input) {
    input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    auto count = input.gcount();
    if (count > 0) {
      safePrint(std::string_view(buffer.data(), static_cast<size_t>(count)));
    }
  }

  return input.bad() ? 1 : 0;
}

auto copy_file_to_stdout(const std::string& filename) -> int {
  if (filename.empty() || filename == "-") {
    return copy_stream_to_stdout(std::cin);
  }
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    safeErrorPrint("more: ");
    safeErrorPrint(filename);
    safeErrorPrintLn(": No such file or directory");
    return 1;
  }
  return copy_stream_to_stdout(file);
}
auto read_display_lines(const std::string& filename)
    -> cp::Result<std::vector<std::string>> {
  std::vector<std::string> lines;
  if (filename.empty() || filename == "-") {
    std::string line;
    while (std::getline(std::cin, line)) {
      if (!line.empty() && line.back() == '') line.pop_back();
      lines.push_back(line);
    }
  } else {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      return std::unexpected(std::string("cannot open '") + filename +
                             "' for reading");
    }
    std::string line;
    while (std::getline(file, line)) {
      if (!line.empty() && line.back() == '') line.pop_back();
      lines.push_back(line);
    }
  }
  return lines;
}
auto prepare_start_line(std::vector<std::string>& lines, const Config& cfg)
    -> size_t {
  if (cfg.squeeze_blank) {
    std::vector<std::string> squeezed;
    bool prev_blank = false;
    for (const auto& line : lines) {
      bool is_blank = line.empty();
      if (!is_blank || !prev_blank) {
        squeezed.push_back(line);
      }
      prev_blank = is_blank;
    }
    lines = std::move(squeezed);
  }

  size_t start_line = std::min(cfg.start_line, lines.size());
  if (!cfg.search_pattern.empty()) {
    for (size_t i = start_line; i < lines.size(); ++i) {
      if (lines[i].find(cfg.search_pattern) != std::string::npos) {
        return i;
      }
    }
  }
  return start_line;
}
auto display_file_noninteractive(const std::string& filename, const Config& cfg)
    -> int {
  auto lines_result = read_display_lines(filename);
  if (!lines_result) {
    cp::report_error(lines_result, L"more");
    return 1;
  }
  auto lines = std::move(*lines_result);
  size_t start_line = prepare_start_line(lines, cfg);
  for (size_t i = start_line; i < lines.size(); ++i) {
    safePrintLn(lines[i]);
  }
  return 0;
}
auto display_file(const std::string& filename, const Config& cfg) -> int {
  auto lines_result = read_display_lines(filename);
  if (!lines_result) {
    cp::report_error(lines_result, L"more");
    return 1;
  }
  auto lines = std::move(*lines_result);
  size_t start_line = prepare_start_line(lines, cfg);

  // Get terminal height
  size_t page_height = cfg.lines_per_page;
  if (page_height == 0) {
    page_height = get_console_height();
  }

  // Display pages
  size_t current_line = start_line;
  bool done = false;

  while (!done && current_line < lines.size()) {
    // Clear screen if requested
    if (cfg.clear_screen || cfg.no_scroll) {
      clear_console();
    }

    // Display one page
    size_t lines_shown = 0;
    for (size_t i = 0; i < page_height && current_line < lines.size();
         ++i, ++current_line) {
      safePrintLn(lines[current_line]);
      lines_shown++;
    }

    // Check if we're at the end
    if (current_line >= lines.size()) {
      break;
    }

    // Prompt for more
    if (cfg.help_prompt) {
      safePrint("--More--(%) [Press space to continue, 'q' to quit]");
    } else {
      safePrint("--More--");
    }

    // Wait for input
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

    INPUT_RECORD record;
    DWORD read;
    while (true) {
      ReadConsoleInput(hInput, &record, 1, &read);
      if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
        char ch = record.Event.KeyEvent.uChar.AsciiChar;
        if (ch == 'q' || ch == 'Q') {
          done = true;
          break;
        } else if (ch == ' ' || ch == '\n' || ch == '\r') {
          break;
        }
      }
    }

    SetConsoleMode(hInput, mode);
    safePrintLn("");
  }

  return 0;
}

auto run(const Config& cfg) -> int {
  if (!isOutputConsole()) {
    bool needs_line_processing =
        cfg.squeeze_blank || cfg.start_line > 0 || !cfg.search_pattern.empty();
    if (cfg.files.empty()) {
      return needs_line_processing ? display_file_noninteractive("", cfg)
                                   : copy_file_to_stdout("");
    }
    int result = 0;
    for (const auto& file : cfg.files) {
      int file_result = needs_line_processing
                            ? display_file_noninteractive(file, cfg)
                            : copy_file_to_stdout(file);
      if (file_result != 0) {
        result = 1;
      }
    }
    return result;
  }
  if (cfg.files.empty()) {
    return display_file("", cfg);
  }

  int result = 0;
  for (const auto& file : cfg.files) {
    if (display_file(file, cfg) != 0) {
      result = 1;
    }
  }
  return result;
}

}  // namespace more_pipeline

REGISTER_COMMAND(
    more, "more", "more [OPTION]... [FILE]...",
    "Display the contents of a file, one screenful at a time.\n"
    "\n"
    "The more command is a filter for paging through text one screen at a "
    "time.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "\n"
    "  -d, --silent       display help instead of ring bell\n"
    "  -f, --logical      count logical lines, not screen lines\n"
    "  -l, --no-pause     suppress pause after form feeds\n"
    "  -c, --print-over   clear line ends before each page\n"
    "  -p, --clean-print  do not scroll, clean screen and display text\n"
    "  -e, --exit-on-eof  exit on end-of-file\n"
    "  -s, --squeeze      squeeze multiple blank lines\n"
    "  -u, --plain        suppress underlining and bold; currently plain "
    "output\n"
    "  -n, --lines NUM    number of lines per screenful\n"
    "  -NUM              same as -n NUM\n"
    "  +NUM              display file beginning from line number\n"
    "  +/PATTERN         display file beginning from pattern match\n"
    "\n"
    "Interactive commands:\n"
    "  SPACE       display next screenful\n"
    "  ENTER       display next line\n"
    "  q           quit\n"
    "\n"
    "Note: On Windows, more uses the console for interactive paging.",
    "  more file.txt         display file one page at a time\n"
    "  more -s file.txt      squeeze blank lines\n"
    "  more -n 40 file.txt   use 40 lines per page\n"
    "  cat file | more       page through piped input",
    "less(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd", MORE_OPTIONS) {
  using namespace more_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"more");
    return 1;
  }

  return run(*cfg_result);
}

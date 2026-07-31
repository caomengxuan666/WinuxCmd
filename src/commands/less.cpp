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
 *  - File: less.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for less.
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

auto constexpr LESS_OPTIONS = std::array{
    OPTION("-e", "--quit-at-eof", "quit after second EOF", BOOL_TYPE),
    OPTION("-E", "--QUIT-AT-EOF", "quit after first EOF", BOOL_TYPE),
    OPTION("-F", "--quit-if-one-screen",
           "quit if entire file fits on first screen", BOOL_TYPE),
    OPTION("-i", "--ignore-case",
           "ignore case in searches that do not contain uppercase", BOOL_TYPE),
    OPTION("-I", "--IGNORE-CASE", "ignore case in all searches", BOOL_TYPE),
    OPTION("-n", "--line-numbers", "display line number at start of each line",
           BOOL_TYPE),
    OPTION("-N", "--LINE-NUMBERS", "display line number at start of each line",
           BOOL_TYPE),
    OPTION("-S", "--chop-long-lines", "chop long lines (do not wrap)",
           BOOL_TYPE),
    OPTION("-q", "--quiet", "accepted placeholder; terminal bell is not used",
           BOOL_TYPE),
    OPTION("-Q", "--QUIET", "accepted placeholder; terminal bell is not used",
           BOOL_TYPE),
    OPTION("-r", "--raw-control-chars",
           "accepted placeholder; control chars are passed through", BOOL_TYPE),
    OPTION("-R", "--RAW-CONTROL-CHARS",
           "accepted placeholder; ANSI color parsing is not implemented",
           BOOL_TYPE),
    OPTION("-z", "--window", "set scrolling window size", INT_TYPE),
    OPTION("-NUM", "", "same as -z NUM", INT_TYPE)};

namespace less_pipeline {
namespace cp = core::pipeline;

struct Config {
  bool quit_at_eof = false;        // -e
  bool quit_first_eof = false;     // -E
  bool quit_one_screen = false;    // -F
  bool ignore_case = false;        // -i
  bool ignore_case_all = false;    // -I
  bool show_line_numbers = false;  // -n, -N
  bool chop_long_lines = false;    // -S
  bool quiet = false;              // -q accepted; pager does not ring a bell.
  bool never_bell = false;         // -Q accepted; pager does not ring a bell.
  bool raw_control = false;        // -r accepted; pass-through output mode.
  bool raw_control_color = false;  // -R accepted; ANSI parsing not implemented.
  int window_size = -1;            // -z, -NUM
  SmallVector<std::string, 16> files;
};

auto build_config(const CommandContext<LESS_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  cfg.quit_at_eof =
      ctx.get<bool>("--quit-at-eof", false) || ctx.get<bool>("-e", false);
  cfg.quit_first_eof =
      ctx.get<bool>("--QUIT-AT-EOF", false) || ctx.get<bool>("-E", false);
  cfg.quit_one_screen = ctx.get<bool>("--quit-if-one-screen", false) ||
                        ctx.get<bool>("-F", false);
  cfg.ignore_case =
      ctx.get<bool>("--ignore-case", false) || ctx.get<bool>("-i", false);
  cfg.ignore_case_all =
      ctx.get<bool>("--IGNORE-CASE", false) || ctx.get<bool>("-I", false);
  cfg.show_line_numbers =
      ctx.get<bool>("--line-numbers", false) || ctx.get<bool>("-n", false) ||
      ctx.get<bool>("--LINE-NUMBERS", false) || ctx.get<bool>("-N", false);
  cfg.chop_long_lines =
      ctx.get<bool>("--chop-long-lines", false) || ctx.get<bool>("-S", false);
  cfg.quiet = ctx.get<bool>("--quiet", false) || ctx.get<bool>("-q", false);
  cfg.never_bell =
      ctx.get<bool>("--QUIET", false) || ctx.get<bool>("-Q", false);
  cfg.raw_control =
      ctx.get<bool>("--raw-control-chars", false) || ctx.get<bool>("-r", false);
  cfg.raw_control_color =
      ctx.get<bool>("--RAW-CONTROL-CHARS", false) || ctx.get<bool>("-R", false);

  for (const auto& occurrence : ctx.options.occurrences()) {
    if (!ctx.metas || occurrence.index >= LESS_OPTIONS.size()) continue;
    const auto& meta = (*ctx.metas)[occurrence.index];
    const auto* value = std::get_if<int>(&occurrence.value);
    if (!value) continue;
    if (option_matches(meta, "-z", "--window") ||
        option_matches(meta, "-NUM", "")) {
      cfg.window_size = *value;
    }
  }

  for (auto arg : ctx.positionals) {
    std::string file_arg(arg);
    if (contains_wildcard(file_arg)) {
      auto glob_result = glob_expand(file_arg);
      if (glob_result.expanded) {
        for (const auto& file : glob_result.files) {
          cfg.files.push_back(wstring_to_utf8(file));
        }
        continue;
      }
    }
    cfg.files.push_back(file_arg);
  }

  if (cfg.files.empty()) {
    cfg.files.push_back("-");
  }

  return cfg;
}

auto read_file_content(const std::string& filename) -> cp::Result<std::string> {
  std::string content;

  if (filename == "-" || filename.empty()) {
    // Read from stdin
    content.assign(std::istreambuf_iterator<char>(std::cin),
                   std::istreambuf_iterator<char>());
    if (std::cin.fail() && !std::cin.eof()) {
      return std::unexpected("error reading from standard input");
    }
  } else {
    // Read from file
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      return std::unexpected(std::string("cannot open '") + filename +
                             "' for reading");
    }
    content.assign(std::istreambuf_iterator<char>(file),
                   std::istreambuf_iterator<char>());
    if (file.fail() && !file.eof()) {
      return std::unexpected("error reading from file");
    }
  }

  return content;
}

auto split_lines(std::string_view content) -> SmallVector<std::string, 4096> {
  SmallVector<std::string, 4096> lines;
  size_t start = 0;
  while (start < content.size()) {
    size_t end = content.find('\n', start);
    if (end == std::string_view::npos) {
      lines.push_back(std::string(content.substr(start)));
      break;
    }
    std::string_view line = content.substr(start, end - start);
    if (!line.empty() && line.back() == '\r') line.remove_suffix(1);
    lines.push_back(std::string(line));
    start = end + 1;
  }
  if (lines.empty()) lines.push_back("");
  return lines;
}

auto console_size() -> std::pair<int, int> {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return {std::max(width, 1), std::max(height, 2)};
  }
  return {80, 24};
}

auto clear_console() -> void {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;

  DWORD cells = static_cast<DWORD>(csbi.dwSize.X) * csbi.dwSize.Y;
  DWORD written = 0;
  FillConsoleOutputCharacterA(hConsole, ' ', cells, {0, 0}, &written);
  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cells, {0, 0},
                             &written);
  SetConsoleCursorPosition(hConsole, {0, 0});
}

class ConsoleInputMode {
 public:
  ConsoleInputMode() : input_(GetStdHandle(STD_INPUT_HANDLE)) {
    if (input_ == INVALID_HANDLE_VALUE || input_ == nullptr) return;
    if (!GetConsoleMode(input_, &original_mode_)) return;

    DWORD raw_mode = original_mode_ & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    if (SetConsoleMode(input_, raw_mode)) active_ = true;
  }

  ~ConsoleInputMode() {
    if (active_) SetConsoleMode(input_, original_mode_);
  }

  [[nodiscard]] auto active() const -> bool { return active_; }
  [[nodiscard]] auto input() const -> HANDLE { return input_; }

 private:
  HANDLE input_ = nullptr;
  DWORD original_mode_ = 0;
  bool active_ = false;
};

enum class PagerAction {
  None,
  NextPage,
  NextLine,
  PrevPage,
  PrevLine,
  SearchForward,
  SearchBackward,
  RepeatSearch,
  ReverseSearch,
  Quit
};

struct PagerCommand {
  PagerAction action = PagerAction::None;
  std::string text;
};

struct SearchState {
  std::string text;
  portable_regex::Pattern pattern;
  bool forward = true;
};

auto read_search_text(HANDLE input, wchar_t prompt)
    -> std::optional<std::string> {
  std::wstring text;
  safePrint(std::wstring_view(&prompt, 1));

  INPUT_RECORD record{};
  DWORD read = 0;
  while (ReadConsoleInputW(input, &record, 1, &read)) {
    if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) {
      continue;
    }

    const auto& key = record.Event.KeyEvent;
    if (key.wVirtualKeyCode == VK_ESCAPE) return std::nullopt;
    if (key.wVirtualKeyCode == VK_RETURN) {
      safePrint("\n");
      return wstring_to_utf8(text);
    }
    if (key.wVirtualKeyCode == VK_BACK) {
      if (!text.empty()) {
        text.pop_back();
        safePrint("\b \b");
      }
      continue;
    }

    wchar_t ch = key.uChar.UnicodeChar;
    if (ch >= L' ') {
      text.push_back(ch);
      safePrint(std::wstring_view(&ch, 1));
    }
  }
  return std::nullopt;
}

auto read_pager_command(HANDLE input) -> PagerCommand {
  INPUT_RECORD record{};
  DWORD read = 0;
  while (ReadConsoleInputW(input, &record, 1, &read)) {
    if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) {
      continue;
    }

    const auto& key = record.Event.KeyEvent;
    char ch = key.uChar.AsciiChar;
    if (ch == 'q' || ch == 'Q') return {PagerAction::Quit, {}};
    if (ch == 'b' || ch == 'B') return {PagerAction::PrevPage, {}};
    if (ch == ' ') return {PagerAction::NextPage, {}};
    if (ch == '\r' || ch == '\n') return {PagerAction::NextLine, {}};
    if (ch == 'n') return {PagerAction::RepeatSearch, {}};
    if (ch == 'N') return {PagerAction::ReverseSearch, {}};
    if (ch == '/') {
      auto text = read_search_text(input, L'/');
      return text ? PagerCommand{PagerAction::SearchForward, *text}
                  : PagerCommand{PagerAction::None, {}};
    }
    if (ch == '?') {
      auto text = read_search_text(input, L'?');
      return text ? PagerCommand{PagerAction::SearchBackward, *text}
                  : PagerCommand{PagerAction::None, {}};
    }

    switch (key.wVirtualKeyCode) {
      case VK_NEXT:
        return {PagerAction::NextPage, {}};
      case VK_PRIOR:
        return {PagerAction::PrevPage, {}};
      case VK_DOWN:
        return {PagerAction::NextLine, {}};
      case VK_UP:
        return {PagerAction::PrevLine, {}};
      case VK_ESCAPE:
        return {PagerAction::Quit, {}};
      default:
        return {PagerAction::None, {}};
    }
  }
  return {PagerAction::Quit, {}};
}

auto compile_search(const Config& cfg, std::string_view text, bool forward)
    -> std::optional<SearchState> {
  if (text.empty()) return std::nullopt;
  bool has_upper = std::ranges::any_of(
      text, [](unsigned char ch) { return std::isupper(ch) != 0; });
  bool ignore_case = cfg.ignore_case_all || (cfg.ignore_case && !has_upper);
  auto compiled = portable_regex::compile(portable_regex::Syntax::Extended,
                                          text, ignore_case);
  if (!compiled) return std::nullopt;
  return SearchState{std::string(text), std::move(compiled.pattern), forward};
}

auto find_search_match(const SmallVector<std::string, 4096>& lines,
                       const SearchState& search, size_t top_line, bool forward)
    -> std::optional<size_t> {
  if (lines.empty()) return std::nullopt;

  if (forward) {
    size_t start = std::min(top_line + 1, lines.size());
    for (size_t i = start; i < lines.size(); ++i) {
      if (search.pattern.find_first(lines[i]).has_value()) return i;
    }
    for (size_t i = 0; i < start; ++i) {
      if (search.pattern.find_first(lines[i]).has_value()) return i;
    }
  } else {
    size_t start = top_line == 0 ? lines.size() - 1 : top_line - 1;
    for (size_t i = start + 1; i-- > 0;) {
      if (search.pattern.find_first(lines[i]).has_value()) return i;
      if (i == 0) break;
    }
    for (size_t i = lines.size(); i-- > start + 1;) {
      if (search.pattern.find_first(lines[i]).has_value()) return i;
    }
  }
  return std::nullopt;
}

auto print_pager_line(const Config& cfg, size_t line_index,
                      std::string_view line, int width) -> void {
  std::string rendered;
  if (cfg.show_line_numbers) {
    rendered += std::format("{:>6}\t", line_index + 1);
  }
  rendered.append(line);

  if (cfg.chop_long_lines && width > 0 &&
      rendered.size() > static_cast<size_t>(width)) {
    rendered.resize(static_cast<size_t>(width));
  }

  safePrintLn(rendered);
}

auto render_page(const Config& cfg, const SmallVector<std::string, 4096>& lines,
                 size_t top_line, int page_size, int width) -> void {
  clear_console();
  for (int i = 0; i < page_size; ++i) {
    size_t line_index = top_line + static_cast<size_t>(i);
    if (line_index >= lines.size()) {
      safePrintLn("");
      continue;
    }
    print_pager_line(cfg, line_index, lines[line_index], width);
  }
}

auto print_prompt(size_t top_line, size_t total_lines, bool at_eof) -> void {
  safePrint(":");
  if (at_eof) {
    safePrint("(END) ");
  }
  safePrint(std::to_string(top_line + 1));
  safePrint("/");
  safePrint(std::to_string(total_lines));
  safePrint("  SPACE:next  b:back  /:search  n:next  q:quit");
}

// Interactive pager. Non-terminal output still behaves like cat, matching less.
auto simple_pager(const Config& cfg, const std::string& content) -> int {
  if (!isOutputConsole()) {
    safePrint(content);
    return 0;
  }

  auto lines = split_lines(content);
  auto [width, height] = console_size();
  int page_size =
      cfg.window_size > 0 ? cfg.window_size : std::max(height - 1, 1);

  if (cfg.quit_one_screen && lines.size() <= static_cast<size_t>(page_size)) {
    for (size_t i = 0; i < lines.size(); ++i) {
      print_pager_line(cfg, i, lines[i], width);
    }
    return 0;
  }

  ConsoleInputMode input_mode;
  if (!input_mode.active()) {
    safePrint(content);
    return 0;
  }

  size_t top_line = 0;
  int eof_count = 0;
  std::optional<SearchState> last_search;

  while (true) {
    bool at_eof = top_line + static_cast<size_t>(page_size) >= lines.size();
    render_page(cfg, lines, top_line, page_size, width);
    print_prompt(top_line, lines.size(), at_eof);

    if (at_eof && cfg.quit_first_eof) return 0;

    PagerCommand command = read_pager_command(input_mode.input());
    PagerAction action = command.action;
    if (action == PagerAction::Quit) return 0;

    const size_t max_top = lines.size() > static_cast<size_t>(page_size)
                               ? lines.size() - static_cast<size_t>(page_size)
                               : 0;

    switch (action) {
      case PagerAction::NextPage:
        if (at_eof) {
          ++eof_count;
          if (cfg.quit_at_eof && eof_count >= 2) return 0;
        } else {
          top_line =
              std::min(max_top, top_line + static_cast<size_t>(page_size));
          eof_count = 0;
        }
        break;
      case PagerAction::NextLine:
        if (top_line >= max_top) {
          ++eof_count;
          if (cfg.quit_at_eof && eof_count >= 2) return 0;
        } else {
          ++top_line;
          eof_count = 0;
        }
        break;
      case PagerAction::PrevPage:
        top_line = top_line > static_cast<size_t>(page_size)
                       ? top_line - static_cast<size_t>(page_size)
                       : 0;
        eof_count = 0;
        break;
      case PagerAction::PrevLine:
        if (top_line > 0) --top_line;
        eof_count = 0;
        break;
      case PagerAction::SearchForward:
      case PagerAction::SearchBackward: {
        bool forward = action == PagerAction::SearchForward;
        auto compiled = compile_search(cfg, command.text, forward);
        if (compiled) {
          last_search = std::move(*compiled);
          if (auto match =
                  find_search_match(lines, *last_search, top_line, forward)) {
            top_line = std::min(*match, max_top);
          }
        }
        eof_count = 0;
        break;
      }
      case PagerAction::RepeatSearch:
      case PagerAction::ReverseSearch:
        if (last_search) {
          bool forward = last_search->forward;
          if (action == PagerAction::ReverseSearch) forward = !forward;
          if (auto match =
                  find_search_match(lines, *last_search, top_line, forward)) {
            top_line = std::min(*match, max_top);
          }
        }
        eof_count = 0;
        break;
      case PagerAction::Quit:
        return 0;
      case PagerAction::None:
        break;
    }
  }
}

auto run(const Config& cfg) -> int {
  for (const auto& file : cfg.files) {
    auto content_result = read_file_content(file);
    if (!content_result) {
      cp::report_error(content_result, L"less");
      return 1;
    }

    int result = simple_pager(cfg, *content_result);
    if (result != 0) {
      return result;
    }
  }

  return 0;
}

}  // namespace less_pipeline

REGISTER_COMMAND(
    less, "less", "less [OPTION]... [FILE]...",
    "A pager for viewing files, similar to more but with more features.\n"
    "Allows backward movement in the file as well as forward movement.\n"
    "\n"
    "This is a simplified implementation of less with core features.",
    "  less file.txt\n"
    "  less -N file.txt          # Show line numbers\n"
    "  less -E file.txt          # Quit at end of file\n"
    "  less -F file.txt          # Quit if fits on one screen",
    "more(1), most(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd", LESS_OPTIONS) {
  using namespace less_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"less");
    return 1;
  }

  return run(*cfg_result);
}

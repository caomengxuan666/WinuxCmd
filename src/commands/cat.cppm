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
 *  - File: cat.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - @contributor1 caomengxuan666 2507560089@qq.com
///   - @contributor2 <email2@example.com>
///   - @contributor3 <email3@example.com>
/// @Description: Implemention for cat.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

module;
#include "pch/pch.h"
#include "core/command_macros.h"


export module cmd:cat;

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;
/**
 * @brief CAT command options definition
 *
 * This array defines all the options supported by the cat command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 *
 * - @a -A, @a --show-all: Equivalent to -vET [IMPLEMENTED]
 * - @a -b, @a --number-nonblank: Number nonempty output lines, overrides -n
 * [IMPLEMENTED]
 * - @a -e: Equivalent to -vE [IMPLEMENTED]
 * - @a -E, @a --show-ends: Display $ at end of each line [IMPLEMENTED]
 * - @a -n, @a --number: Number all output lines [IMPLEMENTED]
 * - @a -s, @a --squeeze-blank: Suppress repeated empty output lines
 * [IMPLEMENTED]
 * - @a -t: Equivalent to -vT [IMPLEMENTED]
 * - @a -T, @a --show-tabs: Display TAB characters as ^I [IMPLEMENTED]
 * - @a -u: (ignored, for POSIX compatibility) [IMPLEMENTED]
 * - @a -v, @a --show-nonprinting: Use ^ and M- notation, except for LFD and TAB
 * [IMPLEMENTED]
 */
// clang-format off
export auto constexpr CAT_OPTIONS =
    std::array{OPTION("-A", "--show-all", "equivalent to -vET"),
               OPTION("-b", "--number-nonblank", "number nonempty output lines, overrides -n"),
               OPTION("-e", "", "equivalent to -vE"),
               OPTION("-E", "--show-ends", "display $ at end of each line"),
               OPTION("-n", "--number", "number all output lines"),
               OPTION("-s", "--squeeze-blank", "suppress repeated empty output lines"),
               OPTION("-t", "", "equivalent to -vT"),
               OPTION("-T", "--show-tabs", "display TAB characters as ^I"),
               OPTION("-u", "", "(ignored, for POSIX compatibility)"),
               OPTION("-v", "--show-nonprinting", "use ^ and M- notation, except for LFD and TAB")};
// clang-format on

// ======================================================
// Pipeline components
// ======================================================
namespace cat_pipeline {
  namespace cp=core::pipeline;

  // ----------------------------------------------
  // 1. Validate arguments
  // ----------------------------------------------
  auto validate_arguments(const CommandContext<CAT_OPTIONS.size()>& ctx) -> cp::Result<std::vector<std::string>> {
    std::vector<std::string> files;
    for (auto arg : ctx.positionals) {
      files.push_back(std::string(arg));
    }
    
    if (files.empty()) {
      files.push_back("-");
    }
    
    return files;
  }

} // namespace cat_pipeline

REGISTER_COMMAND(
    cat,
    /* cmd_name */ "cat",

    /* cmd_synopsis */ "concatenate files and print on the standard output",

    /* cmd_desc */
    "Concatenate FILE(s) to standard output.\n"
    "With no FILE, or when FILE is -, read standard input.\n"
    "\n"
    "Examples:\n"
    "  cat f g  Output f's contents, then g's contents.\n"
    "  cat      Copy standard input to standard output.",

    /* examples */
    "  cat file.txt              Display contents of file.txt\n"
    "  cat -n file.txt           Number all output lines\n"
    "  cat file1.txt file2.txt   Concatenate multiple files\n"
    "  cat                       Read from standard input",

    /* see_also */ "tac(1), head(1), tail(1), more(1), less(1)",
    /* author */ "caomengxuan666",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */
    CAT_OPTIONS) {
  using namespace cat_pipeline;
  using namespace core::pipeline;

  /**
   * @brief Check if a line is empty
   * @param line Line to check
   * @return true if line is empty or contains only whitespace
   */
  auto is_empty_line = [](const std::string &line) -> bool {
    return line.empty() || (line.size() == 1 && isspace(static_cast<unsigned char>(line[0])));
  };

  /**
   * @brief Process a character with show_nonprinting option
   * @param c Character to process
   * @param ctx Command context
   */
  auto process_character = [&](unsigned char c, const CommandContext<CAT_OPTIONS.size()>& ctx) {
    bool show_nonprinting = ctx.get<bool>("--show-nonprinting", false) || ctx.get<bool>("--show-all", false) || ctx.get<bool>("-e", false) || ctx.get<bool>("-t", false);
    bool show_ends = ctx.get<bool>("--show-ends", false) || ctx.get<bool>("--show-all", false) || ctx.get<bool>("-e", false);
    bool show_tabs = ctx.get<bool>("--show-tabs", false) || ctx.get<bool>("--show-all", false) || ctx.get<bool>("-t", false);

    if (show_nonprinting) {
      if (c < 0x20) {
        // Control characters (except newline, tab, form feed)
        if (c == '\n') {
          safePrintLn(L"");  // prints '\n' in both console and pipe mode
        } else if (c == '\t') {
          if (show_tabs) {
            safePrint(L"^I");
          } else {
            safePrint(L"\t");
          }
        } else if (c == '\f') {
          safePrint(L"^L");
        } else {
          // Convert to ^A, ^B, ..., ^Z (e.g., \x01 → ^A)
          wchar_t caret_char = static_cast<wchar_t>(c + 0x40);
          safePrint(L"^" + std::wstring(1, caret_char));
        }
      } else if (c == 0x7F) {
        // DEL character
        safePrint(L"^?");
      } else if (c >= 0x80) {
        // Non-ASCII: represent as M-x (meta notation)
        wchar_t base_char = static_cast<wchar_t>(c - 0x80);
        safePrint(L"M-" + std::wstring(1, base_char));
      } else {
        // Printable ASCII: output as-is
        safePrint(std::wstring(1, static_cast<wchar_t>(c)));
      }
    } else if (show_tabs && c == '\t') {
      // Only show tabs when requested
      safePrint(L"^I");
    } else {
      // Normal character output
      safePrint(std::wstring(1, static_cast<wchar_t>(c)));
    }
  };

  /**
   * @brief Process a line of text with all options
   * @param line Line of text
   * @param ctx Command context
   * @param line_num Reference to line number counter
   */
  auto process_line = [&](std::string &line, const CommandContext<CAT_OPTIONS.size()>& ctx, size_t &line_num) {
    // Remove trailing carriage return (\r) if present (Windows line endings)
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    bool empty = is_empty_line(line);

    // Handle line numbering
    bool number_lines = ctx.get<bool>("--number", false);
    bool number_nonblank = ctx.get<bool>("--number-nonblank", false);
    bool show_ends = ctx.get<bool>("--show-ends", false) || ctx.get<bool>("--show-all", false) || ctx.get<bool>("-e", false);

    if (number_lines && !number_nonblank) {
      // Format line number with consistent spacing
      std::wostringstream oss;
      oss << std::setw(6) << line_num++ << L" ";
      safePrint(oss.str());
    } else if (number_nonblank && !empty) {
      // Format line number with consistent spacing
      std::wostringstream oss;
      oss << std::setw(6) << line_num++ << L" ";
      safePrint(oss.str());
    }

    // Process each character in the line
    for (char ch : line) {
      process_character(static_cast<unsigned char>(ch), ctx);
    }

    // Show end of line marker if requested
    if (show_ends) {
      safePrint(L"$");
    }

    // End with newline
    safePrintLn(L"");
  };

  /**
   * @brief Process input from a stream
   * @param stream Input stream
   * @param ctx Command context
   * @param line_num Reference to line number counter
   */
  auto process_stream = [&](std::istream &stream, const CommandContext<CAT_OPTIONS.size()>& ctx, size_t &line_num) {
    std::string line;
    bool last_line_empty = false;
    bool squeeze_blank = ctx.get<bool>("--squeeze-blank", false);

    while (std::getline(stream, line)) {
      bool empty = is_empty_line(line);

      // Handle squeeze empty lines option
      if (squeeze_blank && empty && last_line_empty) {
        continue;
      }

      last_line_empty = empty;

      // Process and print the line
      process_line(line, ctx, line_num);
    }
  };

  /**
   * @brief Process a single file (or stdin if path is "-")
   * @param path File path or "-" for stdin
   * @param ctx Command context
   * @param line_num Reference to line number counter
   * @return true if file processed successfully, false on error
   */
  auto process_file = [&](std::string_view path, const CommandContext<CAT_OPTIONS.size()>& ctx, size_t &line_num) -> bool {
    if (path == "-") {
      // Read from stdin
      process_stream(std::cin, ctx, line_num);
      return true;
    } else {
      // Open the file
      std::ifstream file(std::string(path), std::ios::binary);

      if (!file.is_open()) {
        std::wstring wpath_str = utf8_to_wstring(std::string(path));
        safeErrorPrintLn(L"cat: '" + wpath_str + L"': No such file or directory");
        return false;
      }

      // Process the file
      process_stream(file, ctx, line_num);

      if (file.bad()) {
        std::wstring wpath_str = utf8_to_wstring(std::string(path));
        safeErrorPrintLn(L"cat: error reading '" + wpath_str + L"'");
        return false;
      }

      return true;
    }
  };

  // Use the pipeline component to validate arguments
  auto files_result = validate_arguments(ctx);
  if (!files_result) {
    return 1;
  }

  const auto &files = files_result.value();
  int result = 0;
  size_t line_num = 1;

  // Process each file
  for (const auto &file : files) {
    if (!process_file(file, ctx, line_num)) {
      result = 1;  // File processing error
    }
  }

  return result;
}

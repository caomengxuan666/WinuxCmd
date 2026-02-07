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
#include "core/auto_flags.h"
#include "core/command_macros.h"
#include "pch/pch.h"
export module commands.cat;

import std;
import core;
import utils;
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
constexpr auto CAT_OPTIONS = std::array{
    OPTION("-A", "--show-all", "equivalent to -vET"),
    OPTION("-b", "--number-nonblank",
           "number nonempty output lines, overrides -n"),
    OPTION("-e", "", "equivalent to -vE"),
    OPTION("-E", "--show-ends", "display $ at end of each line"),
    OPTION("-n", "--number", "number all output lines"),
    OPTION("-s", "--squeeze-blank", "suppress repeated empty output lines"),
    OPTION("-t", "", "equivalent to -vT"),
    OPTION("-T", "--show-tabs", "display TAB characters as ^I"),
    OPTION("-u", "", "(ignored, for POSIX compatibility)"),
    OPTION("-v", "--show-nonprinting",
           "use ^ and M- notation, except for LFD and TAB")};

// Auto-generated lookup table for options from CAT_OPTIONS
constexpr auto OPTION_HANDLERS = generate_option_handlers(CAT_OPTIONS);

CREATE_AUTO_FLAGS_CLASS(
    CatOptions,
    // Define all flags
    DEFINE_FLAG(number_lines, 0)      // -n, --number
    DEFINE_FLAG(number_nonblank, 1)   // -b, --number-nonblank
    DEFINE_FLAG(show_end, 2)          // -E, --show-ends
    DEFINE_FLAG(squeeze_empty, 3)     // -s, --squeeze-blank
    DEFINE_FLAG(show_nonprinting, 4)  // -v, --show-nonprinting
    DEFINE_FLAG(show_tabs, 5)         // -T, --show-tabs
    DEFINE_FLAG(unbuffered, 6)        // -u, --unbuffered
)

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
  /**
   * @brief Parse command line options for cat
   * @param args Command arguments
   * @param options Output parameter for parsed options
   * @param files Output parameter for files to process
   * @return true if parsing succeeded, false on error
   */
  auto parseCatOptions = [](std::span<std::string_view> args,
                            CatOptions &options,
                            std::vector<std::string_view> &files) -> bool {
    // Helper function to find option handler by long option or short option
    auto find_handler = [](std::string_view arg,
                           char opt_char = '\0') -> const OptionHandler * {
      for (const auto &handler : OPTION_HANDLERS) {
        if ((!arg.empty() && handler.long_opt && arg == handler.long_opt) ||
            (opt_char && handler.short_opt == opt_char)) {
          return &handler;
        }
      }
      return nullptr;
    };

    // Helper function to print option error
    auto print_option_error = [](std::string_view arg, char opt_char = '\0') {
      if (!arg.empty()) {
        std::wstring warg(arg.begin(), arg.end());
        safeErrorPrintLn(L"cat: invalid option -- '" + warg.substr(2) + L"'");
      } else {
        safeErrorPrintLn(L"cat: invalid option -- '" +
                         std::wstring(1, opt_char) + L"'");
      }
    };

    // Helper function to set boolean option
    auto set_boolean_option = [&options](char opt_char) {
      switch (opt_char) {
        case 'b':
          options.set_number_nonblank(true);
          options.set_number_lines(false);  // -b overrides -n
          break;
        case 'n':
          options.set_number_lines(true);
          options.set_number_nonblank(false);  // -n overrides -b
          break;
        case 's':
          options.set_squeeze_empty(true);
          break;
        case 'v':
          options.set_show_nonprinting(true);
          break;
        case 'E':
          options.set_show_end(true);
          break;
        case 'T':
          options.set_show_tabs(true);
          break;
        case 'A':
          options.set_show_nonprinting(true);
          options.set_show_end(true);
          options.set_show_tabs(true);
          break;
        case 'e':
          options.set_show_nonprinting(true);
          options.set_show_end(true);
          break;
        case 't':
          options.set_show_nonprinting(true);
          options.set_show_tabs(true);
          break;
        case 'u':
          options.set_unbuffered(true);
          break;
      }
    };

    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        const auto *handler = find_handler(arg);
        if (handler) {
          if (handler->requires_arg) {
            // cat command has no options that require arguments
            print_option_error(arg);
            return false;
          } else {
            set_boolean_option(handler->short_opt);
          }
        } else {
          print_option_error(arg);
          return false;
        }
      } else if (arg.starts_with('-')) {
        // This is a short option
        if (arg == "-") {
          // Single dash, read from stdin
          files.push_back(arg);
          continue;
        }

        // Process option characters
        for (size_t j = 1; j < arg.size(); ++j) {
          char opt_char = arg[j];
          const auto *handler = find_handler("", opt_char);
          if (handler) {
            if (handler->requires_arg) {
              // cat command has no options that require arguments
              print_option_error("", opt_char);
              return false;
            } else {
              set_boolean_option(opt_char);
            }
          } else {
            print_option_error("", opt_char);
            return false;
          }
        }
      } else {
        // This is a file path
        files.push_back(arg);
      }
    }

    // Default to stdin if no files specified
    if (files.empty()) {
      files.push_back("-");
    }

    return true;
  };

  /**
   * @brief Process a character with show_nonprinting option
   * @param c Character to process
   * @param options cat command options
   */
  auto processCharacter = [](unsigned char c, const CatOptions &options) {
    if (options.get_show_nonprinting()) {
      if (c < 0x20) {
        // Control characters (except newline, tab, form feed)
        if (c == '\n') {
          safePrintLn(L"");  // prints '\n' in both console and pipe mode
        } else if (c == '\t') {
          if (options.get_show_tabs()) {
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
    } else if (options.get_show_tabs() && c == '\t') {
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
   * @param options cat command options
   * @param line_num Reference to line number counter
   */
  auto processLine = [&processCharacter](std::string &line,
                                         const CatOptions &options,
                                         size_t &line_num) {
    // Remove trailing carriage return (\r) if present (Windows line endings)
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    bool is_empty =
        line.empty() ||
        (line.size() == 1 && isspace(static_cast<unsigned char>(line[0])));

    // Handle line numbering
    if (options.get_number_lines()) {
      safePrint(std::to_wstring(line_num++) + L"      ");
    } else if (options.get_number_nonblank() && !is_empty) {
      safePrint(std::to_wstring(line_num++) + L"      ");
    }

    // Process each character in the line
    for (char ch : line) {
      processCharacter(static_cast<unsigned char>(ch), options);
    }

    // Show end of line marker if requested
    if (options.get_show_end()) {
      putwchar('$');
    }

    // End with newline
    putwchar('\n');
  };

  /**
   * @brief Process a single file (or stdin if path is "-")
   * @param path File path or "-" for stdin
   * @param options cat command options
   * @param line_num Reference to line number counter
   * @return true if file processed successfully, false on error
   */
  auto processFile = [&processLine](std::string_view path,
                                    const CatOptions &options,
                                    size_t &line_num) -> bool {
    if (path == "-") {
      // Read from stdin
      std::string line;
      bool last_line_empty = false;

      while (std::getline(std::cin, line)) {
        bool is_empty =
            line.empty() ||
            (line.size() == 1 && isspace(static_cast<unsigned char>(line[0])));

        // Handle squeeze empty lines option
        if (options.get_squeeze_empty() && is_empty && last_line_empty) {
          continue;
        }

        last_line_empty = is_empty;

        // Process and print the line
        processLine(line, options, line_num);
      }

      return true;
    } else {
      // Open the file
      std::ifstream file(std::string(path), std::ios::binary);

      if (!file.is_open()) {
        fprintf(stderr, "cat: '%s': No such file or directory\n", path.data());
        return false;
      }

      std::string line;
      bool last_line_empty = false;

      while (std::getline(file, line)) {
        bool is_empty =
            line.empty() ||
            (line.size() == 1 && isspace(static_cast<unsigned char>(line[0])));

        // Handle squeeze empty lines option
        if (options.get_squeeze_empty() && is_empty && last_line_empty) {
          continue;
        }

        last_line_empty = is_empty;

        // Process and print the line
        processLine(line, options, line_num);
      }

      if (file.bad()) {
        std::wstring wpath_str = utf8_to_wstring(std::string(path));
        safeErrorPrintLn(L"cat: error reading '" + wpath_str + L"'");
        return false;
      }

      return true;
    }
  };

  // Main implementation
  CatOptions options;
  std::vector<std::string_view> files;

  if (!parseCatOptions(args, options, files)) {
    return 2;  // Invalid option error code
  }

  int result = 0;
  size_t line_num = 1;

  // Process each file
  for (const auto &file : files) {
    if (!processFile(file, options, line_num)) {
      result = 1;  // File processing error
    }
  }

  return result;
}

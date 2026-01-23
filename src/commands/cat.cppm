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
// Use global module fragment for C standard library
module;
#include <cctype>
#include <cstdio>

#include "core/command_macros.h"
export module commands.cat;

import std;
import core.dispatcher;
import core.cmd_meta;

/**
 * @brief Concatenate files and print on the standard output
 * @param args Command-line arguments
 * @return Exit code (0 on success, non-zero on error)
 *
 * Options:
 *  -b, --number-nonblank    Number non-empty output lines
 *  -n, --number             Number all output lines
 *  -s, --squeeze-blank      Squeeze multiple adjacent empty lines
 *  -E, --show-ends          Display $ at end of each line
 *  -T, --show-tabs          Display TAB characters as ^I
 *  -v, --show-nonprinting   Use ^ and M- notation, except for LFD and TAB
 *  -e                       Equivalent to -vE
 *  -A, --show-all           Equivalent to -vET
 *  -u                       Disable output buffering
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
  // Option flags for cat command
  struct CatOptions {
    bool number_lines = false;      // -n, --number
    bool number_nonblank = false;   // -b, --number-nonblank
    bool show_end = false;          // -E, --show-ends
    bool squeeze_empty = false;     // -s, --squeeze-blank
    bool show_nonprinting = false;  // -v, --show-nonprinting
    bool show_tabs = false;         // -T, --show-tabs
    bool unbuffered = false;        // -u, --unbuffered
  };

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
    for (size_t i = 0; i < args.size(); ++i) {
      auto arg = args[i];

      if (arg.starts_with("--")) {
        // This is a long option
        if (arg == "--number-nonblank") {
          options.number_nonblank = true;
          options.number_lines = false;  // -b overrides -n
        } else if (arg == "--number") {
          options.number_lines = true;
          options.number_nonblank = false;  // -n overrides -b
        } else if (arg == "--squeeze-blank") {
          options.squeeze_empty = true;
        } else if (arg == "--show-nonprinting") {
          options.show_nonprinting = true;
        } else if (arg == "--show-ends") {
          options.show_end = true;
        } else if (arg == "--show-tabs") {
          options.show_tabs = true;
        } else if (arg == "--show-all") {
          options.show_nonprinting = true;
          options.show_end = true;
          options.show_tabs = true;
        } else if (arg == "--unbuffered") {
          options.unbuffered = true;
        } else {
          fprintf(stderr, "cat: invalid option -- '%.*s'\n",
                  static_cast<int>(arg.size() - 2), arg.data() + 2);
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
          switch (arg[j]) {
            case 'b':
              options.number_nonblank = true;
              options.number_lines = false;  // -b overrides -n
              break;
            case 'n':
              options.number_lines = true;
              options.number_nonblank = false;  // -n overrides -b
              break;
            case 's':
              options.squeeze_empty = true;
              break;
            case 'v':
              options.show_nonprinting = true;
              break;
            case 'E':
              options.show_end = true;
              break;
            case 'T':
              options.show_tabs = true;
              break;
            case 'A':
              options.show_nonprinting = true;
              options.show_end = true;
              options.show_tabs = true;
              break;
            case 'e':
              options.show_nonprinting = true;
              options.show_end = true;
              break;
            case 't':
              options.show_nonprinting = true;
              options.show_tabs = true;
              break;
            case 'u':
              options.unbuffered = true;
              break;
            default:
              fprintf(stderr, "cat: invalid option -- '%c'\n", arg[j]);
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
    if (options.show_nonprinting) {
      if (c < 0x20) {
        // Control characters (except newline, tab, form feed)
        if (c == '\n') {
          putchar('\n');
        } else if (c == '\t') {
          if (options.show_tabs) {
            printf("^I");
          } else {
            putchar('\t');
          }
        } else if (c == '\f') {
          printf("^L");
        } else {
          printf("^%c", c + 0x40);  // Convert to ^A, ^B, etc.
        }
      } else if (c == 0x7f) {
        // DEL character
        printf("^?");
      } else if (c >= 0x80) {
        // Non-ASCII characters
        printf("M-%c", c - 0x80);
      } else {
        // Printable ASCII characters
        putchar(c);
      }
    } else if (options.show_tabs && c == '\t') {
      // Only show tabs when requested
      printf("^I");
    } else {
      // Normal character output
      putchar(c);
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
    if (options.number_lines) {
      printf("%6zu  ", line_num++);
    } else if (options.number_nonblank && !is_empty) {
      printf("%6zu  ", line_num++);
    }

    // Process each character in the line
    for (char ch : line) {
      processCharacter(static_cast<unsigned char>(ch), options);
    }

    // Show end of line marker if requested
    if (options.show_end) {
      putchar('$');
    }

    // End with newline
    putchar('\n');
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
        if (options.squeeze_empty && is_empty && last_line_empty) {
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
        if (options.squeeze_empty && is_empty && last_line_empty) {
          continue;
        }

        last_line_empty = is_empty;

        // Process and print the line
        processLine(line, options, line_num);
      }

      if (file.bad()) {
        fprintf(stderr, "cat: error reading '%s'\n", path.data());
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

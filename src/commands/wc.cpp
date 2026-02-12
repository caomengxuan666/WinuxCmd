/// @Author: TODO: fill in your name
/// @contributors:
///   - contributor1 <name> <email2@example.com>
///   - contributor2 <name> <email2@example.com>
///   - contributor3 <name> <email3@example.com>
///   - description:
/// @Description: TODO: Add command description
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

// ======================================================
// Constants
// ======================================================
namespace wc_constants {
// Add constants here
}

// ======================================================
// Options (constexpr)
// ======================================================

/**
 * @brief WC command options definition
 *
 * This array defines all the options supported by the wc command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 * - @a -c, @a --bytes: Print the byte counts [IMPLEMENTED]
 * - @a -m, @a --chars: Print the character counts [IMPLEMENTED]
 * - @a -l, @a --lines: Print the newline counts [IMPLEMENTED]
 * - @a --files0-from=F: Read input from the files specified by NUL-terminated
 * names in file F [TODO]
 * - @a -L, @a --max-line-length: Print the maximum display width [IMPLEMENTED]
 * - @a -w, @a --words: Print the word counts [IMPLEMENTED]
 * - @a --total=WHEN: When to print a line with total counts [IMPLEMENTED]
 * - @a --help: Display this help and exit [IMPLEMENTED]
 * - @a --version: Output version information and exit [IMPLEMENTED]
 */
auto constexpr WC_OPTIONS = std::array{
    OPTION("-c", "--bytes", "print the byte counts"),
    OPTION("-m", "--chars", "print the character counts"),
    OPTION("-l", "--lines", "print the newline counts"),
    OPTION(
        "--files0-from", "",
        "read input from the files specified by NUL-terminated names in file F",
        STRING_TYPE),
    OPTION("-L", "--max-line-length", "print the maximum display width"),
    OPTION("-w", "--words", "print the word counts"),
    OPTION("--total", "", "when to print a line with total counts",
           STRING_TYPE),
    OPTION("--help", "", "display this help and exit"),
    OPTION("--version", "", "output version information and exit")};

// ======================================================
// Pipeline components
// ======================================================
namespace wc_pipeline {
namespace cp = core::pipeline;

// ----------------------------------------------
// 1. Types
// ----------------------------------------------
/**
 * @brief Structure to store count results
 */
struct CountResult {
  std::uintmax_t lines = 0;
  std::uintmax_t words = 0;
  std::uintmax_t chars = 0;
  std::uintmax_t bytes = 0;
  std::uintmax_t max_line_length = 0;
  std::string filename;
};

// ----------------------------------------------
// 2. Validate arguments
// ----------------------------------------------
/**
 * @brief Validate command arguments
 *
 * This function validates the command arguments and returns a list of files to
 * process. If no files are provided, it returns an empty list indicating stdin
 * should be used.
 *
 * @param args Command line arguments
 * @return A Result containing the list of files to process
 */
auto validate_arguments(std::span<const std::string_view> args)
    -> cp::Result<std::vector<std::string>> {
  std::vector<std::string> paths;
  for (auto arg : args) {
    paths.push_back(std::string(arg));
  }
  return paths;
}

// ----------------------------------------------
// 3. Count file contents
// ----------------------------------------------
/**
 * @brief Count lines, words, chars, bytes, and max line length in a file
 *
 * This function reads a file and counts the number of lines, words, characters,
 * bytes, and the maximum line length.
 *
 * @param path Path to the file to count
 * @return A Result containing the count result
 */
auto count_file(const std::string& path) -> cp::Result<CountResult> {
  CountResult result;
  result.filename = path;

  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return std::unexpected("cannot open file '" + path + "'");
  }

  std::string line;
  std::uintmax_t current_line_length = 0;
  bool in_word = false;

  char c;
  while (file.get(c)) {
    result.bytes++;
    result.chars++;

    if (c == '\n') {
      result.lines++;
      if (current_line_length > result.max_line_length) {
        result.max_line_length = current_line_length;
      }
      current_line_length = 0;
      in_word = false;
    } else if (std::isspace(static_cast<unsigned char>(c))) {
      current_line_length++;
      in_word = false;
    } else {
      current_line_length++;
      if (!in_word) {
        result.words++;
        in_word = true;
      }
    }
  }

  // Check for final line without newline
  if (current_line_length > 0) {
    result.lines++;
    if (current_line_length > result.max_line_length) {
      result.max_line_length = current_line_length;
    }
  }

  return result;
}

// ----------------------------------------------
// 4. Count stdin contents
// ----------------------------------------------
/**
 * @brief Count lines, words, chars, bytes, and max line length from stdin
 *
 * This function reads from standard input and counts the number of lines,
 * words, characters, bytes, and the maximum line length.
 *
 * @return A Result containing the count result
 */
auto count_stdin() -> cp::Result<CountResult> {
  CountResult result;
  result.filename = "-";

  std::uintmax_t current_line_length = 0;
  bool in_word = false;

  char c;
  while (std::cin.get(c)) {
    result.bytes++;
    result.chars++;

    if (c == '\n') {
      result.lines++;
      if (current_line_length > result.max_line_length) {
        result.max_line_length = current_line_length;
      }
      current_line_length = 0;
      in_word = false;
    } else if (std::isspace(static_cast<unsigned char>(c))) {
      current_line_length++;
      in_word = false;
    } else {
      current_line_length++;
      if (!in_word) {
        result.words++;
        in_word = true;
      }
    }
  }

  // Check for final line without newline
  if (current_line_length > 0) {
    result.lines++;
    if (current_line_length > result.max_line_length) {
      result.max_line_length = current_line_length;
    }
  }

  return result;
}

// ----------------------------------------------
// 5. Main pipeline
// ----------------------------------------------
/**
 * @brief Main command processing pipeline
 *
 * This function implements the main processing pipeline for the wc command.
 * It processes the command context, validates arguments, and counts file
 * contents.
 *
 * @tparam N Number of options in the command context
 * @param ctx Command context containing options and arguments
 * @return A Result containing the list of count results
 */
template <size_t N>
auto process_command(const CommandContext<N>& ctx)
    -> cp::Result<std::vector<CountResult>> {
  return validate_arguments(ctx.positionals)
      .and_then([](std::vector<std::string> paths)
                    -> cp::Result<std::vector<CountResult>> {
        std::vector<CountResult> results;

        if (paths.empty()) {
          // Read from stdin
          auto stdin_result = count_stdin();
          if (!stdin_result) {
            return std::unexpected(stdin_result.error());
          }
          results.push_back(*stdin_result);
        } else {
          // Read from files
          for (const auto& path : paths) {
            auto file_result = count_file(path);
            if (!file_result) {
              return std::unexpected(file_result.error());
            }
            results.push_back(*file_result);
          }
        }

        return results;
      });
}

}  // namespace wc_pipeline

// ======================================================
// Command registration
// ======================================================

REGISTER_COMMAND(
    wc,
    /* name */
    "wc",

    /* synopsis */
    "wc [OPTION]... [FILE]...",

    /* description */
    "Print newline, word, and byte counts for each FILE, and a total line if\n"
    "more than one FILE is specified.  A word is a non-zero-length sequence "
    "of\n"
    "printable characters delimited by white space.\n"
    "\n"
    "With no FILE, or when FILE is -, read standard input.\n"
    "\n"
    "The options below may be used to select which counts are printed, always "
    "in\n"
    "the following order: newline, word, character, byte, maximum line "
    "length.\n"
    "  -c, --bytes            print the byte counts\n"
    "  -m, --chars            print the character counts\n"
    "  -l, --lines            print the newline counts\n"
    "      --files0-from=F    read input from the files specified by\n"
    "                           NUL-terminated names in file F;\n"
    "                           If F is - then read names from standard input\n"
    "  -L, --max-line-length  print the maximum display width\n"
    "  -w, --words            print the word counts\n"
    "      --total=WHEN       when to print a line with total counts;\n"
    "                           WHEN can be: auto, always, only, never\n"
    "      --help        display this help and exit\n"
    "      --version     output version information and exit",

    /* examples */
    "  wc file.txt           # Count lines, words, and bytes in file.txt\n"
    "  wc -l file.txt        # Count only lines in file.txt\n"
    "  wc -w file.txt        # Count only words in file.txt\n"
    "  wc -c file.txt        # Count only bytes in file.txt\n"
    "  wc -m file.txt        # Count only characters in file.txt\n"
    "  wc -L file.txt        # Print maximum line length in file.txt\n"
    "  wc file1.txt file2.txt # Count multiple files and show total",

    /* see_also */
    "cat(1), grep(1)",

    /* author */
    "WinuxCmd",

    /* copyright */
    "Copyright © 2026 WinuxCmd",

    /* options */
    WC_OPTIONS) {
  using namespace wc_pipeline;

  // Check for --help or --version
  if (ctx.get<bool>("--help", false)) {
    // Help is automatically handled by the framework
    return 0;
  }

  if (ctx.get<bool>("--version", false)) {
    // OPTIMIZED: Use string literals instead of wstring
    safePrintLn("wc (WinuxCmd) 0.1.0");
    safePrintLn("Copyright © 2026 WinuxCmd");
    safePrintLn("This is free software; see the source for copying conditions.");
    safePrintLn("There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    return 0;
  }

  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"wc");
    return 1;
  }

  auto count_results = *result;

  // Determine which counts to print
  bool print_lines =
      ctx.get<bool>("--lines", false) || ctx.get<bool>("-l", false);
  bool print_words =
      ctx.get<bool>("--words", false) || ctx.get<bool>("-w", false);
  bool print_chars =
      ctx.get<bool>("--chars", false) || ctx.get<bool>("-m", false);
  bool print_bytes =
      ctx.get<bool>("--bytes", false) || ctx.get<bool>("-c", false);
  bool print_max_line_length =
      ctx.get<bool>("--max-line-length", false) || ctx.get<bool>("-L", false);

  // If no options specified, print lines, words, and bytes
  if (!print_lines && !print_words && !print_chars && !print_bytes &&
      !print_max_line_length) {
    print_lines = true;
    print_words = true;
    print_bytes = true;
  }

  // Determine when to print total
  std::string total_when = ctx.get<std::string>("--total", "auto");
  bool print_total = false;

  if (total_when == "always") {
    print_total = true;
  } else if (total_when == "never") {
    print_total = false;
  } else if (total_when == "only") {
    print_total = true;
  } else {  // auto
    print_total = count_results.size() > 1;
  }

  // Calculate total
  CountResult total_result;
  total_result.filename = "total";

  for (const auto& result : count_results) {
    total_result.lines += result.lines;
    total_result.words += result.words;
    total_result.chars += result.chars;
    total_result.bytes += result.bytes;
    if (result.max_line_length > total_result.max_line_length) {
      total_result.max_line_length = result.max_line_length;
    }
  }

  // Print results
  auto print_result = [&](const CountResult& result) {
    // OPTIMIZED: Use snprintf instead of to_wstring and avoid wstring concatenation
    char buf[256];
    int offset = 0;
    
    if (print_lines) {
      int len = snprintf(buf + offset, sizeof(buf) - offset, "%ju ", result.lines);
      offset += len;
    }
    if (print_words) {
      int len = snprintf(buf + offset, sizeof(buf) - offset, "%ju ", result.words);
      offset += len;
    }
    if (print_chars) {
      int len = snprintf(buf + offset, sizeof(buf) - offset, "%ju ", result.chars);
      offset += len;
    }
    if (print_bytes) {
      int len = snprintf(buf + offset, sizeof(buf) - offset, "%ju ", result.bytes);
      offset += len;
    }
    if (print_max_line_length) {
      int len = snprintf(buf + offset, sizeof(buf) - offset, "%ju ", result.max_line_length);
      offset += len;
    }
    
    // Remove trailing space
    if (offset > 0 && buf[offset - 1] == ' ') {
      buf[offset - 1] = '\0';
      offset--;
    }
    
    if (!(result.filename == "-" && count_results.size() == 1)) {
      if (offset > 0) {
        buf[offset++] = ' ';
      }
      // Copy filename (assuming it fits in remaining buffer)
      size_t filename_len = result.filename.length();
      if (offset + filename_len < sizeof(buf)) {
        memcpy(buf + offset, result.filename.c_str(), filename_len);
        offset += filename_len;
      }
      buf[offset] = '\0';
    }
    
    safePrintLn(std::string_view(buf, offset));
  };

  if (total_when == "only") {
    print_result(total_result);
  } else {
    for (const auto& result : count_results) {
      print_result(result);
    }

    if (print_total) {
      print_result(total_result);
    }
  }

  return 0;
}

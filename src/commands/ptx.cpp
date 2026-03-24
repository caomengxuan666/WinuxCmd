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
 *  - File: ptx.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for ptx.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
//include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr PTX_OPTIONS = std::array{
    OPTION("-A", "--auto-reference", "output automatically generated references", BOOL_TYPE),
    OPTION("-C", "--copyright", "display copyright and version", BOOL_TYPE),
    OPTION("-G", "--traditional", "behave more like System V 'ptx'", BOOL_TYPE),
    OPTION("-F", "--flag-truncation", "truncate words flagged by 'g'", STRING_TYPE),
    OPTION("-M", "--macro-name", "macro name for missing file", STRING_TYPE),
    OPTION("-O", "--format", "roff format for output", BOOL_TYPE),
    OPTION("-R", "--right-side-refs", "put references in right margin", BOOL_TYPE),
    OPTION("-S", "--sentence-regexp", "regexp for sentence ends", STRING_TYPE),
    OPTION("-T", "--tabs", "tabs in output", STRING_TYPE),
    OPTION("-W", "--word-regexp", "regexp for words", STRING_TYPE),
    OPTION("-b", "--break", "file break character", STRING_TYPE),
    OPTION("-f", "--ignore-case", "fold lower case to upper case for sorting", BOOL_TYPE),
    OPTION("-g", "--gap-size", "gap size for output", STRING_TYPE),
    OPTION("-i", "--ignore-file", "ignore file", STRING_TYPE),
    OPTION("-o", "--only-file", "output only file", BOOL_TYPE),
    OPTION("-r", "--references", "first field is reference", BOOL_TYPE),
    OPTION("-t", "--typeset-mode", "output for troff/nroff", BOOL_TYPE),
    OPTION("-w", "--width", "output width", STRING_TYPE)
};

namespace ptx_pipeline {
namespace cp = core::pipeline;

struct Config {
  bool auto_reference = false;
  bool copyright = false;
  bool traditional = false;
  std::string flag_truncation;
  std::string macro_name;
  bool roff_format = false;
  bool right_side_refs = false;
  std::string sentence_regexp;
  std::string tabs;
  std::string word_regexp;
  std::string break_char;
  bool ignore_case = false;
  std::string gap_size;
  std::string ignore_file;
  bool only_file = false;
  bool references = false;
  bool typeset_mode = false;
  int width = 72;
  SmallVector<std::string, 64> files;
};

auto build_config(const CommandContext<PTX_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  cfg.auto_reference = ctx.get<bool>("--auto-reference", false) || ctx.get<bool>("-A", false);
  cfg.copyright = ctx.get<bool>("--copyright", false) || ctx.get<bool>("-C", false);
  cfg.traditional = ctx.get<bool>("--traditional", false) || ctx.get<bool>("-G", false);
  cfg.roff_format = ctx.get<bool>("--format", false) || ctx.get<bool>("-O", false);
  cfg.right_side_refs = ctx.get<bool>("--right-side-refs", false) || ctx.get<bool>("-R", false);
  cfg.ignore_case = ctx.get<bool>("--ignore-case", false) || ctx.get<bool>("-f", false);
  cfg.only_file = ctx.get<bool>("--only-file", false) || ctx.get<bool>("-o", false);
  cfg.references = ctx.get<bool>("--references", false) || ctx.get<bool>("-r", false);
  cfg.typeset_mode = ctx.get<bool>("--typeset-mode", false) || ctx.get<bool>("-t", false);

  auto trunc_opt = ctx.get<std::string>("--flag-truncation", "");
  if (trunc_opt.empty()) {
    trunc_opt = ctx.get<std::string>("-F", "");
  }
  cfg.flag_truncation = trunc_opt;

  auto macro_opt = ctx.get<std::string>("--macro-name", "");
  if (macro_opt.empty()) {
    macro_opt = ctx.get<std::string>("-M", "");
  }
  cfg.macro_name = macro_opt;

  auto sent_opt = ctx.get<std::string>("--sentence-regexp", "");
  if (sent_opt.empty()) {
    sent_opt = ctx.get<std::string>("-S", "");
  }
  cfg.sentence_regexp = sent_opt;

  auto tabs_opt = ctx.get<std::string>("--tabs", "");
  if (tabs_opt.empty()) {
    tabs_opt = ctx.get<std::string>("-T", "");
  }
  cfg.tabs = tabs_opt;

  auto word_opt = ctx.get<std::string>("--word-regexp", "");
  if (word_opt.empty()) {
    word_opt = ctx.get<std::string>("-W", "");
  }
  cfg.word_regexp = word_opt;

  auto break_opt = ctx.get<std::string>("--break", "");
  if (break_opt.empty()) {
    break_opt = ctx.get<std::string>("-b", "");
  }
  cfg.break_char = break_opt;

  auto gap_opt = ctx.get<std::string>("--gap-size", "");
  if (gap_opt.empty()) {
    gap_opt = ctx.get<std::string>("-g", "");
  }
  cfg.gap_size = gap_opt;

  auto ignore_opt = ctx.get<std::string>("--ignore-file", "");
  if (ignore_opt.empty()) {
    ignore_opt = ctx.get<std::string>("-i", "");
  }
  cfg.ignore_file = ignore_opt;

  auto width_opt = ctx.get<std::string>("--width", "");
  if (width_opt.empty()) {
    width_opt = ctx.get<std::string>("-w", "");
  }
  if (!width_opt.empty()) {
    try {
      cfg.width = std::stoi(width_opt);
    } catch (...) {
      return std::unexpected("invalid width value");
    }
  }

  for (auto arg : ctx.positionals) {
    cfg.files.push_back(std::string(arg));
  }

  if (cfg.files.empty()) {
    cfg.files.push_back("-");
  }

  return cfg;
}

auto run(const Config& cfg) -> int {
  // Note: ptx is a very complex command that generates a permuted index
  // This is a placeholder implementation that just explains the command
  
  safePrintLn("ptx: permuted index generator");
  safePrintLn("");
  safePrintLn("The ptx command generates a permuted index of file contents,");
  safePrintLn("listing all words alphabetically with their context.");
  safePrintLn("");
  safePrintLn("This is a complex command that requires:");
  safePrintLn("  - Word extraction and tokenization");
  safePrintLn("  - Context tracking (lines before/after each word)");
  safePrintLn("  - Alphabetical sorting");
  safePrintLn("  - Formatting the output");
  safePrintLn("");
  safePrintLn("Note: Full implementation is not provided due to complexity.");
  safePrintLn("This command is mainly used for building book indexes.");
  
  return 1;
}

}  // namespace ptx_pipeline

REGISTER_COMMAND(ptx, "ptx",
                 "ptx [OPTION]... [FILE]...",
                 "Produce a permuted index of file contents.\n"
                 "\n"
                 "Output a permuted index, including context, of the words in the given files.\n"
                 "\n"
                 "Note: This is an advanced command. Full implementation is not provided\n"
                 "due to its complexity. This is mainly used for building book indexes.",
                 "  ptx file.txt\n"
                 "  ptx -w file.txt",
                 "grep(1), sort(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", PTX_OPTIONS) {
  using namespace ptx_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"ptx");
    return 1;
  }

  return run(*cfg_result);
}
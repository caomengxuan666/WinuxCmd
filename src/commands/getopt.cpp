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
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: getopt.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Implementation for getopt.
/// @Version: 0.1.0
/// @License: MIT

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr GETOPT_OPTIONS = std::array{
    // [GNU] -o, --options: short option specification
    OPTION("-o", "--options", "short option specification", STRING_TYPE),
    // [GNU] -l, --longoptions: long options to be recognized
    OPTION("-l", "--longoptions", "long options to be recognized", STRING_TYPE),
    // [GNU] -n, --name: name used in diagnostics
    OPTION("-n", "--name", "name used in diagnostics", STRING_TYPE),
    // [GNU] -q, --quiet: disable error reporting by getopt
    OPTION("-q", "--quiet", "disable error reporting by getopt"),
    // [GNU] -Q, --quiet-output: no normal output
    OPTION("-Q", "--quiet-output", "no normal output"),
    // [GNU] -s, --shell: set quoting conventions to those of <shell>
    OPTION("-s", "--shell", "set quoting conventions to those of <shell>",
           STRING_TYPE),
    // [GNU] -T, --test: test for getopt(1) version
    OPTION("-T", "--test", "test for getopt(1) version"),
    // [GNU] -u, --unquoted: do not quote the output
    OPTION("-u", "--unquoted", "do not quote the output"),
    // [GNU] -a, --alternative: allow long options starting with single -
    OPTION("-a", "--alternative", "allow long options starting with single -")};

namespace getopt_pipeline {

enum class ArgKind { None, Required, Optional };

struct Config {
  std::string name = "getopt";
  std::string optstring;
  std::string longoptions;
  bool quiet = false;
  bool quiet_output = false;
  bool unquoted = false;
  bool alternative = false;
  std::string shell;
  bool test = false;
  std::vector<std::string> args;
};

auto shell_quote(std::string_view text) -> std::string {
  std::string out = "'";
  for (char ch : text) {
    if (ch == '\'') {
      out += "'\\''";
    } else {
      out.push_back(ch);
    }
  }
  out.push_back('\'');
  return out;
}

auto output_token(std::string_view text, bool unquoted) -> std::string {
  if (unquoted) return std::string(text);
  return shell_quote(text);
}

auto option_kind(std::string_view optstring, char option)
    -> std::optional<ArgKind> {
  for (size_t i = 0; i < optstring.size(); ++i) {
    if (optstring[i] != option) continue;
    if (i + 1 < optstring.size() && optstring[i + 1] == ':') {
      if (i + 2 < optstring.size() && optstring[i + 2] == ':') {
        return ArgKind::Optional;
      }
      return ArgKind::Required;
    }
    return ArgKind::None;
  }
  return std::nullopt;
}

auto build_config(const CommandContext<GETOPT_OPTIONS.size()>& ctx)
    -> std::optional<Config> {
  Config cfg;
  cfg.name = ctx.get<std::string>("-n", ctx.get<std::string>("--name", ""));
  if (cfg.name.empty()) cfg.name = "getopt";
  cfg.quiet = ctx.get<bool>("-q", false) || ctx.get<bool>("--quiet", false);
  cfg.quiet_output =
      ctx.get<bool>("-Q", false) || ctx.get<bool>("--quiet-output", false);
  cfg.unquoted =
      ctx.get<bool>("-u", false) || ctx.get<bool>("--unquoted", false);
  cfg.alternative =
      ctx.get<bool>("-a", false) || ctx.get<bool>("--alternative", false);
  cfg.shell = ctx.get<std::string>("-s", ctx.get<std::string>("--shell", ""));
  cfg.test = ctx.get<bool>("-T", false) || ctx.get<bool>("--test", false);
  cfg.optstring =
      ctx.get<std::string>("-o", ctx.get<std::string>("--options", ""));
  cfg.longoptions =
      ctx.get<std::string>("-l", ctx.get<std::string>("--longoptions", ""));

  // Handle -T/--test: test for getopt version
  if (cfg.test) {
    safePrintLn("getopt (GNU libc) 1.1.6");
    return std::nullopt;
  }

  size_t first_arg = 0;
  if (cfg.optstring.empty()) {
    if (ctx.positionals.empty()) return std::nullopt;
    cfg.optstring = std::string(ctx.positionals.front());
    first_arg = 1;
  }

  for (size_t i = first_arg; i < ctx.positionals.size(); ++i) {
    cfg.args.emplace_back(ctx.positionals[i]);
  }
  return cfg;
}

auto fail(const Config& cfg, std::string_view message) -> int {
  if (!cfg.quiet) safeErrorPrintLn(cfg.name + ": " + std::string(message));
  return 1;
}

auto run(const Config& cfg) -> int {
  std::vector<std::string> options;
  std::vector<std::string> operands;

  for (size_t i = 0; i < cfg.args.size(); ++i) {
    std::string_view arg = cfg.args[i];
    if (arg == "--") {
      for (++i; i < cfg.args.size(); ++i) operands.push_back(cfg.args[i]);
      break;
    }
    if (arg.size() < 2 || arg[0] != '-' || arg == "-") {
      operands.emplace_back(arg);
      continue;
    }

    for (size_t pos = 1; pos < arg.size(); ++pos) {
      char option = arg[pos];
      auto kind = option_kind(cfg.optstring, option);
      if (!kind) {
        return fail(cfg, std::string("invalid option -- '") + option + "'");
      }

      options.push_back(std::string("-") + option);
      if (*kind == ArgKind::None) continue;

      std::string value;
      if (pos + 1 < arg.size()) {
        value = std::string(arg.substr(pos + 1));
        pos = arg.size();
      } else if (*kind == ArgKind::Required) {
        if (i + 1 >= cfg.args.size()) {
          return fail(cfg, std::string("option requires an argument -- '") +
                               option + "'");
        }
        value = cfg.args[++i];
      }

      if (*kind == ArgKind::Required || !value.empty()) {
        options.push_back(std::move(value));
      }
      break;
    }
  }

  std::string out;
  auto append = [&](std::string_view token) {
    if (!out.empty()) out.push_back(' ');
    out += output_token(token, cfg.unquoted);
  };

  // Handle -Q/--quiet-output: no normal output
  if (cfg.quiet_output) {
    // Still process arguments but don't output anything
    for (size_t i = 0; i < cfg.args.size(); ++i) {
      std::string_view arg = cfg.args[i];
      if (arg == "--") {
        break;
      }
    }
    return 0;
  }

  for (const auto& option : options) append(option);
  append("--");
  for (const auto& operand : operands) append(operand);
  safePrintLn(out);
  return 0;
}

}  // namespace getopt_pipeline

REGISTER_COMMAND(getopt, "getopt", "getopt [OPTION]... OPTSTRING -- ARGS...",
                 "Parse command options and print a shell-compatible argument "
                 "list.\n"
                 "This native subset supports short options, required ':' "
                 "arguments, and optional '::' arguments.",
                 "  getopt ab:c -- -a -b value file\n"
                 "  getopt -o ab:c -- -a -b value file",
                 "env(1), xargs(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
                 GETOPT_OPTIONS) {
  auto cfg = getopt_pipeline::build_config(ctx);
  if (!cfg) {
    safeErrorPrintLn("getopt: missing optstring");
    safeErrorPrintLn("Try 'getopt --help' for more information.");
    return 1;
  }
  return getopt_pipeline::run(*cfg);
}

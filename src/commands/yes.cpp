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
 *  - File: yes.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for yes.
/// @Version: 0.2.0
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

auto constexpr YES_OPTIONS =
    std::array{OPTION("", "", "output a string repeatedly", STRING_TYPE)};

namespace yes_pipeline {
namespace cp = core::pipeline;

struct Config {
  std::string output = "y";
};

auto join_yes_args(std::span<const std::string_view> args) -> std::string {
  std::string output;
  for (size_t i = 0; i < args.size(); ++i) {
    if (i != 0) output += char(32);
    output += args[i];
  }
  return output;
}

auto build_config(const CommandContext<YES_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  if (!ctx.positionals.empty()) {
    cfg.output = join_yes_args(std::span<const std::string_view>(
        ctx.positionals.data(), ctx.positionals.size()));
  }
  return cfg;
}

std::string repeated_block(const std::string& line) {
  const std::string unit = line + "\n";
  std::string block = unit;
  constexpr size_t target_size = 8192;
  while (block.size() + block.size() <= target_size) {
    block.append(block.data(), block.size());
  }
  while (block.size() + unit.size() <= target_size) {
    block += unit;
  }
  return block;
}

std::optional<size_t> test_repeat_limit() {
  const char* raw_limit = getenv("WINUXCMD_YES_REPEAT_LIMIT");
  if (!raw_limit || !*raw_limit) return std::nullopt;
  std::string_view text(raw_limit);
  size_t value = 0;
  auto [ptr, ec] =
      std::from_chars(text.data(), text.data() + text.size(), value);
  if (ec != std::errc() || ptr != text.data() + text.size()) {
    return std::nullopt;
  }
  return value;
}

auto run(const Config& cfg) -> int {
  if (auto limit = test_repeat_limit()) {
    std::string line = cfg.output + "\n";
    for (size_t i = 0; i < *limit; ++i) {
      if (fwrite(line.data(), 1, line.size(), stdout) != line.size()) {
        return ferror(stdout) ? 1 : 0;
      }
    }
    return 0;
  }

  std::string block = repeated_block(cfg.output);
  for (;;) {
    size_t written = fwrite(block.data(), 1, block.size(), stdout);
    if (written != block.size()) {
      return ferror(stdout) ? 1 : 0;
    }
  }
}

}  // namespace yes_pipeline

REGISTER_COMMAND(yes, "yes", "yes [STRING]...",
                 "Repeatedly output a line with all specified STRING(s), or y.",
                 "  yes\n"
                 "  yes please",
                 "", "WinuxCmd", "Copyright © 2026 WinuxCmd", YES_OPTIONS) {
  using namespace yes_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"yes");
    return 1;
  }

  return run(*cfg_result);
}

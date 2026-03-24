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
 *  - File: mkfifo.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for mkfifo.
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

auto constexpr MKFIFO_OPTIONS = std::array{
    OPTION("-m", "--mode", "set file permission mode (not implemented on Windows)", STRING_TYPE),
    OPTION("-Z", "--context", "set SELinux security context (not implemented on Windows)", STRING_TYPE)
};

namespace mkfifo_pipeline {
namespace cp = core::pipeline;

struct Config {
  std::string mode;
  std::string context;
  SmallVector<std::string, 64> fifos;
};

auto build_config(const CommandContext<MKFIFO_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;

  auto mode_opt = ctx.get<std::string>("--mode", "");
  if (mode_opt.empty()) {
    mode_opt = ctx.get<std::string>("-m", "");
  }
  cfg.mode = mode_opt;

  auto context_opt = ctx.get<std::string>("--context", "");
  if (context_opt.empty()) {
    context_opt = ctx.get<std::string>("-Z", "");
  }
  cfg.context = context_opt;

  for (auto arg : ctx.positionals) {
    cfg.fifos.push_back(std::string(arg));
  }

  if (cfg.fifos.empty()) {
    return std::unexpected("missing operand");
  }

  return cfg;
}

auto run(const Config& cfg) -> int {
  // Note: Windows doesn't support named pipes in the same way as Unix FIFOs
  // Windows has named pipes but they are created programmatically, not via mkfifo
  
  safePrintLn("mkfifo: named pipes (FIFOs) are not supported on Windows in the same way as Unix.");
  safePrintLn("mkfifo: Windows supports named pipes but they must be created programmatically");
  safePrintLn("mkfifo: using CreateNamedPipe() API or via PowerShell's New-Item cmdlet");
  safePrintLn("");
  safePrintLn("To create a named pipe on Windows:");
  safePrintLn("  PowerShell: New-Item -ItemType NamedPipe -Path \\\\.\\pipe\\mypipe");
  
  return 1;
}

}  // namespace mkfifo_pipeline

REGISTER_COMMAND(mkfifo, "mkfifo",
                 "mkfifo [OPTION]... NAME...",
                 "Create named pipes (FIFOs) with the given NAMEs.\n"
                 "\n"
                 "Note: This command is not implemented on Windows because Windows\n"
                 "doesn't support Unix-style FIFOs. Windows has named pipes but they\n"
                 "work differently and must be created programmatically.",
                 "  mkfifo mypipe",
                 "mknod(1), readlink(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", MKFIFO_OPTIONS) {
  using namespace mkfifo_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"mkfifo");
    return 1;
  }

  return run(*cfg_result);
}
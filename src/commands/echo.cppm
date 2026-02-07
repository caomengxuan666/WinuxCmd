module;
#include "pch/pch.h"
#include "core/command_macros.h"


export module cmd:echo;

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

// ======================================================
// Constants
// ======================================================
namespace echo_constants {
constexpr int MIN_REPEAT = 1;
constexpr int MAX_REPEAT = 100000;
}  // namespace echo_constants

// ======================================================
// Options (constexpr)
// ======================================================

export auto constexpr ECHO_OPTIONS =
    std::array{OPTION("-u", "--upper", "convert text to uppercase"),
               OPTION("-r", "--repeat", "repeat output N times", INT_TYPE)};

// ======================================================
// Pipeline components
// ======================================================
namespace echo_pipeline {
   namespace cp=core::pipeline;
  // ----------------------------------------------
  // 1. Build text
  // ----------------------------------------------
  auto build_text(std::span<const std::string_view> args) -> cp::Result<std::string> {
    if (args.empty()) return std::unexpected("no arguments provided");
    std::string text;
    for (auto arg : args) {
      if (!text.empty()) text += ' ';
      text += arg;
    }
    return text;
  }

  // ----------------------------------------------
  // 2. Uppercase transformation
  // ----------------------------------------------
  auto to_uppercase(std::string text, bool enabled) -> cp::Result<std::string> {
    if (enabled) {
      for (char &c : text) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return text;
  }

  // ----------------------------------------------
  // 3. Repeat count validation
  // ----------------------------------------------
  auto validate_repeat(int count) -> cp::Result<int> {
    if (count < echo_constants::MIN_REPEAT || count > echo_constants::MAX_REPEAT)
      return std::unexpected("repeat count out of range");
    return count;
  }

  // ----------------------------------------------
  // 4. Main pipeline
  // ----------------------------------------------
  template<size_t N>
  auto process_command(const CommandContext<N>& ctx)
      -> cp::Result<std::pair<std::string, int>>
  {
    return build_text(ctx.positionals)
        .and_then([&](std::string text) {
            return to_uppercase(std::move(text), ctx.get<bool>("--upper", false));
        })
        .and_then([&](std::string utext) {
            return validate_repeat(ctx.get<int>("--repeat", 1))
                .transform([&](int repeat) {
                    return std::pair{std::move(utext), repeat};
                });
        });
  }

} // namespace echo_pipeline

// ======================================================
// Command registration
// ======================================================

REGISTER_COMMAND(echo,
                 /* name */
                 "echo",

                 /* synopsis */
                 "display a line of text",

                 /* description */
                 "Write arguments to the standard output.\n"
                 "\n"
                 "By default, echo prints the provided text once.\n"
                 "Options allow transforming the output.",

                 /* examples */
                 "  echo hello world\n"
                 "  echo --upper hello\n"
                 "  echo --repeat 3 hello",

                 /* see also */
                 "printf(1)",

                 /* author */
                 "WinuxCmd",

                 /* copyright */
                 "Copyright © 2026 WinuxCmd",

                 /* options */
                 ECHO_OPTIONS) {
  using namespace echo_pipeline;

  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"echo");
    return 1;
  }

  auto [text, repeat] = *result;
  std::wstring wtext(text.begin(), text.end());

  for (int i = 0; i < repeat; ++i) {
    safePrintLn(wtext);
  }

  return 0;
}

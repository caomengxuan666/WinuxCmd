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
}

// ======================================================
// Options (constexpr)
// ======================================================

export auto constexpr ECHO_OPTIONS = std::array{
    OPTION("-u", "--upper", "convert text to uppercase"),
    OPTION("-r", "--repeat", "repeat output N times", INT_TYPE)
};

// ======================================================
// Pipeline components
// ======================================================
namespace echo_pipeline {

    // ----------------------------------------------
    // 1. Text Builder (接受 const span)
    // ----------------------------------------------
    auto build_text(std::span<const std::string_view> positionals) -> std::string {
        std::string text;
        for (auto arg : positionals) {
            if (!text.empty())
                text += ' ';
            text += arg;
        }
        return text;
    }

    // ----------------------------------------------
    // 2. Uppercase Transformer
    // ----------------------------------------------
    auto to_uppercase(std::string text) -> std::string {
        for (char &c : text) {
            c = static_cast<char>(
                std::toupper(static_cast<unsigned char>(c))
            );
        }
        return text;
    }

    // ----------------------------------------------
    // 3. Repeat Validator
    // ----------------------------------------------
    auto validate_repeat(int count) -> int {
        return std::clamp(
            count,
            echo_constants::MIN_REPEAT,
            echo_constants::MAX_REPEAT
        );
    }

    // ----------------------------------------------
    // 4. Output Formatter
    // ----------------------------------------------
    auto to_wide_string(const std::string& text) -> std::wstring {
        return std::wstring(text.begin(), text.end());
    }

    // ----------------------------------------------
    // 5. Main Pipeline
    // ----------------------------------------------
    auto process_command(const CommandContext& ctx) -> std::pair<std::string, int> {
        // Build initial text
        std::string text = build_text(std::span<const std::string_view>(
            ctx.positionals.data(),
            ctx.positionals.size()
        ));

        // Apply transformations
        if (ctx.options.get<bool>("--upper", false)) {
            text = to_uppercase(std::move(text));
        }

        // Get and validate repeat count
        int repeat = validate_repeat(
            ctx.options.get<int>("--repeat", 1)
        );

        return {std::move(text), repeat};
    }
}

// ======================================================
// Command registration
// ======================================================

REGISTER_COMMAND(
    echo,
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
    ECHO_OPTIONS
) {
    using namespace echo_pipeline;

    // ==================================================
    // Execute pipeline
    // ==================================================
    auto [text, repeat] = process_command(ctx);
    std::wstring wtext = to_wide_string(text);

    // ==================================================
    // Output with repetition
    // ==================================================
    for (int i = 0; i < repeat; ++i) {
        safePrintLn(wtext);
    }

    return 0;
}
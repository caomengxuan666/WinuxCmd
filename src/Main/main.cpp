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
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: main.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
// src/main.cpp
// Main entry point for WinuxCmd
import std;
import core;
import utils;
import wildcard_handler;

/**
 * @brief Print help information
 * @return Exit code (1 - error)
 */
static int printHelp() noexcept {
    safePrintLn(L"WinuxCmd - Windows Compatible Linux Command Set");
    safePrintLn(L"Usage: winuxcmd <command> [options]...");
    safePrintLn(L"");
    safePrintLn(L"Available commands:");

    // Get all registered commands and display them with brief descriptions
    auto commands = CommandRegistry::getAllCommands();
    for (const auto &[cmd_name, cmd_desc]: commands) {
        // Display command name with its brief description
        std::wstring cmd_str = utf8_to_wstring(std::string(cmd_name));
        std::wstring desc_str = utf8_to_wstring(std::string(cmd_desc));
        // Pad command name for alignment
        if (cmd_str.length() < 10) {
            cmd_str.append(10 - cmd_str.length(), L' ');
        }
        safePrintLn(L"  " + cmd_str + L"   " + desc_str);
    }

    safePrintLn(L"");
    safePrintLn(L"Use 'winuxcmd <command> --help' for command-specific help.");
    return 1;
}

/**
 * @brief Main function for WinuxCmd
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit code from the executed command (0 = success, non-zero = error)
 */
int main(int argc, char *argv[]) noexcept {
    if (argc < 1) {
        return printHelp();
    }
    // Automatically set console or pipe output.
    setupConsoleForUnicode();

    // Get the executable name (stem only)
    std::filesystem::path self_path(argv[0]);
    std::string self_name = self_path.stem().string();

    // Convert command-line arguments to string_views for efficiency
    std::vector<std::string_view> raw_args(argv + 1, argv + argc);

    // Expand wildcards
    std::vector<std::string> expanded_args = expand_all_wildcards(raw_args);

    // Convert expanded args back to string_view for dispatch
    std::vector<std::string_view> args;
    args.reserve(expanded_args.size());
    for (const auto &arg: expanded_args) {
        args.emplace_back(arg);
    }

    if (self_name == "winuxcmd") {
        // Mode 1: winuxcmd <command> [args...] (e.g., winuxcmd ls -la)
        if (args.empty()) {
            return printHelp();
        }

        // Check for --help flag
        if (args.size() == 1 && args[0] == "--help") {
            return printHelp();
        }

        // Extract command name and remaining arguments
        const std::string_view cmd_name = args[0];
        const std::span<std::string_view> cmd_args(args.data() + 1,
                                                   args.size() - 1);

        // Dispatch the command to corresponding implementation
        return CommandRegistry::dispatch(cmd_name, cmd_args);
    } else {
        // Mode 2: <command>.exe [args...] (e.g., ls.exe -la)
        // Treat executable name as command name for direct calls
        const std::span<std::string_view> cmd_args(args.data(), args.size());

        // Dispatch the command to corresponding implementation
        return CommandRegistry::dispatch(self_name, cmd_args);
    }
}

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
import readline;

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
  for (const auto &[cmd_name, cmd_desc] : commands) {
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

static std::wstring buildReplPrompt() noexcept {
  try {
    auto cwd = std::filesystem::current_path().wstring();
    return L"winux " + cwd + L"> ";
  } catch (...) {
    return L"winux > ";
  }
}

static int runNativeFallback(const std::string &line) noexcept {
  if (line.empty()) return 0;
  std::wstring cmdline = L"cmd.exe /d /c " + utf8_to_wstring(line);

  STARTUPINFOW        si{};
  PROCESS_INFORMATION pi{};
  si.cb = sizeof(si);

  // CreateProcessW requires a mutable command buffer.
  std::vector<wchar_t> mutable_cmd(cmdline.begin(), cmdline.end());
  mutable_cmd.push_back(L'\0');

  BOOL ok = CreateProcessW(nullptr, mutable_cmd.data(), nullptr, nullptr, TRUE,
                           0, nullptr, nullptr, &si, &pi);
  if (!ok) {
    safePrintLn(L"winuxcmd: failed to run native command: " +
                utf8_to_wstring(line));
    return 127;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exit_code = 0;
  GetExitCodeProcess(pi.hProcess, &exit_code);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return static_cast<int>(exit_code);
}

// ─────────────────────────────────────────────────────────────────────────────
// Interactive REPL
// ─────────────────────────────────────────────────────────────────────────────

/// Build a completer function that suggests command names (prefix match) and
/// command options (after a space).
static Completer makeCompleter() {
  return [](std::string_view input) -> std::vector<CompletionItem> {
    // Find the first space to decide whether we're completing a command name
    // or an option for an already-typed command.
    auto space = input.find(' ');

    if (space == std::string_view::npos) {
      // ── Complete command name ──
      auto all = CommandRegistry::getAllCommands();
      std::vector<CompletionItem> items;
      for (auto &[name, desc] : all) {
        if (name.starts_with(input))
          items.push_back({std::string(name), std::string(desc)});
      }
      std::ranges::sort(items, {}, &CompletionItem::text);
      return items;
    }

    // ── Complete option for a known command ──
    std::string_view cmd_name  = input.substr(0, space);
    std::string_view opt_input = input.substr(space + 1);
    // Skip extra spaces
    while (!opt_input.empty() && opt_input.front() == ' ')
      opt_input.remove_prefix(1);

    auto opts = CommandRegistry::getCommandOptions(cmd_name);
    std::vector<CompletionItem> items;

    for (auto &opt : opts) {
      // Check both short and long forms
      for (const std::string *form : {&opt.short_name, &opt.long_name}) {
        if (form->empty()) continue;
        if (opt_input.empty() || form->starts_with(opt_input)) {
          std::string full = std::string(cmd_name) + " " + *form;
          items.push_back({std::move(full), opt.description});
        }
      }
    }
    return items;
  };
}

/// Run WinuxCmd in interactive REPL mode.
static void runReplMode() noexcept {
  safePrintLn(
      L"WinuxCmd " + utf8_to_wstring(std::string("0.4.5")) +
      L"  (interactive)  Type 'exit' to quit, '--help' for command list.");
  safePrintLn(L"Use \u2191\u2193 arrows or Tab to navigate completions.\n");

  auto completer = makeCompleter();

  while (true) {
    std::string line = readInteractiveLine(buildReplPrompt(), completer);

    // Ctrl+D sentinel
    if (!line.empty() && line[0] == '\x04') break;

    // Strip leading/trailing whitespace
    auto first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) continue;
    line = line.substr(first);
    auto last = line.find_last_not_of(" \t\r\n");
    if (last != std::string::npos) line = line.substr(0, last + 1);
    if (line.empty()) continue;

    if (line == "exit" || line == "quit") break;

    // Tokenise (simple whitespace split; no quoting yet)
    std::vector<std::string> tokens;
    {
      std::istringstream iss(line);
      std::string tok;
      while (iss >> tok) tokens.push_back(std::move(tok));
    }
    if (tokens.empty()) continue;

    std::vector<std::string_view> args;
    args.reserve(tokens.size());
    for (auto &t : tokens) args.emplace_back(t);

    std::string_view             cmd_name = args[0];
    std::span<std::string_view>  cmd_args(args.data() + 1, args.size() - 1);
    if (CommandRegistry::hasCommand(cmd_name)) {
      CommandRegistry::dispatch(cmd_name, cmd_args);
    } else {
      runNativeFallback(line);
    }
  }

  safePrintLn(L"\nGoodbye!");
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
  std::string self_name = path::get_executable_name(argv[0]);

  // Convert command-line arguments to string_views for efficiency
  // Note: We don't expand wildcards here anymore. Each command decides whether to expand.
  std::vector<std::string_view> args;
  args.reserve(argc - 1);
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  if (self_name == "winuxcmd") {
    // Mode 1: winuxcmd <command> [args...] (e.g., winuxcmd ls -la)
    if (args.empty()) {
      // Enter interactive REPL when running on a real console, otherwise help.
      HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
      DWORD  mode = 0;
      if (isOutputConsole() && GetConsoleMode(hIn, &mode)) {
        runReplMode();
        return 0;
      }
      return printHelp();
    }

    // ── Machine-readable completion query: winuxcmd --complete-cmd [prefix] ──
    if (args[0] == "--complete-cmd") {
      std::string_view prefix = args.size() >= 2 ? args[1] : "";
      auto all = CommandRegistry::getAllCommands();
      std::vector<std::pair<std::string_view, std::string_view>> sorted(all.begin(), all.end());
      std::ranges::sort(sorted, {}, [](const auto &p) { return p.first; });
      for (auto &[name, desc] : sorted) {
        if (name.starts_with(prefix)) {
          safePrint(name);
          safePrint("\t");
          safePrintLn(desc);
        }
      }
      return 0;
    }

    // ── Machine-readable option query: winuxcmd --complete-opt <cmd> [prefix] ──
    if (args[0] == "--complete-opt" && args.size() >= 2) {
      std::string_view cmd    = args[1];
      std::string_view prefix = args.size() >= 3 ? args[2] : "";
      auto opts = CommandRegistry::getCommandOptions(cmd);
      for (auto &opt : opts) {
        for (const std::string *form : {&opt.short_name, &opt.long_name}) {
          if (form->empty()) continue;
          if (prefix.empty() || std::string_view(*form).starts_with(prefix)) {
            safePrint(*form);
            safePrint("\t");
            safePrintLn(opt.description);
          }
        }
      }
      return 0;
    }

    // Check for --help flag
    if (args.size() == 1 && args[0] == "--help") {
      return printHelp();
    }

    // Extract command name and remaining arguments
    const std::string_view cmd_name = args[0];
    const std::span<std::string_view> cmd_args(args.data() + 1,
                                               args.size() - 1);

    if (!CommandRegistry::hasCommand(cmd_name)) {
      std::string raw_line;
      raw_line.reserve(64);
      for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) raw_line.push_back(' ');
        raw_line.append(args[i]);
      }
      return runNativeFallback(raw_line);
    }

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

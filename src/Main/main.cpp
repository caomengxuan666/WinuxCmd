/*
 *  Copyright (c) 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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
#include <windows.h>

import std;
import core;
import utils;
import version;

namespace {
static void applyInheritedStdbuf(const char* name, FILE* stream) {
  const char* mode = std::getenv(name);
  if (mode == nullptr || *mode == '\0') return;
  if (std::strcmp(mode, "0") == 0) {
    setvbuf(stream, nullptr, _IONBF, 0);
    return;
  }
  if (std::strcmp(mode, "L") == 0) {
    // MSVC's CRT does not support POSIX line buffering reliably. Accept the
    // inherited GNU-compatible marker without mutating the stream.
    return;
  }
  char* end = nullptr;
  const auto size = std::strtoull(mode, &end, 10);
  if (end != mode && *end == '\0' && size > 0 &&
      size <= std::numeric_limits<size_t>::max()) {
    setvbuf(stream, nullptr, _IOFBF, static_cast<size_t>(size));
  }
}

static std::string toLowerAscii(std::string s) {
  std::ranges::transform(s, s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return s;
}

static void printCommandSummary(std::string_view name, std::string_view desc,
                                std::string_view command_style, bool color) {
  constexpr size_t command_width = 12;
  std::string command(name);
  if (command.size() < command_width) {
    command.append(command_width - command.size(), ' ');
  }

  const std::string first_prefix =
      "  " + (color ? colorizeStdout(command, command_style) : command) + " ";
  const std::string continuation_prefix(2 + command_width + 1, ' ');

  const auto translated_desc = winux::i18n::translate(
      "command." + std::string(name) + ".synopsis", desc);
  desc = translated_desc;
  bool first_line = true;
  while (true) {
    size_t newline_pos = desc.find('\n');
    std::string_view line = newline_pos == std::string_view::npos
                                ? desc
                                : desc.substr(0, newline_pos);
    safePrintLn((first_line ? first_prefix : continuation_prefix) +
                std::string(line));
    first_line = false;

    if (newline_pos == std::string_view::npos) break;
    desc = desc.substr(newline_pos + 1);
  }
}

// Shim-layout packages (installed via `wpm install` with artifact
// layout=shim) keep their payload self-contained under <root>\opt\<pkg>\.
// The command entry in usr\bin is a plain hardlink of winuxcmd.exe; when the
// invoked name is not a registered command, we forward to
// <root>\opt\<name>\<name>.exe so the payload's private DLLs resolve from its
// own directory. Returns nullopt when no payload exists (caller falls back to
// the regular command-not-found path).
static std::optional<int> forwardToOptPayload(std::string_view name) noexcept {
  namespace fs = std::filesystem;
  wchar_t buffer[MAX_PATH];
  const DWORD size = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
  if (size == 0 || size >= MAX_PATH) return std::nullopt;
  const fs::path self(buffer);
  // <root>\usr\bin\<self>.exe -> <root>
  const fs::path root = self.parent_path().parent_path().parent_path();

  const fs::path payload_direct =
      root / L"opt" / utf8_to_wstring(std::string(name)) /
      (utf8_to_wstring(std::string(name)) + L".exe");
  std::error_code ec;
  fs::path payload = payload_direct;
  if (!fs::is_regular_file(payload, ec)) {
    // Package dir may differ from the command name (e.g. opt\sysinternals-suite\
    // provides accesschk.exe): scan one level of opt\ for a matching payload.
    const fs::path opt_dir = root / L"opt";
    if (!fs::exists(opt_dir, ec)) return std::nullopt;
    bool found = false;
    for (const auto& entry : fs::directory_iterator(opt_dir, ec)) {
      if (ec) break;
      if (!entry.is_directory(ec)) continue;
      const fs::path candidate =
          entry.path() / (utf8_to_wstring(std::string(name)) + L".exe");
      std::error_code file_ec;
      if (fs::is_regular_file(candidate, file_ec)) {
        payload = candidate;
        found = true;
        break;
      }
    }
    if (!found) return std::nullopt;
  }

  // Rebuild the child command line from the raw one so the original quoting
  // of every argument is preserved verbatim.
  const std::wstring raw = GetCommandLineW();
  std::wstring rest;
  if (!raw.empty()) {
    size_t start = std::wstring::npos;
    if (raw[0] == L'"') {
      const size_t closing = raw.find(L'"', 1);
      if (closing != std::wstring::npos)
        start = raw.find_first_not_of(L' ', closing + 1);
    } else {
      const size_t space = raw.find_first_of(L" \t");
      if (space != std::wstring::npos)
        start = raw.find_first_not_of(L" \t", space);
    }
    if (start != std::wstring::npos) rest = raw.substr(start);
  }
  std::wstring command_line = L"\"" + payload.wstring() + L"\"";
  if (!rest.empty()) command_line += L" " + rest;

  STARTUPINFOW si{};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};
  if (!CreateProcessW(payload.c_str(), command_line.data(), nullptr, nullptr,
                      TRUE, 0, nullptr, nullptr, &si, &pi)) {
    safeErrorPrintLn(winux::i18n::format(
        "main.error.shim_launch_failed",
        "winuxcmd: failed to launch shim payload '{}': error {}",
        std::string(name), static_cast<unsigned long>(GetLastError())));
    return 126;
  }
  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exit_code = 1;
  GetExitCodeProcess(pi.hProcess, &exit_code);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return static_cast<int>(exit_code);
}

}  // namespace

/**
 * @brief Print help information
 * @return Exit code (1 - error)
 */
static int printHelp() noexcept {
  const bool color = shouldUseAnsiColorStdout();
  const std::string title_style =
      std::string(ANSI_BOLD) + ansiFgRgb(98, 214, 255);
  const std::string section_style =
      std::string(ANSI_BOLD) + ANSI_UNDERLINE + ansiFg256(82);
  const std::string command_style = std::string(ANSI_BOLD) + ansiFg256(117);
  const std::string subtle_style = ansiFg256(245);

  const auto subtitle = winux::i18n::translate(
      "main.subtitle", "Windows Compatible Linux Command Set");
  const auto usage = winux::i18n::translate("common.usage", "Usage:");
  safePrintLn(color ? colorizeStdout("WinuxCmd", title_style) + " - " + subtitle
                    : "WinuxCmd - " + subtitle);
  safePrintLn(color ? colorizeStdout(usage, section_style) +
                          " winuxcmd <command> [options]..."
                    : usage + " winuxcmd <command> [options]...");
  safePrintLn("");
  const auto available =
      winux::i18n::translate("main.available_commands", "Available Commands:");
  safePrintLn(color ? colorizeStdout(available, section_style) : available);

  // Get all registered commands and display them with brief descriptions
  auto commands = CommandRegistry::getAllCommands();
  for (const auto& [cmd_name, cmd_desc] : commands) {
    printCommandSummary(cmd_name, cmd_desc, command_style, color);
  }

  safePrintLn("");
  const auto tip = winux::i18n::translate(
      "main.help_tip",
      "Tip: Use 'winuxcmd <command> --help' for command-specific help.");
  safePrintLn(color ? colorizeStdout("Tip:", subtle_style) + " " + tip : tip);
  return 1;
}

/**
 * @brief Main function for WinuxCmd
 * @param argc Number of command-line arguments
 * @param argv Array of command-line arguments
 * @return Exit code from the executed command (0 = success, non-zero = error)
 */
int main(int argc, char* argv[]) noexcept {
  if (argc < 1) {
    return printHelp();
  }
  // Automatically set console or pipe output.
  setupConsoleForUnicode();
  applyInheritedStdbuf("WINUX_STDBUF_I", stdin);
  applyInheritedStdbuf("WINUX_STDBUF_O", stdout);
  applyInheritedStdbuf("WINUX_STDBUF_E", stderr);
  // Get the executable name (stem only)
  std::string self_name = path::get_executable_name(argv[0]);

  // Convert command-line arguments to string_views for efficiency
  std::vector<std::string_view> args;
  args.reserve(argc - 1);
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  if (self_name == "winuxcmd") {
    // Mode 1: winuxcmd <command> [args...] (e.g., winuxcmd ls -la)
    if (args.empty()) {
      return printHelp();
    }

    // Check for top-level help flag/alias
    if (args.size() == 1 && args[0] == "--help") {
      return printHelp();
    }

    // Check for version flags
    if (args.size() == 1 && (args[0] == "--version" || args[0] == "-v")) {
      safePrintLn(L"WinuxCmd " + utf8_to_wstring(WinuxCmd::VERSION_STRING));
      return 0;
    }

    if (!args.empty() && args[0] == "help") {
      if (args.size() == 1) return printHelp();
      if (args.size() == 2) {
        std::string topic(args[1]);
        if (CommandRegistry::hasCommand(topic)) {
          CommandRegistry::printHelp(topic);
          return 0;
        }
        std::string lowered = toLowerAscii(topic);
        if (CommandRegistry::hasCommand(lowered)) {
          CommandRegistry::printHelp(lowered);
          return 0;
        }
        safeErrorPrintLn(winux::i18n::format("main.error.no_help_topic",
                                             "winuxcmd: no help topic for '{}'",
                                             topic));
        return 1;
      }
      safeErrorPrintLn(winux::i18n::translate(
          "main.error.help_too_many_topics",
          "winuxcmd: help accepts at most one command name"));
      return 1;
    }

    // Extract command name and remaining arguments

    const std::string_view cmd_name = args[0];

    const std::span<std::string_view> cmd_args(args.data() + 1,

                                               args.size() - 1);

    // Check for --version in command arguments
    bool has_version = false;
    for (const auto& arg : cmd_args) {
      if (arg == "--version") {
        has_version = true;
        break;
      }
    }

    if (has_version && CommandRegistry::hasCommand(cmd_name)) {
      safePrintLn(L"WinuxCmd " + utf8_to_wstring(WinuxCmd::VERSION_STRING));
      return 0;
    }

    if (!CommandRegistry::hasCommand(cmd_name)) {
      // Not a builtin: try forwarding to a shim-layout payload first.
      if (auto forwarded = forwardToOptPayload(cmd_name)) {
        return *forwarded;
      }
      safeErrorPrintLn(winux::i18n::format("core.error.command_not_found",
                                           "winuxcmd: command not found: {}",
                                           cmd_name));
      return 127;
    }

    // Direct execution
    // Dispatch the command to corresponding implementation
    return CommandRegistry::dispatch(cmd_name, cmd_args);
  } else {
    // Mode 2: <command>.exe [args...] (e.g., ls.exe -la)
    // Treat executable name as command name for direct calls
    const std::span<std::string_view> cmd_args(args.data(), args.size());

    // Not a builtin: try forwarding to a shim-layout payload first.
    if (!CommandRegistry::hasCommand(self_name)) {
      if (auto forwarded = forwardToOptPayload(self_name)) {
        return *forwarded;
      }
    }

    // Dispatch the command to corresponding implementation
    return CommandRegistry::dispatch(self_name, cmd_args);
  }
}

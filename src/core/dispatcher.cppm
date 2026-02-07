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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: dispatcher.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
export module core:dispatcher;

import std;
import :cmd_meta;
import :command_context;
import utils;

export template <size_t N>
using CommandFunc = int (*)(CommandContext<N> &) noexcept;

export template <size_t N>
struct CommandEntry {
  cmd::meta::CommandMetaHandle meta;
  CommandFunc<N> handler;
  std::string_view brief_desc;

  CommandEntry() = default;
  CommandEntry(cmd::meta::CommandMetaHandle m, CommandFunc<N> h)
      : meta(std::move(m)), handler(h), brief_desc(meta.brief_desc()) {}
};

struct CommandEntryErased {
  cmd::meta::CommandMetaHandle meta;
  std::function<int(std::span<std::string_view>)> handler;
  std::string_view brief_desc;

  CommandEntryErased() = default;

  CommandEntryErased(cmd::meta::CommandMetaHandle m,
                     std::function<int(std::span<std::string_view>)> h,
                     std::string_view brief)
      : meta(std::move(m)), handler(std::move(h)), brief_desc(brief) {}
};

// Internal registry implementation class
class RegistryImpl {
  std::unordered_map<std::string_view, CommandEntryErased> registry_;

 public:
  // Register a command with compile-time metadata
  template <size_t N>
  void add(std::string_view name, const cmd::meta::CommandMeta<N> &meta,
           CommandFunc<N> handler) {
    // compile-time meta registry
    cmd::meta::Registry::register_command(name, meta);

    // type erase
    auto lambda = [meta, handler](std::span<std::string_view> args) -> int {
      bool ok = true;
      auto ctx = make_context<N>(args, meta.options(), ok);
      if (!ok) return 1;
      return handler(ctx);
    };

    registry_.emplace(
        name, CommandEntryErased{cmd::meta::CommandMetaHandle(meta), lambda,
                                 meta.brief_desc()});
  }

  // Dispatch command execution
  int run(std::string_view cmdName, std::span<std::string_view> args) {
    auto it = registry_.find(cmdName);
    if (it == registry_.end()) {
      safePrintLn(L"winuxcmd: command not found: " +
                  std::wstring(cmdName.begin(), cmdName.end()));
      return 127;  // Command not found exit code
    }

    // Check for help flag
    for (const auto &arg : args) {
      if (arg == "--help" || arg == "-h") {
        cmd::meta::Registry::print_help(cmdName);
        return 0;
      }
    }

    // Execute command handler
    return it->second.handler(args);
  }

  // Print command help
  void help(std::string_view cmdName) {
    cmd::meta::Registry::print_help(cmdName);
  }

  // Get all registered command names
  std::vector<std::pair<std::string_view, std::string_view>> list() {
    std::vector<std::pair<std::string_view, std::string_view>> commands;
    commands.reserve(registry_.size());
    for (const auto &[name, entry] : registry_) {
      commands.emplace_back(name, entry.brief_desc);
    }
    return commands;
  }
};

// Get singleton instance
inline RegistryImpl &getImpl() {
  static RegistryImpl instance;
  return instance;
}

// Static interface class
export class CommandRegistry {
 public:
  // Template method for registering commands with compile-time metadata
  template <size_t N>
  static void registerCommand(std::string_view name,
                              const cmd::meta::CommandMeta<N> &meta,
                              CommandFunc<N> handler) {
    getImpl().add<N>(name, meta, handler);
  }

  // Dispatch command execution (public interface)
  static int dispatch(std::string_view cmdName,
                      std::span<std::string_view> args) noexcept {
    try {
      return getImpl().run(cmdName, args);
    } catch (const std::exception &e) {
      safePrintLn(L"winuxcmd: dispatch error: " +
                  std::wstring(e.what(), e.what() + std::strlen(e.what())));
      return 1;
    } catch (...) {
      safePrintLn(L"winuxcmd: unknown dispatch error");
      return 1;
    }
  }

  // Print command help (public interface)
  static void printHelp(std::string_view cmdName) noexcept {
    try {
      getImpl().help(cmdName);
    } catch (const std::exception &e) {
      safePrintLn(L"winuxcmd: help error: " +
                  std::wstring(e.what(), e.what() + std::strlen(e.what())));
    } catch (...) {
      safePrintLn(L"winuxcmd: unknown help error");
    }
  }

  // Get all registered command names (public interface)
  static std::vector<std::pair<std::string_view, std::string_view>>
  getAllCommands() noexcept {
    try {
      return getImpl().list();
    } catch (const std::exception &e) {
      safePrintLn(L"winuxcmd: getAllCommands error: " +
                  std::wstring(e.what(), e.what() + std::strlen(e.what())));
      return {};
    } catch (...) {
      safePrintLn(L"winuxcmd: unknown getAllCommands error");
      return {};
    }
  }
};

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

#define MY_FUCK_DEBUG wprintf(L"__DISPATCH__:%d\n", __LINE__)

export using CommandFunc = int (*)(CommandContext&) noexcept;

export class CommandRegistry {
 private:
  // Internal data structure for command entry
  struct CommandEntry {
    cmd::meta::CommandMetaHandle meta;
    CommandFunc handler;
    std::string_view brief_desc;  // Brief description for help listing

    CommandEntry() = default;

    // Constructor with parameters
    CommandEntry(cmd::meta::CommandMetaHandle m, CommandFunc h)
        : meta(std::move(m)), handler(h), brief_desc(meta.brief_desc()) {}
  };

  // Internal registry implementation class
  class RegistryImpl {
    std::unordered_map<std::string_view, CommandEntry> registry_;

   public:
    // Register a command with compile-time metadata
    template <size_t N>
    void registerCommand(
        std::string_view name,
        const cmd::meta::CommandMeta<N>& meta,
        CommandFunc handler)
    {
      cmd::meta::Registry::register_command(name, meta);

      registry_[name] =
          CommandEntry(cmd::meta::CommandMetaHandle(meta), handler);
    }

    // Dispatch command execution
    int dispatch(std::string_view cmdName, std::span<std::string_view> args) {
      auto it = registry_.find(cmdName);
      if (it == registry_.end()) {
        safePrintLn(L"winuxcmd: command not found: " + std::wstring(cmdName.begin(), cmdName.end()));
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
      auto& entry = it->second;

      bool ok = true;

      auto ctx = make_context(
          args,
          entry.meta.options(),
          ok
      );
      if (!ok) {
        safePrintLn(std::wstring(cmdName.begin(), cmdName.end()) + L": invalid arguments");
        return 1;
      }

      return entry.handler(ctx);
    }

    // Print command help
    void printHelp(std::string_view cmdName) {
      cmd::meta::Registry::print_help(cmdName);
    }

    // Get all registered command names
    std::vector<std::pair<std::string_view, std::string_view>>
    getAllCommands() {
      std::vector<std::pair<std::string_view, std::string_view>> commands;
      commands.reserve(registry_.size());
      for (const auto &[name, entry] : registry_) {
        commands.push_back({name, entry.brief_desc});
      }
      return commands;
    }
  };

  // Get singleton instance
  static RegistryImpl &getImpl() {
    static RegistryImpl instance;
    return instance;
  }

 public:
  // Template method for registering commands with compile-time metadata
  // Must be defined in the interface due to template nature
  template <size_t N>
  static void registerCommand(
      std::string_view name,
      const cmd::meta::CommandMeta<N>& meta,
      CommandFunc handler) noexcept
  {
    getImpl().registerCommand(name, meta, handler);
  }

  // Dispatch command execution (public interface)
  static int dispatch(std::string_view cmdName,
                      std::span<std::string_view> args) noexcept {
    try {

      return getImpl().dispatch(cmdName, args);
    } catch (const std::exception &e) {
      safePrintLn(L"winuxcmd: dispatch error: " + std::wstring(e.what(), e.what() + std::strlen(e.what())));
      return 1;
    } catch (...) {
      safePrintLn(L"winuxcmd: unknown dispatch error");
      return 1;
    }
  }

  // Print command help (public interface)
  static void printHelp(std::string_view cmdName) noexcept {
    try {
      getImpl().printHelp(cmdName);
    } catch (const std::exception &e) {
      safePrintLn(L"winuxcmd: help error: " + std::wstring(e.what(), e.what() + std::strlen(e.what())));
    } catch (...) {
      safePrintLn(L"winuxcmd: unknown help error");
    }
  }

  // Get all registered command names (public interface)
  static std::vector<std::pair<std::string_view, std::string_view>>
  getAllCommands() noexcept {
    try {
      return getImpl().getAllCommands();
    } catch (const std::exception &e) {
      safePrintLn(L"winuxcmd: getAllCommands error: " + std::wstring(e.what(), e.what() + std::strlen(e.what())));
      return {};
    } catch (...) {
      safePrintLn(L"winuxcmd: unknown getAllCommands error");
      return {};
    }
  }
};

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
import version;

export template <size_t N>
using CommandFunc = int (*)(CommandContext<N> &) noexcept;

export using CommandInvoker = int (*)(std::span<std::string_view>) noexcept;

export template <size_t N>
struct CommandEntry {
  cmd::meta::CommandMetaHandle meta;
  CommandFunc<N> handler;
  std::string_view brief_desc;

  CommandEntry() = default;
  CommandEntry(cmd::meta::CommandMetaHandle m, CommandFunc<N> h)
      : meta(std::move(m)), handler(h), brief_desc(meta.brief_desc()) {}
};

// Per-option information returned for completion
export struct OptionInfo {
  std::string short_name;
  std::string long_name;
  std::string description;
};

struct CommandEntryErased {
  std::span<const cmd::meta::OptionMeta> options;
  CommandInvoker handler = nullptr;
  std::string_view brief_desc;

  CommandEntryErased() = default;

  CommandEntryErased(std::span<const cmd::meta::OptionMeta> opts,
                     CommandInvoker h, std::string_view brief)
      : options(opts), handler(h), brief_desc(brief) {}
};

using ArgsRewriteHook =
    std::optional<std::vector<std::string>> (*)(std::span<std::string_view>);
using ArgsValidationHook =
    std::optional<std::string> (*)(std::span<std::string_view>);
using SpecialDispatchHook = std::optional<int> (*)(const CommandEntryErased &,
                                                   std::span<std::string_view>);
using StandardInterceptionHook = bool (*)(std::span<std::string_view>);

constexpr size_t kMaxRewriteHooks = 5;

struct CommandBehavior {
  int parse_error_exit_code = 1;
  ArgsValidationHook validate_args = nullptr;
  std::string_view validation_prefix;
  std::array<ArgsRewriteHook, kMaxRewriteHooks> rewrite_hooks{};
  size_t rewrite_hook_count = 0;
  SpecialDispatchHook special_dispatch = nullptr;
  StandardInterceptionHook standard_interception_enabled = nullptr;
};

auto is_posixly_correct() -> bool {
  const char *value = std::getenv("POSIXLY_CORRECT");
  return value != nullptr && value[0] != '\0';
}

auto legacy_count_value(std::string_view arg) -> std::string {
  if (arg.empty()) return {};
  if ((arg[0] == '-' || arg[0] == '+') && arg.size() > 1) {
    return std::string(arg.substr(1));
  }
  return std::string(arg);
}

auto append_remaining_args(std::vector<std::string> &out,
                           std::span<std::string_view> args, size_t start)
    -> void {
  for (size_t i = start; i < args.size(); ++i) {
    out.emplace_back(args[i]);
  }
}

auto parse_decimal_prefix(std::string_view arg, size_t start) -> size_t {
  size_t pos = start;
  while (pos < arg.size() &&
         std::isdigit(static_cast<unsigned char>(arg[pos])) != 0) {
    ++pos;
  }
  return pos;
}

auto rewrite_head_obsolete_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  if (args.empty()) return std::nullopt;
  std::string_view first = args[0];
  if (first.size() < 2 || first[0] != '-' || first[1] == '-')
    return std::nullopt;

  size_t suffix_pos = parse_decimal_prefix(first, 1);
  if (suffix_pos == 1) return std::nullopt;

  std::string count(first.substr(1, suffix_pos - 1));
  std::string mode = "-n";
  std::string value = count;
  std::vector<std::string> flags;

  for (size_t i = suffix_pos; i < first.size(); ++i) {
    switch (first[i]) {
      case 'l':
        mode = "-n";
        value = count;
        break;
      case 'c':
        mode = "-c";
        value = count;
        break;
      case 'b':
        mode = "-c";
        value = count + "b";
        break;
      case 'k':
        mode = "-c";
        value = count + "K";
        break;
      case 'm':
        mode = "-c";
        value = count + "M";
        break;
      case 'q':
        flags.emplace_back("-q");
        break;
      case 'v':
        flags.emplace_back("-v");
        break;
      default:
        return std::nullopt;
    }
  }

  std::vector<std::string> rewritten;
  rewritten.reserve(args.size() + flags.size() + 1);
  rewritten.emplace_back(mode);
  rewritten.emplace_back(value);
  for (const auto &flag : flags) rewritten.emplace_back(flag);
  append_remaining_args(rewritten, args, 1);
  return rewritten;
}

auto rewrite_tail_obsolete_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  if (args.empty()) return std::nullopt;
  std::string_view first = args[0];

  if (first.size() >= 2 && first[0] == '+' && first[1] != '+') {
    size_t suffix_pos = parse_decimal_prefix(first, 1);
    if (suffix_pos == 1) return std::nullopt;

    std::string count(first.substr(1, suffix_pos - 1));
    std::string mode = "-n";
    std::string value = "+" + count;

    for (size_t i = suffix_pos; i < first.size(); ++i) {
      switch (first[i]) {
        case 'l':
          mode = "-n";
          value = "+" + count;
          break;
        case 'c':
          mode = "-c";
          value = "+" + count;
          break;
        case 'b':
          mode = "-c";
          value = "+" + count + "b";
          break;
        default:
          return std::nullopt;
      }
    }

    std::vector<std::string> rewritten;
    rewritten.reserve(args.size() + 1);
    rewritten.emplace_back(mode);
    rewritten.emplace_back(value);
    append_remaining_args(rewritten, args, 1);
    return rewritten;
  }

  if (first.size() < 2 || first[0] != '-' || first[1] == '-')
    return std::nullopt;

  size_t suffix_pos = parse_decimal_prefix(first, 1);
  if (suffix_pos == 1) return std::nullopt;

  bool has_compact_suffix = suffix_pos < first.size();
  if (has_compact_suffix && args.size() > 2) return std::nullopt;

  std::string count(first.substr(1, suffix_pos - 1));
  std::string mode = "-n";
  std::string value = count;
  bool follow = false;

  for (size_t i = suffix_pos; i < first.size(); ++i) {
    switch (first[i]) {
      case 'l':
        mode = "-n";
        value = count;
        break;
      case 'c':
        mode = "-c";
        value = count;
        break;
      case 'b':
        mode = "-c";
        value = count + "b";
        break;
      case 'f':
        follow = true;
        break;
      default:
        return std::nullopt;
    }
  }

  std::vector<std::string> rewritten;
  rewritten.reserve(args.size() + 2);
  rewritten.emplace_back(mode);
  rewritten.emplace_back(value);
  if (follow) rewritten.emplace_back("-f");
  append_remaining_args(rewritten, args, 1);
  return rewritten;
}

auto is_head_obsolete_count_arg(std::string_view arg) -> bool {
  if (arg.size() < 2 || arg[0] != '-' || arg[1] == '-') return false;

  size_t suffix_pos = parse_decimal_prefix(arg, 1);
  if (suffix_pos == 1) return false;

  for (size_t i = suffix_pos; i < arg.size(); ++i) {
    switch (arg[i]) {
      case 'l':
      case 'c':
      case 'b':
      case 'k':
      case 'm':
      case 'q':
      case 'v':
        break;
      default:
        return false;
    }
  }

  return true;
}

auto is_tail_obsolete_count_arg(std::string_view arg) -> bool {
  if (arg.size() < 2) return false;

  if (arg[0] == '+') {
    if (arg[1] == '+') return false;

    size_t suffix_pos = parse_decimal_prefix(arg, 1);
    if (suffix_pos == 1) return false;
    for (size_t i = suffix_pos; i < arg.size(); ++i) {
      if (arg[i] != 'l' && arg[i] != 'c' && arg[i] != 'b') return false;
    }
    return true;
  }

  if (arg[0] != '-' || arg[1] == '-') return false;
  size_t suffix_pos = parse_decimal_prefix(arg, 1);
  if (suffix_pos == 1) return false;
  for (size_t i = suffix_pos; i < arg.size(); ++i) {
    if (arg[i] != 'l' && arg[i] != 'c' && arg[i] != 'b' && arg[i] != 'f') {
      return false;
    }
  }
  return true;
}

auto tail_invalid_obsolete_count_context(std::span<std::string_view> args)
    -> std::optional<std::string> {
  if (args.size() < 2) return std::nullopt;

  bool stop_option_parsing = false;
  bool expect_value = false;

  for (size_t i = 0; i < args.size(); ++i) {
    std::string_view arg = args[i];

    if (stop_option_parsing) continue;
    if (arg == "--") {
      stop_option_parsing = true;
      continue;
    }
    if (expect_value) {
      expect_value = false;
      continue;
    }

    if (i > 0 && is_tail_obsolete_count_arg(arg)) {
      return legacy_count_value(arg);
    }

    if (arg == "-c" || arg == "--bytes" || arg == "-n" || arg == "--lines" ||
        arg == "-s" || arg == "--sleep-interval" ||
        arg == "--max-unchanged-stats" || arg == "--pid") {
      expect_value = true;
      continue;
    }

    if (arg == "--follow" && i + 1 < args.size() && args[i + 1] != "--" &&
        !args[i + 1].empty() && args[i + 1][0] != '-') {
      expect_value = true;
    }
  }

  return std::nullopt;
}

auto head_invalid_obsolete_count_context(std::span<std::string_view> args)
    -> std::optional<std::string> {
  if (args.size() < 2) return std::nullopt;

  bool stop_option_parsing = false;
  bool expect_value = false;

  for (size_t i = 0; i < args.size(); ++i) {
    std::string_view arg = args[i];

    if (stop_option_parsing) continue;
    if (arg == "--") {
      stop_option_parsing = true;
      continue;
    }
    if (expect_value) {
      expect_value = false;
      continue;
    }

    if (i > 0 && is_head_obsolete_count_arg(arg)) {
      return legacy_count_value(arg);
    }

    if (arg == "-c" || arg == "--bytes" || arg == "-n" || arg == "--lines") {
      expect_value = true;
    }
  }

  return std::nullopt;
}

auto is_chmod_negative_symbolic_mode_arg(std::string_view arg) -> bool {
  if (arg.size() < 2 || arg[0] != '-' || arg[1] == '-') {
    return false;
  }

  return std::ranges::all_of(arg.substr(1), [](unsigned char ch) {
    return ch == 'r' || ch == 'w' || ch == 'x';
  });
}

auto rewrite_chmod_gnu_negative_mode_args(std::string_view cmdName,
                                          std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  if (cmdName != "chmod" || args.empty()) {
    return std::nullopt;
  }

  std::vector<std::string> negative_modes;
  std::vector<std::string> pre_double_hyphen_args;
  std::vector<std::string> post_double_hyphen_args;
  bool seen_double_hyphen = false;

  for (auto arg : args) {
    if (!seen_double_hyphen && arg == "--") {
      seen_double_hyphen = true;
      continue;
    }

    if (!seen_double_hyphen && is_chmod_negative_symbolic_mode_arg(arg)) {
      negative_modes.push_back("a" + std::string(arg));
      continue;
    }

    if (seen_double_hyphen) {
      post_double_hyphen_args.emplace_back(arg);
    } else {
      pre_double_hyphen_args.emplace_back(arg);
    }
  }

  if (negative_modes.empty()) {
    return std::nullopt;
  }

  std::vector<std::string> rewritten;
  rewritten.reserve(args.size() + 1);

  std::string merged_mode;
  for (size_t i = 0; i < negative_modes.size(); ++i) {
    if (i != 0) {
      merged_mode.push_back(',');
    }
    merged_mode += negative_modes[i];
  }

  rewritten.push_back(std::move(merged_mode));
  for (const auto &arg : pre_double_hyphen_args) {
    rewritten.push_back(arg);
  }
  if (seen_double_hyphen) {
    rewritten.push_back("--");
    for (const auto &arg : post_double_hyphen_args) {
      rewritten.push_back(arg);
    }
  }

  return rewritten;
}

auto rewrite_chmod_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  return rewrite_chmod_gnu_negative_mode_args("chmod", args);
}

auto is_chattr_mode_arg(std::string_view arg) -> bool {
  if (arg.size() < 2) return false;
  if (arg[0] != '+' && arg[0] != '-' && arg[0] != '=') return false;
  if (arg == "-R" || arg == "-V") return false;
  if (arg.starts_with("--")) return false;

  return std::ranges::all_of(arg.substr(1), [](unsigned char ch) {
    switch (ch) {
      case 'R':
      case 'r':
      case 'H':
      case 'h':
      case 'S':
      case 's':
      case 'A':
      case 'a':
      case 'I':
      case 'i':
      case 'T':
      case 't':
      case 'O':
      case 'o':
        return true;
      default:
        return false;
    }
  });
}

auto rewrite_chattr_mode_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  bool seen_double_hyphen = false;
  for (size_t i = 0; i < args.size(); ++i) {
    auto arg = args[i];
    if (arg == "--") {
      seen_double_hyphen = true;
      continue;
    }
    if (seen_double_hyphen || !is_chattr_mode_arg(arg)) continue;

    std::vector<std::string> rewritten;
    rewritten.reserve(args.size() + 1);
    for (size_t j = 0; j < i; ++j) rewritten.emplace_back(args[j]);
    rewritten.emplace_back("--");
    append_remaining_args(rewritten, args, i);
    return rewritten;
  }
  return std::nullopt;
}

auto rewrite_chattr_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  return rewrite_chattr_mode_args(args);
}

auto parse_legacy_nice_adjustment(std::string_view arg) -> std::optional<int> {
  if (arg.size() < 2 || arg[0] != '-') {
    return std::nullopt;
  }

  if (arg == "-n" || arg == "--adjustment") {
    return std::nullopt;
  }

  std::string_view numeric = arg.substr(1);
  if (numeric.empty() || numeric == "-" || numeric == "+") {
    return std::nullopt;
  }

  if (numeric[0] == '+') {
    numeric.remove_prefix(1);
    if (numeric.empty()) {
      return std::nullopt;
    }
  }

  int value = 0;
  auto [ptr, ec] =
      std::from_chars(numeric.data(), numeric.data() + numeric.size(), value);
  if (ec != std::errc() || ptr != numeric.data() + numeric.size()) {
    return std::nullopt;
  }

  return value;
}

auto rewrite_nice_legacy_args(std::string_view cmdName,
                              std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  if (cmdName != "nice" || args.empty()) {
    return std::nullopt;
  }

  std::vector<std::string> rewritten;
  rewritten.reserve(args.size());
  bool changed = false;
  bool seen_command = false;
  bool expecting_adjustment_value = false;

  for (auto arg : args) {
    if (seen_command) {
      rewritten.emplace_back(arg);
      continue;
    }

    if (arg == "--") {
      rewritten.emplace_back(arg);
      seen_command = true;
      expecting_adjustment_value = false;
      continue;
    }

    if (expecting_adjustment_value) {
      rewritten.emplace_back(arg);
      expecting_adjustment_value = false;
      continue;
    }

    if (arg == "-n" || arg == "--adjustment") {
      rewritten.emplace_back(arg);
      expecting_adjustment_value = true;
      continue;
    }

    if (arg.starts_with("-n") || arg.starts_with("--adjustment=")) {
      rewritten.emplace_back(arg);
      continue;
    }

    if (auto legacy_value = parse_legacy_nice_adjustment(arg)) {
      rewritten.emplace_back("-n" + std::to_string(*legacy_value));
      changed = true;
      continue;
    }

    if (!arg.empty() && arg[0] != '-') {
      rewritten.emplace_back("--");
      rewritten.emplace_back(arg);
      seen_command = true;
      changed = true;
      continue;
    }

    rewritten.emplace_back(arg);
  }

  if (!changed) {
    return std::nullopt;
  }

  return rewritten;
}

auto rewrite_nice_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  return rewrite_nice_legacy_args("nice", args);
}

auto rewrite_pr_legacy_column_args(std::string_view cmdName,
                                   std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  if (cmdName != "pr" || args.empty()) {
    return std::nullopt;
  }

  std::vector<std::string> rewritten;
  rewritten.reserve(args.size() + 2);
  bool changed = false;
  bool stop_option_parsing = false;

  for (auto arg : args) {
    if (!stop_option_parsing && arg == "--") {
      stop_option_parsing = true;
      rewritten.emplace_back(arg);
      continue;
    }

    if (!stop_option_parsing && arg.size() >= 2 && arg[0] == '-' &&
        arg[1] != '-') {
      size_t suffix_pos = parse_decimal_prefix(arg, 1);
      if (suffix_pos == arg.size() && suffix_pos > 1) {
        rewritten.emplace_back("-COLUMN");
        rewritten.emplace_back(std::string(arg.substr(1)));
        changed = true;
        continue;
      }
    }

    rewritten.emplace_back(arg);
  }

  if (!changed) {
    return std::nullopt;
  }

  return rewritten;
}

auto rewrite_pr_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  return rewrite_pr_legacy_column_args("pr", args);
}

auto echo_posixly_correct_literal_mode(std::string_view cmdName,
                                       std::span<std::string_view> args)
    -> bool {
  if (cmdName != "echo") {
    return false;
  }

  const char *value = std::getenv("POSIXLY_CORRECT");
  if (value == nullptr || value[0] == '\0') {
    return false;
  }

  return args.empty() || args[0] != "-n";
}

auto command_declares_option(std::span<const cmd::meta::OptionMeta> options,
                             std::string_view name) -> bool {
  return std::ranges::any_of(options, [name](const auto &option) {
    return option.short_name == name || option.long_name == name;
  });
}
auto command_declares_version_short(
    std::span<const cmd::meta::OptionMeta> options, std::string_view name)
    -> bool {
  return std::ranges::any_of(options, [name](const auto &option) {
    return option.short_name == name && option.long_name == "--version";
  });
}
auto wants_standard_version(std::string_view cmdName,
                            std::span<std::string_view> args,
                            std::span<const cmd::meta::OptionMeta> options)
    -> bool {
  if (echo_posixly_correct_literal_mode(cmdName, args)) {
    return false;
  }
  for (const auto &arg : args) {
    if (arg == "--") {
      break;
    }
    if (arg == "--version") {
      return true;
    }
    if (arg == "-v" && command_declares_version_short(options, "-v")) {
      return true;
    }
    if (arg == "-V") {
      return !command_declares_option(options, arg) ||
             command_declares_version_short(options, arg);
    }
  }
  return false;
}
auto rewrite_echo_posix_args(std::string_view cmdName,
                             std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  if (!echo_posixly_correct_literal_mode(cmdName, args)) {
    return std::nullopt;
  }

  std::vector<std::string> rewritten;
  rewritten.reserve(args.size() + 1);
  rewritten.emplace_back("--");
  append_remaining_args(rewritten, args, 0);
  return rewritten;
}

auto rewrite_echo_args(std::span<std::string_view> args)
    -> std::optional<std::vector<std::string>> {
  return rewrite_echo_posix_args("echo", args);
}

auto echo_standard_interception_enabled(std::span<std::string_view> args)
    -> bool {
  return !echo_posixly_correct_literal_mode("echo", args);
}

auto default_standard_interception_enabled(std::span<std::string_view>)
    -> bool {
  return true;
}

auto dispatch_wpm_help_version(const CommandEntryErased &entry,
                               std::span<std::string_view> args)
    -> std::optional<int> {
  for (const auto &arg : args) {
    if (arg == "--help") {
      return entry.handler(std::span<std::string_view>{});
    }
    if (arg == "--version" || arg == "-V") {
      std::array<std::string_view, 1> version_args{"version"};
      return entry.handler(version_args);
    }
  }

  return std::nullopt;
}

auto append_rewrite_hook(CommandBehavior &behavior, ArgsRewriteHook hook)
    -> void {
  if (behavior.rewrite_hook_count < behavior.rewrite_hooks.size()) {
    behavior.rewrite_hooks[behavior.rewrite_hook_count++] = hook;
  }
}

auto behavior_for(std::string_view name) -> CommandBehavior {
  CommandBehavior behavior;
  behavior.standard_interception_enabled =
      default_standard_interception_enabled;

  if (name == "env" || name == "nice" || name == "stdbuf" ||
      name == "timeout") {
    behavior.parse_error_exit_code = 125;
  } else if (name == "nohup") {
    behavior.parse_error_exit_code = is_posixly_correct() ? 127 : 125;
  } else if (name == "printenv" || name == "tty") {
    behavior.parse_error_exit_code = 2;
  }

  if (name == "head") {
    behavior.validate_args = head_invalid_obsolete_count_context;
    behavior.validation_prefix = "head";
    append_rewrite_hook(behavior, rewrite_head_obsolete_args);
  } else if (name == "tail") {
    behavior.validate_args = tail_invalid_obsolete_count_context;
    behavior.validation_prefix = "tail";
    append_rewrite_hook(behavior, rewrite_tail_obsolete_args);
  } else if (name == "chmod") {
    append_rewrite_hook(behavior, rewrite_chmod_args);
  } else if (name == "chattr") {
    append_rewrite_hook(behavior, rewrite_chattr_args);
  } else if (name == "nice") {
    append_rewrite_hook(behavior, rewrite_nice_args);
  } else if (name == "pr") {
    append_rewrite_hook(behavior, rewrite_pr_args);
  } else if (name == "echo") {
    append_rewrite_hook(behavior, rewrite_echo_args);
    behavior.standard_interception_enabled = echo_standard_interception_enabled;
  } else if (name == "wpm") {
    behavior.special_dispatch = dispatch_wpm_help_version;
  }

  return behavior;
}

auto replace_effective_args(std::vector<std::string> rewritten,
                            std::vector<std::string> &storage,
                            std::vector<std::string_view> &views)
    -> std::span<std::string_view> {
  storage = std::move(rewritten);
  views.clear();
  views.reserve(storage.size());
  for (const auto &arg : storage) {
    views.emplace_back(arg);
  }
  return std::span<std::string_view>(views);
}

// Internal registry implementation class
class RegistryImpl {
  std::unordered_map<std::string_view, CommandEntryErased> registry_;

 public:
  static auto parse_error_exit_code(std::string_view name) -> int {
    return behavior_for(name).parse_error_exit_code;
  }

  // Register a command with compile-time metadata
  template <size_t N>
  void add(std::string_view name, const cmd::meta::CommandMeta<N> &meta,
           CommandInvoker handler) {
    // compile-time meta registry
    cmd::meta::Registry::register_command(name, meta);

    registry_.emplace(
        name, CommandEntryErased{meta.options(), handler, meta.brief_desc()});
  }

  // Dispatch command execution
  int run(std::string_view cmdName, std::span<std::string_view> args) {
    auto it = registry_.find(cmdName);
    if (it == registry_.end()) {
      safePrintLn(L"winuxcmd: command not found: " +
                  std::wstring(cmdName.begin(), cmdName.end()));
      return 127;
    }

    const CommandBehavior behavior = behavior_for(cmdName);

    if (behavior.validate_args != nullptr) {
      if (auto invalid_arg = behavior.validate_args(args)) {
        safeErrorPrintLn(std::string(behavior.validation_prefix) +
                         ": option used in invalid context -- " + *invalid_arg);
        return behavior.parse_error_exit_code;
      }
    }

    std::vector<std::string> rewritten_storage;
    std::vector<std::string_view> rewritten_views;
    std::span<std::string_view> effective_args = args;

    for (size_t i = 0; i < behavior.rewrite_hook_count; ++i) {
      if (auto hook = behavior.rewrite_hooks[i]) {
        if (auto rewritten = hook(effective_args)) {
          effective_args = replace_effective_args(
              std::move(*rewritten), rewritten_storage, rewritten_views);
        }
      }
    }

    // Get meta data from the command
    auto options = it->second.options;  // std::span<const OptionMeta>

    if (behavior.special_dispatch != nullptr) {
      if (auto status = behavior.special_dispatch(it->second, effective_args)) {
        return *status;
      }
    }

    // Check if it contains help
    bool wants_help = false;
    if (behavior.standard_interception_enabled(args)) {
      for (const auto &arg : effective_args) {
        if (arg == "--help") {
          wants_help = true;
          break;
        }
      }
    }

    if (wants_help) {
      cmd::meta::Registry::print_help(cmdName);
      return 0;
    }

    if (wants_standard_version(cmdName, effective_args, options)) {
      safePrintLn(std::string(cmdName) + " (WinuxCmd) " +
                  std::string(WinuxCmd::VERSION_STRING));
      return 0;
    }

    return it->second.handler(effective_args);
  }

  // Print command help
  void help(std::string_view cmdName) {
    cmd::meta::Registry::print_help(cmdName);
  }

  // Get man page for a command
  std::string man(std::string_view cmdName) {
    return cmd::meta::Registry::get_man(cmdName);
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

  // Get options for a specific command (for completion)
  std::vector<OptionInfo> command_options(std::string_view cmdName) {
    auto it = registry_.find(cmdName);
    if (it == registry_.end()) return {};
    auto opts = it->second.options;
    std::vector<OptionInfo> result;
    result.reserve(opts.size());
    for (const auto &opt : opts) {
      result.push_back({std::string(opt.short_name), std::string(opt.long_name),
                        std::string(opt.description)});
    }
    return result;
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
                              CommandInvoker handler) {
    getImpl().add<N>(name, meta, handler);
  }

  static int parseErrorExitCode(std::string_view name) noexcept {
    return RegistryImpl::parse_error_exit_code(name);
  }

  // Dispatch command execution (public interface)
  static int dispatch(std::string_view cmdName,
                      std::span<std::string_view> args) noexcept {
    return getImpl().run(cmdName, args);
  }

  // Print command help (public interface)
  static void printHelp(std::string_view cmdName) noexcept {
    getImpl().help(cmdName);
  }

  // Get all registered command names (public interface)
  static std::vector<std::pair<std::string_view, std::string_view>>
  getAllCommands() noexcept {
    return getImpl().list();
  }

  // Get options for a command (for completion)
  static std::vector<OptionInfo> getCommandOptions(
      std::string_view cmdName) noexcept {
    return getImpl().command_options(cmdName);
  }

  // Check whether a command is registered.
  static bool hasCommand(std::string_view cmdName) noexcept {
    auto all = getImpl().list();
    return std::ranges::any_of(
        all, [cmdName](const auto &item) { return item.first == cmdName; });
  }

  // Print man page for a command
  static std::string getManPage(std::string_view cmdName) noexcept {
    return getImpl().man(cmdName);
  }
};

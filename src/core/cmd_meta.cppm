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
 *  - File: cmd_meta.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
module;
#include <cstdio>
export module core.cmd_meta;
import std;

namespace cmd::meta {
// OptionMeta with constexpr support
export struct OptionMeta {
  std::string_view short_name;
  std::string_view long_name;
  std::string_view description;

  constexpr OptionMeta(std::string_view s = "", std::string_view l = "",
                       std::string_view d = "")
      : short_name(s), long_name(l), description(d) {}
};

// Compile-time command metadata (fully compile-time)
export template <size_t OptionCount>
class CommandMeta {
 private:
  std::string_view m_name;
  std::string_view m_synopsis;
  std::string_view m_description;
  std::array<OptionMeta, OptionCount> m_options;
  std::string_view m_examples;
  std::string_view m_see_also;
  std::string_view m_author;
  std::string_view m_copyright;

 public:
  // constexpr constructor
  constexpr CommandMeta(
      std::string_view name, std::string_view synopsis,
      std::string_view description, std::array<OptionMeta, OptionCount> options,
      std::string_view examples = "", std::string_view see_also = "",
      std::string_view author = "WinuxCmd Project",
      std::string_view copyright = "Copyright © 2026 WinuxCmd")
      : m_name(name),
        m_synopsis(synopsis),
        m_description(description),
        m_options(options),
        m_examples(examples),
        m_see_also(see_also),
        m_author(author),
        m_copyright(copyright) {}

  // Accessors
  constexpr std::string_view name() const { return m_name; }
  constexpr std::string_view synopsis() const { return m_synopsis; }
  constexpr std::string_view description() const { return m_description; }
  constexpr const auto &options() const { return m_options; }
  constexpr size_t option_count() const { return OptionCount; }
  constexpr std::string_view examples() const { return m_examples; }
  constexpr std::string_view see_also() const { return m_see_also; }
  constexpr std::string_view author() const { return m_author; }
  constexpr std::string_view copyright() const { return m_copyright; }

  // Generate help text
  std::string get_help() const {
    std::string result;
    result.reserve(4096);

    // 1. Usage line
    result += "Usage: ";
    result.append(m_name.data(), m_name.size());
    result += " [OPTION]... [FILE]...\n";

    // 2. Synopsis
    if (!m_synopsis.empty()) {
      result += m_synopsis;
      result += "\n\n";
    }

    // 3. Description
    if (!m_description.empty()) {
      result.append(m_description.data(), m_description.size());
      result += "\n\n";
    }

    // 4. Mandatory arguments line
    // result += "Mandatory arguments to long options are mandatory for short
    // options too.\n\n";

    // 5. Options list (compile-time array, runtime iteration)
    if constexpr (OptionCount > 0) {
      // Keep options in the order they were registered (no sorting)
      std::vector<const OptionMeta *> ordered_options;
      ordered_options.reserve(OptionCount);
      for (const auto &opt : m_options) {
        ordered_options.push_back(&opt);
      }

      // Calculate alignment width
      size_t max_option_width = 0;
      for (const auto *opt : ordered_options) {
        size_t width = 2;
        if (!opt->short_name.empty() && !opt->long_name.empty()) {
          width += opt->short_name.size() + 2 + opt->long_name.size();
        } else if (!opt->short_name.empty()) {
          width += opt->short_name.size();
        } else if (!opt->long_name.empty()) {
          width += 4 + opt->long_name.size();
        }
        max_option_width = std::max(max_option_width, width);
      }

      result += "OPTIONS\n";
      for (const auto *opt : ordered_options) {
        // Build option string
        std::string option_str;
        if (!opt->short_name.empty() && !opt->long_name.empty()) {
          option_str = "  " + std::string(opt->short_name) + ", " +
                       std::string(opt->long_name);
        } else if (!opt->short_name.empty()) {
          option_str = "  " + std::string(opt->short_name);
        } else if (!opt->long_name.empty()) {
          option_str = "      " + std::string(opt->long_name);
        }

        // Align description
        result += option_str;
        if (opt->description.empty()) {
          result += "\n";
        } else {
          size_t padding = max_option_width + 2 - option_str.size();
          if (padding > 0) {
            result.append(padding, ' ');
          }

          // Multi-line parsing
          std::string_view desc = opt->description;
          bool first_line = true;
          while (!desc.empty()) {
            if (!first_line) {
              result.append(max_option_width + 2, ' ');
            }

            size_t newline_pos = desc.find('\n');
            if (newline_pos == std::string_view::npos) {
              result.append(desc.data(), desc.size());
              result += "\n";
              break;
            } else {
              result.append(desc.data(), newline_pos);
              result += "\n";
              desc = desc.substr(newline_pos + 1);
            }
            first_line = false;
          }
        }
        result += "\n";
      }
    }

    // 6. Standard help options
    result += "  -h, --help\n";
    result += "          display this help and exit\n\n";

    // 7. Version
    result += "  -V, --version\n";
    result += "          output version information and exit\n\n";

    // 8. Exit status
    result += "Exit status:\n";
    result += "  0  if OK,\n";
    result += "  1  if minor problems,\n";
    result += "  2  if serious trouble.\n";

    return result;
  }

  // Generate man page format
  std::string get_man() const {
    std::string result;
    result.reserve(4096);

    // ========== 1. NAME ==========
    result += "NAME\n";
    result += "       ";
    result.append(m_name.data(), m_name.size());
    result += " - ";
    result.append(m_synopsis.data(), m_synopsis.size());
    result += "\n\n";

    // ========== 2. SYNOPSIS ==========
    result += "SYNOPSIS\n";
    result += "       ";
    result.append(m_name.data(), m_name.size());
    result += " [OPTION]... [FILE]...\n\n";

    // ========== 3. DESCRIPTION ==========
    result += "DESCRIPTION\n";
    if (!m_description.empty()) {
      std::string_view desc = m_description;
      bool first_line = true;

      while (!desc.empty()) {
        result += "       ";

        size_t newline_pos = desc.find('\n');
        if (newline_pos == std::string_view::npos) {
          result.append(desc.data(), desc.size());
          result += "\n";
          break;
        } else {
          result.append(desc.data(), newline_pos);
          result += "\n";
          desc = desc.substr(newline_pos + 1);
        }
      }
      result += "\n";
    }

    // ========== 4. OPTIONS ==========
    if constexpr (OptionCount > 0) {
      result += "OPTIONS\n";

      // Keep options in the order they were registered (no sorting)
      std::vector<const OptionMeta *> ordered_options;
      ordered_options.reserve(OptionCount);
      for (const auto &opt : m_options) {
        ordered_options.push_back(&opt);
      }

      // Calculate max option width
      size_t max_option_width = 0;
      for (const auto *opt : ordered_options) {
        size_t width = 7;  // "       " prefix

        if (!opt->short_name.empty() && !opt->long_name.empty()) {
          width += opt->short_name.size() + 2 + opt->long_name.size();
        } else if (!opt->short_name.empty()) {
          width += opt->short_name.size();
        } else if (!opt->long_name.empty()) {
          width += opt->long_name.size();
        }

        max_option_width = std::max(max_option_width, width);
      }

      // Print options
      for (const auto *opt : ordered_options) {
        result += "       ";

        if (!opt->short_name.empty() && !opt->long_name.empty()) {
          result += opt->short_name;
          result += ", ";
          result += opt->long_name;
        } else if (!opt->short_name.empty()) {
          result += opt->short_name;
        } else if (!opt->long_name.empty()) {
          result += opt->long_name;
        }

        if (!opt->description.empty()) {
          // Align the description
          size_t current_width = result.length() - result.rfind('\n');
          if (current_width < max_option_width) {
            result.append(max_option_width - current_width, ' ');
          } else {
            result += "\n";
            result.append(max_option_width, ' ');
          }

          // Multi-line description
          std::string_view desc = opt->description;
          bool first_line = true;

          while (!desc.empty()) {
            if (!first_line) {
              result += "\n";
              result.append(max_option_width, ' ');
            }

            size_t newline_pos = desc.find('\n');
            if (newline_pos == std::string_view::npos) {
              result.append(desc.data(), desc.size());
              break;
            } else {
              result.append(desc.data(), newline_pos);
              desc = desc.substr(newline_pos + 1);
            }
            first_line = false;
          }
        }
        result += "\n";
      }
      result += "\n";
    }

    // ========== 5. Standard Options ==========
    result += "       -h, --help\n";
    result += "              display this help and exit\n\n";

    result += "       -V, --version\n";
    result += "              output version information and exit\n\n";

    // ========== 6. EXAMPLES ==========
    if (!m_examples.empty()) {
      result += "EXAMPLES\n";
      std::string_view ex = m_examples;
      bool first_line = true;

      while (!ex.empty()) {
        result += "       ";

        size_t newline_pos = ex.find('\n');
        if (newline_pos == std::string_view::npos) {
          result.append(ex.data(), ex.size());
          result += "\n";
          break;
        } else {
          result.append(ex.data(), newline_pos);
          result += "\n";
          ex = ex.substr(newline_pos + 1);
        }
      }
      result += "\n";
    }

    // ========== 7. SEE ALSO ==========
    if (!m_see_also.empty()) {
      result += "SEE ALSO\n";
      result += "       ";
      result.append(m_see_also.data(), m_see_also.size());
      result += "\n\n";
    }

    // ========== 8. AUTHOR ==========
    result += "AUTHOR\n";
    result += "       ";
    result.append(m_author.data(), m_author.size());
    result += "\n\n";

    // ========== 9. COPYRIGHT ==========
    result += "COPYRIGHT\n";
    result += "       ";
    result.append(m_copyright.data(), m_copyright.size());
    result += "\n";

    return result;
  }
};

// Runtime type-erased wrapper
export class CommandMetaBase {
 public:
  virtual ~CommandMetaBase() = default;

  virtual std::string_view name() const = 0;

  virtual std::string get_help() const = 0;

  virtual std::string get_man() const = 0;
};

export template <size_t N>
class CommandMetaWrapper : public CommandMetaBase {
  const CommandMeta<N> &m_meta;

 public:
  CommandMetaWrapper(const CommandMeta<N> &meta) : m_meta(meta) {}

  std::string_view name() const override { return m_meta.name(); }
  std::string get_help() const override { return m_meta.get_help(); }
  std::string get_man() const override { return m_meta.get_man(); }
};

// Type-erased runtime handle
export class CommandMetaHandle {
  std::unique_ptr<CommandMetaBase> m_ptr;

 public:
  constexpr CommandMetaHandle() noexcept : m_ptr(nullptr) {}

  template <size_t N>
  CommandMetaHandle(const CommandMeta<N> &meta)
      : m_ptr(std::make_unique<CommandMetaWrapper<N> >(meta)) {}

  std::string_view name() const { return m_ptr->name(); }
  std::string get_help() const { return m_ptr->get_help(); }
  std::string get_man() const { return m_ptr->get_man(); }
};

// Registry for metadata
export class Registry {
 public:
  template <size_t N>
  static void register_command(std::string_view command_name,
                               const CommandMeta<N> &meta) {
    get_storage()[command_name] = CommandMetaHandle(meta);
  }

  static bool print_help(std::string_view cmd_name) {
    auto &storage = get_storage();
    auto it = storage.find(cmd_name);
    if (it != storage.end()) {
      printf("%s\n", it->second.get_help().c_str());
      return true;
    }
    return false;
  }

 private:
  static std::unordered_map<std::string_view, CommandMetaHandle> &
  get_storage() {
    static std::unordered_map<std::string_view, CommandMetaHandle> instance;
    return instance;
  }
};
}  // namespace cmd::meta

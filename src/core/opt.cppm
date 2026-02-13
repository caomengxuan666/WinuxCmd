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
 *  - File: opt.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
export module core:opt;

import std;
import :cmd_meta;

using OptionValue = std::variant<bool, int, std::string>;

export template <size_t N>
class ParsedOptions {
 private:
  std::array<OptionValue, N> values_{};
  std::bitset<N> present_;

 public:
  constexpr ParsedOptions() = default;

  // Simple set method
  void set(size_t index, OptionValue v) {
    values_[index] = std::move(v);
    present_.set(index);
  }

  bool has(size_t index) const { return present_.test(index); }

  template <typename T>
  T get(size_t index, T default_value = {}) const {
    if (!present_.test(index)) return default_value;

    if (auto p = std::get_if<T>(&values_[index])) return *p;

    return default_value;
  }
};

export template <size_t N>
struct ParseResult {
  ParsedOptions<N> options;
  std::vector<std::string_view> positionals;
  bool ok = true;
};

export template <size_t N>
ParseResult<N> parse_command(
    std::span<std::string_view> args,
    const std::array<cmd::meta::OptionMeta, N>& metas) {
  ParseResult<N> result;
  using cmd::meta::OptionType;

  bool end_of_options = false;

  for (size_t i = 0; i < args.size(); ++i) {
    std::string_view arg = args[i];

    // ---------- "--" : end of options ----------
    if (!end_of_options && arg == "--") {
      end_of_options = true;
      continue;
    }

    // ---------- long option ----------
    if (!end_of_options && arg.starts_with("--")) {
      const cmd::meta::OptionMeta* meta = nullptr;
      std::string_view value;
      std::string_view name = arg;

      size_t eq_pos = arg.find('=');
      if (eq_pos != std::string_view::npos) {
        name = arg.substr(0, eq_pos);
        value = arg.substr(eq_pos + 1);
      }

      for (const auto& m : metas) {
        if (m.long_name == name) {
          meta = &m;
          break;
        }
      }

      if (!meta) {
        result.ok = false;
        return result;
      }

      size_t idx = meta->index;

      switch (meta->type) {
        case OptionType::Bool:
          result.options.set(idx, true);
          break;

        case OptionType::Int: {
          int v = 0;
          std::string str;

          if (!value.empty()) {
            // --int=123
            str = std::string(value);
          } else {
            // --int 123
            if (i + 1 >= args.size()) {
              result.ok = false;
              return result;
            }
            str = std::string(args[++i]);
          }

          auto [ptr, ec] =
              std::from_chars(str.data(), str.data() + str.size(), v);
          if (ec != std::errc() || ptr != str.data() + str.size()) {
            result.ok = false;
            return result;
          }

          result.options.set(idx, v);
          break;
        }

        case OptionType::String: {
          if (!value.empty()) {
            // --string=value
            result.options.set(idx, std::string(value));
          } else {
            // --string value
            if (i + 1 >= args.size()) {
              result.ok = false;
              return result;
            }
            result.options.set(idx, std::string(args[++i]));
          }
          break;
        }
      }

      continue;
    }

    // ---------- short option(s) ----------
    if (!end_of_options && arg.size() >= 2 && arg[0] == '-' && arg[1] != '-') {
      // Support GNU-style single-dash multi-char options (e.g. -name)
      const cmd::meta::OptionMeta* exact_meta = nullptr;
      std::string_view exact_value;
      std::string_view exact_name = arg;
      size_t exact_eq_pos = arg.find('=');
      if (exact_eq_pos != std::string_view::npos) {
        exact_name = arg.substr(0, exact_eq_pos);
        exact_value = arg.substr(exact_eq_pos + 1);
      }

      for (const auto& m : metas) {
        if (!m.short_name.empty() && m.short_name == exact_name) {
          exact_meta = &m;
          break;
        }
      }

      if (exact_meta && exact_meta->short_name.size() > 2) {
        size_t idx = exact_meta->index;
        switch (exact_meta->type) {
          case OptionType::Bool:
            result.options.set(idx, true);
            break;
          case OptionType::Int: {
            int v = 0;
            std::string str;
            if (!exact_value.empty()) {
              str = std::string(exact_value);
            } else {
              if (i + 1 >= args.size()) {
                result.ok = false;
                return result;
              }
              str = std::string(args[++i]);
            }

            auto [ptr, ec] =
                std::from_chars(str.data(), str.data() + str.size(), v);
            if (ec != std::errc() || ptr != str.data() + str.size()) {
              result.ok = false;
              return result;
            }

            result.options.set(idx, v);
            break;
          }
          case OptionType::String:
            if (!exact_value.empty()) {
              result.options.set(idx, std::string(exact_value));
            } else {
              if (i + 1 >= args.size()) {
                result.ok = false;
                return result;
              }
              result.options.set(idx, std::string(args[++i]));
            }
            break;
        }

        continue;
      }

      // iterate each short flag: -abc
      for (size_t pos = 1; pos < arg.size(); ++pos) {
        char ch = arg[pos];

        const cmd::meta::OptionMeta* meta = nullptr;

        for (const auto& m : metas) {
          if (!m.short_name.empty() && m.short_name.size() == 2 &&
              m.short_name[0] == '-' && m.short_name[1] == ch) {
            meta = &m;
            break;
          }
        }

        if (!meta) {
          result.ok = false;
          return result;
        }

        size_t idx = meta->index;

        switch (meta->type) {
          // ----- bool: can be grouped -----
          case OptionType::Bool:
            result.options.set(idx, true);
            break;

          // ----- value option -----
          case OptionType::Int:
          case OptionType::String: {
            // value option MUST be last in group
            if (pos != arg.size() - 1) {
              result.ok = false;
              return result;
            }

            if (i + 1 >= args.size()) {
              result.ok = false;
              return result;
            }

            if (meta->type == OptionType::Int) {
              int v = 0;
              std::string str = std::string(args[++i]);
              auto [ptr, ec] =
                  std::from_chars(str.data(), str.data() + str.size(), v);

              if (ec != std::errc() || ptr != str.data() + str.size()) {
                result.ok = false;
                return result;
              }

              result.options.set(idx, v);
            } else {
              result.options.set(idx, std::string(args[++i]));
            }

            break;
          }
        }
      }

      continue;
    }

    // ---------- positional ----------
    result.positionals.push_back(arg);
  }

  return result;
}

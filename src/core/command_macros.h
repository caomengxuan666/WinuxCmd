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
 *  - File: command_macros.h
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#pragma once

#ifdef _MSC_VER
#define CMD_MSG(msg) __pragma(message("[WINUX] " msg))
#else
#define CMD_MSG(msg)
#endif

#define REGISTER_COMMAND(name, cmd_name, cmd_synopsis, cmd_desc, examples,     \
                         see_also, author, copyright, ...)                     \
  export int execute##name(std::span<std::string_view> args) noexcept;         \
  namespace command_##name##_internal {                                        \
    /* We must make sure no more include in this header */                     \
    /* So put them after the "import std" could avoid ODR between import       \
     * modules and traditional include headers */                              \
    template <typename T>                                                      \
    concept IsOptionMeta =                                                     \
        std::is_same_v<std::remove_cvref_t<T>, cmd::meta::OptionMeta>;         \
                                                                               \
    template <typename T>                                                      \
    concept IsOptionContainer = requires(T t) {                                \
      requires std::is_array_v<T> || requires {                                \
        { t.begin() } -> std::input_iterator;                                  \
        { t.end() } -> std::input_iterator;                                    \
        requires IsOptionMeta<std::iter_value_t<decltype(t.begin())>>;         \
      };                                                                       \
    };                                                                         \
                                                                               \
    template <typename T>                                                      \
    concept IsSingleOption = IsOptionMeta<T>;                                  \
                                                                               \
    template <typename T>                                                      \
    concept IsOptionArray = IsOptionContainer<T> && !IsOptionMeta<T>;          \
                                                                               \
    template <typename... Args>                                                \
    constexpr auto make_option_array_impl(Args && ... args) {                  \
      if constexpr (sizeof...(Args) == 0) {                                    \
        /* No param */                                                         \
        return std::array<cmd::meta::OptionMeta, 0>{};                         \
      } else if constexpr (sizeof...(Args) == 1) {                             \
        /* Single Param */                                                     \
        using FirstType = std::tuple_element_t<0, std::tuple<Args...>>;        \
        if constexpr (IsSingleOption<FirstType>) {                             \
          /* Single OptionalMeta */                                            \
          return std::array<cmd::meta::OptionMeta, 1>{                         \
              std::forward<Args>(args)...};                                    \
        } else if constexpr (IsOptionArray<FirstType>) {                       \
          /* Array|container */                                                \
          return std::forward<FirstType>(args...);                             \
        } else {                                                               \
          /* Type Error */                                                     \
          static_assert(                                                       \
              IsSingleOption<FirstType> || IsOptionArray<FirstType>,           \
              "Argument must be OptionMeta or container/array of OptionMeta"); \
          return std::array<cmd::meta::OptionMeta, 0>{}; /* fallback */        \
        }                                                                      \
      } else {                                                                 \
        /* Multi Params,all must be OptionMeta */                              \
        static_assert((IsSingleOption<Args> && ...),                           \
                      "All arguments must be OptionMeta when multiple "        \
                      "arguments are provided");                               \
        return std::array<cmd::meta::OptionMeta, sizeof...(Args)>{             \
            std::forward<Args>(args)...};                                      \
      }                                                                        \
    }                                                                          \
                                                                               \
    constexpr auto options = []() {                                            \
      return make_option_array_impl(__VA_ARGS__);                              \
    }();                                                                       \
                                                                               \
    constexpr auto meta = cmd::meta::CommandMeta<options.size()>(              \
        std::string_view(cmd_name), std::string_view(cmd_synopsis),            \
        std::string_view(cmd_desc), options, std::string_view(examples),       \
        std::string_view(see_also), std::string_view(author),                  \
        std::string_view(copyright));                                          \
  }                                                                            \
  namespace {                                                                  \
  CMD_MSG("Registering command with constexpr options: " #name)                \
  struct _Registrar_##name {                                                   \
    _Registrar_##name() {                                                      \
      CommandRegistry::registerCommand(#name, command_##name##_internal::meta, \
                                       execute##name);                         \
    }                                                                          \
  } _registrar_instance_##name;                                                \
  }                                                                            \
  int execute##name(std::span<std::string_view> args) noexcept

#undef OPTION
#define OPTION(s, l, d) \
  cmd::meta::OptionMeta { s, l, d }

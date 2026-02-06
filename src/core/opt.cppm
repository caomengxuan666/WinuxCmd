export module core:opt;

import std;
import :cmd_meta;

// Option handler structure for lookup table
export struct OptionHandler {
  char short_opt;
  bool requires_arg = false;
  const char* long_opt = nullptr;
};

// Helper functions for option handling
export constexpr char get_short_opt(std::string_view short_name) {
  if (short_name.size() >= 2 && short_name[0] == '-') {
    return short_name[1];
  }
  return 0;
}

// Helper function to check if an option requires an argument
export constexpr bool requires_argument(std::string_view long_name) {
  // Common options that require arguments
  return false;
}

// Overload that accepts a custom set of argument-requiring options
export template <typename... Args>
constexpr bool requires_argument(std::string_view long_name, Args... args) {
  // Check if long_name matches any of the argument-requiring options
  return ((long_name == args) || ...);
}

export constexpr const char* sv_to_cstr(std::string_view sv) {
  return sv.empty() ? nullptr : sv.data();
}

// Metafunction to generate OPTION_HANDLERS from options array
template <size_t N, size_t... Is, typename... Args>
constexpr auto generate_option_handlers_impl(
    const std::array<cmd::meta::OptionMeta, N>& options,
    std::index_sequence<Is...>, Args... args) {
  return std::array<OptionHandler, N>{
      {{get_short_opt(options[Is].short_name),
        requires_argument(options[Is].long_name, args...),
        sv_to_cstr(options[Is].long_name)}...}};
}

// Base case: no argument-requiring options
export template <size_t N>
constexpr auto generate_option_handlers(
    const std::array<cmd::meta::OptionMeta, N>& options) {
  return generate_option_handlers_impl(options, std::make_index_sequence<N>{});
}

// Overload: accept custom argument-requiring options
export template <size_t N, typename... Args>
constexpr auto generate_option_handlers(
    const std::array<cmd::meta::OptionMeta, N>& options, Args... args) {
  return generate_option_handlers_impl(options, std::make_index_sequence<N>{},
                                       args...);
}

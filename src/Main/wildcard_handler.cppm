export module wildcard_handler;
import std;
import utils;

export std::vector<std::string> expand_all_wildcards(
    const std::vector<std::string_view> &raw_args) noexcept {
  std::vector<std::string> expanded_args;

  for (const auto &arg : raw_args) {
    // skip option args
    if (!arg.empty() && arg[0] == '-') {
      expanded_args.push_back(std::string(arg));
      continue;
    }

    auto matched = expand_wildcard(arg);
    if (matched.empty()) {
      expanded_args.push_back(std::string(arg));
    } else {
      // sort the matched files
      std::sort(matched.begin(), matched.end());
      auto last = std::unique(matched.begin(), matched.end());
      matched.erase(last, matched.end());

      expanded_args.insert(expanded_args.end(),
                           std::make_move_iterator(matched.begin()),
                           std::make_move_iterator(matched.end()));
    }
  }

  return expanded_args;
}

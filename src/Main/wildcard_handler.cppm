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
 *  - File: wildcard_handler.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
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

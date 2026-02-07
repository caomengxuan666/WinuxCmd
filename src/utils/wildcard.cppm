export module utils:wildcard;

import std;
namespace fs = std::filesystem;

static bool wildcard_match(const std::string &str,
                           const std::string &pattern) noexcept {
  const size_t str_len = str.size();
  const size_t pat_len = pattern.size();
  size_t s = 0, p = 0, star_pos = static_cast<size_t>(-1);

  while (s < str_len) {
    if (p < pat_len && (pattern[p] == '?' || str[s] == pattern[p])) {
      ++s;
      ++p;
    } else if (p < pat_len && pattern[p] == '*') {
      star_pos = p++;
    } else if (star_pos != static_cast<size_t>(-1)) {
      p = star_pos + 1;
      ++s;
    } else {
      return false;
    }
  }

  while (p < pat_len && pattern[p] == '*') {
    ++p;
  }

  return p == pat_len;
}

export std::vector<std::string> expand_wildcard(
    const std::string_view pattern) noexcept {
  std::vector<std::string> matched_files;
  const std::string pat(pattern);

  if (pat.find_first_of("*?") == std::string::npos) {
    matched_files.push_back(pat);
    return matched_files;
  }

  std::error_code ec;
  const fs::directory_iterator dir_iter(fs::current_path(), ec);
  if (ec) {
    return matched_files;
  }

  for (const auto &entry : dir_iter) {
    if (!entry.exists(ec) || ec) {
      continue;
    }
    const std::string filename = entry.path().filename().string();
    if (wildcard_match(filename, pat)) {
      matched_files.push_back(filename);
    }
  }

  std::sort(matched_files.begin(), matched_files.end());
  auto last = std::unique(matched_files.begin(), matched_files.end());
  matched_files.erase(last, matched_files.end());

  return matched_files;
}

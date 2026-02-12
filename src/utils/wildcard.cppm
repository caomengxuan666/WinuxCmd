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
 *  - File: wildcard.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
export module utils:wildcard;

import std;
import :utf8;
import :path;

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

  // Get current directory
  std::string current_dir = path::current_path();
  if (current_dir.empty()) {
    return matched_files;
  }

  // Build search pattern: current_dir\*
  std::wstring search_pattern = utf8_to_wstring(path::join(current_dir, "*"));

  // Find first file
  WIN32_FIND_DATAW find_data;
  HANDLE hFind = FindFirstFileW(search_pattern.c_str(), &find_data);
  if (hFind == INVALID_HANDLE_VALUE) {
    return matched_files;
  }

  // Iterate through all files
  do {
    // Skip . and ..
    if (wcscmp(find_data.cFileName, L".") == 0 ||
        wcscmp(find_data.cFileName, L"..") == 0) {
      continue;
    }

    // Convert filename to UTF-8
    int filename_len = WideCharToMultiByte(CP_UTF8, 0, find_data.cFileName,
                                          -1, NULL, 0, NULL, NULL);
    if (filename_len <= 0) {
      continue;
    }
    std::string filename(filename_len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, find_data.cFileName, -1, &filename[0],
                        filename_len, NULL, NULL);

    // Check if filename matches the pattern
    if (wildcard_match(filename, pat)) {
      matched_files.push_back(filename);
    }
  } while (FindNextFileW(hFind, &find_data));

  FindClose(hFind);

  // Sort and remove duplicates
  std::sort(matched_files.begin(), matched_files.end());
  auto last = std::unique(matched_files.begin(), matched_files.end());
  matched_files.erase(last, matched_files.end());

  return matched_files;
}

/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: wildcard.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
export module utils:wildcard;

import std;
import :utf8;
import :path;

namespace wildcard_impl {

/**
 * @brief Check if a character matches a character class pattern
 * @param char_class The character class string (e.g., "[abc]", "[a-z]", "[^0-9]")
 * @param c The character to check
 * @return true if the character matches the class, false otherwise
 */
static bool match_char_class(std::wstring_view char_class, wchar_t c) {
  if (static_cast<size_t>(char_class.size()) < 2) return false;

  // Check for negation [^...]
  bool negate = (char_class[1] == L'^');
  size_t start = negate ? 2 : 1;
  size_t end = static_cast<size_t>(char_class.size()) - 1;  // Exclude closing ']'

  if (start >= end) return false;

  // Check for range [a-z]
  for (size_t i = start; i < end; ++i) {
    if (i + 2 < end && char_class[i + 1] == L'-') {
      // Range pattern: a-z
      wchar_t range_start = char_class[i];
      wchar_t range_end = char_class[i + 2];
      if (c >= range_start && c <= range_end) {
        return !negate;
      }
      i += 2;  // Skip the '-' and next character
    } else {
      // Single character: a
      if (char_class[i] == c) {
        return !negate;
      }
    }
  }

  return negate;  // If negate is true, return true (no match)
}

/**
 * @brief Core wildcard matching implementation with support for *, ?, and []
 * @param pattern The wildcard pattern
 * @param text The text to match against
 * @return true if text matches pattern, false otherwise
 */
static bool wildcard_match_impl(std::wstring_view pattern, std::wstring_view text) {
  size_t pi = 0, ti = 0;
  while (pi < static_cast<size_t>(pattern.size())) {
    if (pattern[pi] == L'*') {
      while (pi < static_cast<size_t>(pattern.size()) && pattern[pi] == L'*') ++pi;
      if (pi == static_cast<size_t>(pattern.size())) return true;
      while (ti <= static_cast<size_t>(text.size())) {
        if (wildcard_match_impl(pattern.substr(pi), text.substr(ti))) return true;
        if (ti == static_cast<size_t>(text.size())) break;
        ++ti;
      }
      return false;
    }

    if (ti >= static_cast<size_t>(text.size())) return false;

    if (pattern[pi] == L'?') {
      ++pi;
      ++ti;
    } else if (pattern[pi] == L'[') {
      // Find matching ']'
      size_t bracket_end = pi + 1;
      while (bracket_end < static_cast<size_t>(pattern.size()) && pattern[bracket_end] != L']') {
        bracket_end++;
      }

      if (bracket_end >= static_cast<size_t>(pattern.size())) {
        // No closing bracket, treat '[' as literal
        if (pattern[pi] != text[ti]) return false;
        ++pi;
        ++ti;
      } else {
        // Extract character class: [abc] or [a-z] or [^0-9]
        std::wstring_view char_class = pattern.substr(pi, bracket_end - pi + 1);
        if (!match_char_class(char_class, text[ti])) {
          return false;
        }
        pi = bracket_end + 1;
        ++ti;
      }
    } else if (pattern[pi] == text[ti]) {
      ++pi;
      ++ti;
    } else {
      return false;
    }
  }

  return ti == static_cast<size_t>(text.size());
}

}  // namespace wildcard_impl

/**
 * @brief Enhanced wildcard matching with support for *, ?, and []
 * @param pattern The wildcard pattern
 * @param text The text to match against
 * @param case_sensitive Whether to perform case-sensitive matching (default: false)
 * @return true if text matches pattern, false otherwise
 */
export bool wildcard_match(const std::wstring &pattern, const std::wstring &text, bool case_sensitive = false) {
  if (case_sensitive) {
    return wildcard_impl::wildcard_match_impl(pattern, text);
  }

  // Case-insensitive matching
  auto to_lower = [](std::wstring_view s) {
    std::wstring result;
    result.reserve(static_cast<size_t>(s.size()));
    for (wchar_t c : s) {
      result.push_back(std::towlower(c));
    }
    return result;
  };

  auto p = to_lower(pattern);
  auto t = to_lower(text);
  return wildcard_impl::wildcard_match_impl(p, t);
}

/**
 * @brief Expand wildcard pattern in the current directory
 * @param pattern The wildcard pattern (e.g., "*.txt", "dir/*.cpp", "[0-9]*")
 * @return Vector of matched file paths (relative to current directory)
 *
 * This function:
 * - Handles patterns with directory paths (e.g., "src/*.cpp")
 * - Supports *, ?, and [] wildcards
 * - Returns sorted and deduplicated results
 * - If pattern contains no wildcards, returns the pattern itself
 */
export std::vector<std::string> expand_wildcard(const std::string_view pattern) noexcept {
  std::vector<std::string> matched_files;
  const std::string pat(pattern);

  // If pattern contains no wildcards, return it as-is
  if (pat.find_first_of("*?[") == std::string::npos) {
    matched_files.push_back(pat);
    return matched_files;
  }

  // Split pattern into directory part and filename part
  std::string dir_part, file_part;
  size_t last_slash = pat.find_last_of("/\\");
  if (last_slash == std::string::npos) {
    dir_part = ".";
    file_part = pat;
  } else {
    dir_part = pat.substr(0, last_slash);
    file_part = pat.substr(last_slash + 1);
  }

  // Convert to wide strings
  std::wstring wdir = utf8_to_wstring(dir_part);
  std::wstring wfile_pat = utf8_to_wstring(file_part);

  // Build search path: dir\*
  std::wstring search_path = path::join(wdir, L"*");

  // Find first file
  WIN32_FIND_DATAW find_data;
  HANDLE hFind = FindFirstFileW(search_path.c_str(), &find_data);
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

    std::wstring filename = find_data.cFileName;

    // Check if filename matches the pattern
    if (wildcard_match(wfile_pat, filename, false)) {
      // Build full path
      std::string full_path;
      if (dir_part == ".") {
        full_path = wstring_to_utf8(filename);
      } else {
        full_path = path::join(dir_part, wstring_to_utf8(filename));
      }
      matched_files.push_back(full_path);
    }
  } while (FindNextFileW(hFind, &find_data));

  FindClose(hFind);

  // Sort and remove duplicates
  std::sort(matched_files.begin(), matched_files.end());
  auto last = std::unique(matched_files.begin(), matched_files.end());
  matched_files.erase(last, matched_files.end());

  return matched_files;
}
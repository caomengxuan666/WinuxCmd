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
 *  - File: path.cppm
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
export module utils:path;

import std;
import :utf8;

#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

namespace path {

/**
 * @brief Normalize path separators to backslashes
 * @param path Path to normalize
 * @return Normalized path with backslashes
 */
export std::string normalize_path(const std::string& path) {
  std::string result = path;
  for (auto& c : result) {
    if (c == '/') {
      c = '\\';
    }
  }
  return result;
}

/**
 * @brief Normalize path separators to backslashes (wide string version)
 * @param path Path to normalize
 * @return Normalized path with backslashes
 */
export std::wstring normalize_path(const std::wstring& path) {
  std::wstring result = path;
  for (auto& c : result) {
    if (c == L'/') {
      c = L'\\';
    }
  }
  return result;
}

/**
 * @brief Get the parent directory of a path
 * @param path Path to get parent from
 * @return Parent directory path
 */
export std::string get_parent_path(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  wchar_t parent[MAX_PATH];
  if (PathRemoveFileSpecW(wpath.data())) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), -1, NULL, 0,
                                   NULL, NULL);
    std::string result(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wpath.c_str(), -1, &result[0], len, NULL,
                        NULL);
    return result;
  }
  return "";
}

/**
 * @brief Get the parent directory of a path (wide string version)
 * @param path Path to get parent from
 * @return Parent directory path
 */
export std::wstring get_parent_path(const std::wstring& path) {
  std::wstring result = path;
  if (PathRemoveFileSpecW(result.data())) {
    return result;
  }
  return L"";
}

/**
 * @brief Get the file name (stem + extension) from a path
 * @param path Path to extract file name from
 * @return File name
 */
export std::string get_filename(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  LPWSTR file_name = PathFindFileNameW(wpath.c_str());
  if (file_name) {
    int len = WideCharToMultiByte(CP_UTF8, 0, file_name, -1, NULL, 0, NULL,
                                   NULL);
    std::string result(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, file_name, -1, &result[0], len, NULL,
                        NULL);
    return result;
  }
  return "";
}

/**
 * @brief Get the file name (stem + extension) from a path (wide string version)
 * @param path Path to extract file name from
 * @return File name
 */
export std::wstring get_filename(const std::wstring& path) {
  LPWSTR file_name = PathFindFileNameW(path.c_str());
  if (file_name) {
    return std::wstring(file_name);
  }
  return L"";
}

/**
 * @brief Get the file stem (without extension) from a path
 * @param path Path to extract stem from
 * @return File stem
 */
export std::string get_stem(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  LPWSTR file_name = PathFindFileNameW(wpath.c_str());
  if (file_name) {
    LPWSTR dot = PathFindExtensionW(file_name);
    if (dot) {
      *dot = L'\0';
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, file_name, -1, NULL, 0, NULL,
                                   NULL);
    std::string result(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, file_name, -1, &result[0], len, NULL,
                        NULL);
    return result;
  }
  return "";
}

/**
 * @brief Get the file stem (without extension) from a path (wide string version)
 * @param path Path to extract stem from
 * @return File stem
 */
export std::wstring get_stem(const std::wstring& path) {
  LPWSTR file_name = PathFindFileNameW(path.c_str());
  if (file_name) {
    LPWSTR dot = PathFindExtensionW(file_name);
    if (dot) {
      *dot = L'\0';
    }
    return std::wstring(file_name);
  }
  return L"";
}

/**
 * @brief Get the file extension from a path
 * @param path Path to extract extension from
 * @return File extension (including the dot)
 */
export std::string get_extension(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  LPWSTR ext = PathFindExtensionW(wpath.c_str());
  if (ext) {
    int len = WideCharToMultiByte(CP_UTF8, 0, ext, -1, NULL, 0, NULL, NULL);
    std::string result(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, ext, -1, &result[0], len, NULL, NULL);
    return result;
  }
  return "";
}

/**
 * @brief Get the file extension from a path (wide string version)
 * @param path Path to extract extension from
 * @return File extension (including the dot)
 */
export std::wstring get_extension(const std::wstring& path) {
  LPWSTR ext = PathFindExtensionW(path.c_str());
  if (ext) {
    return std::wstring(ext);
  }
  return L"";
}

/**
 * @brief Check if a path exists
 * @param path Path to check
 * @return true if path exists, false otherwise
 */
export bool exists(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  DWORD attr = GetFileAttributesW(wpath.c_str());
  return attr != INVALID_FILE_ATTRIBUTES;
}

/**
 * @brief Check if a path exists (wide string version)
 * @param path Path to check
 * @return true if path exists, false otherwise
 */
export bool exists(const std::wstring& path) {
  DWORD attr = GetFileAttributesW(path.c_str());
  return attr != INVALID_FILE_ATTRIBUTES;
}

/**
 * @brief Check if a path is a directory
 * @param path Path to check
 * @return true if path is a directory, false otherwise
 */
export bool is_directory(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  DWORD attr = GetFileAttributesW(wpath.c_str());
  if (attr == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/**
 * @brief Check if a path is a directory (wide string version)
 * @param path Path to check
 * @return true if path is a directory, false otherwise
 */
export bool is_directory(const std::wstring& path) {
  DWORD attr = GetFileAttributesW(path.c_str());
  if (attr == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/**
 * @brief Check if a path is a regular file
 * @param path Path to check
 * @return true if path is a regular file, false otherwise
 */
export bool is_regular_file(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  DWORD attr = GetFileAttributesW(wpath.c_str());
  if (attr == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

/**
 * @brief Check if a path is a regular file (wide string version)
 * @param path Path to check
 * @return true if path is a regular file, false otherwise
 */
export bool is_regular_file(const std::wstring& path) {
  DWORD attr = GetFileAttributesW(path.c_str());
  if (attr == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

/**
 * @brief Create a single directory
 * @param path Path to create
 * @return true if directory was created or already exists, false on error
 */
export bool create_directory(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  if (!CreateDirectoryW(wpath.c_str(), NULL)) {
    DWORD error = GetLastError();
    return error == ERROR_ALREADY_EXISTS;
  }
  return true;
}

/**
 * @brief Create a single directory (wide string version)
 * @param path Path to create
 * @return true if directory was created or already exists, false on error
 */
export bool create_directory(const std::wstring& path) {
  if (!CreateDirectoryW(path.c_str(), NULL)) {
    DWORD error = GetLastError();
    return error == ERROR_ALREADY_EXISTS;
  }
  return true;
}

/**
 * @brief Create directories recursively
 * @param path Path to create (can contain both / and \ separators)
 * @return true if directory was created successfully, false on error
 */
export bool create_directories(const std::string& path) {
  std::wstring wpath(path.begin(), path.end());
  std::wstring normalized_path = normalize_path(wpath);

  // Skip drive letter if present (e.g., "C:\dir1" -> start from "C:\dir1")
  size_t start_pos = 0;
  if (normalized_path.size() >= 3 && normalized_path[1] == L':' &&
      (normalized_path[2] == L'\\' || normalized_path[2] == L'/')) {
    // Path starts with drive letter (e.g., "C:\dir1")
    // Start from position 3 (after "C:\")
    start_pos = 3;
  }

  std::wstring current_path;
  std::wstring::size_type pos = start_pos;

  while (true) {
    // Find the next backslash
    pos = normalized_path.find(L'\\', pos);
    if (pos != std::wstring::npos) {
      current_path = normalized_path.substr(0, pos);
      pos++;  // Move past this backslash
    } else {
      current_path = normalized_path;
      break;
    }

    // Skip if current_path is empty or just a drive letter
    if (current_path.empty() ||
        (current_path.size() == 2 && current_path[1] == L':')) {
      continue;
    }

    if (!CreateDirectoryW(current_path.c_str(), NULL)) {
      DWORD error = GetLastError();
      if (error != ERROR_ALREADY_EXISTS) {
        return false;
      }
    }
  }

  // Create the final directory
  if (!current_path.empty() &&
      !(current_path.size() == 2 && current_path[1] == L':')) {
    if (!CreateDirectoryW(current_path.c_str(), NULL)) {
      DWORD error = GetLastError();
      if (error != ERROR_ALREADY_EXISTS) {
        return false;
      }
    }
  }

  return true;
}

/**
 * @brief Create directories recursively (wide string version)
 * @param path Path to create (can contain both / and \ separators)
 * @return true if directory was created successfully, false on error
 */
export bool create_directories(const std::wstring& path) {
  std::wstring normalized_path = normalize_path(path);

  // Skip drive letter if present (e.g., "C:\dir1" -> start from "C:\dir1")
  size_t start_pos = 0;
  if (normalized_path.size() >= 3 && normalized_path[1] == L':' &&
      (normalized_path[2] == L'\\' || normalized_path[2] == L'/')) {
    // Path starts with drive letter (e.g., "C:\dir1")
    // Start from position 3 (after "C:\")
    start_pos = 3;
  }

  std::wstring current_path;
  std::wstring::size_type pos = start_pos;

  while (true) {
    // Find the next backslash
    pos = normalized_path.find(L'\\', pos);
    if (pos != std::wstring::npos) {
      current_path = normalized_path.substr(0, pos);
      pos++;  // Move past this backslash
    } else {
      current_path = normalized_path;
      break;
    }

    // Skip if current_path is empty or just a drive letter
    if (current_path.empty() ||
        (current_path.size() == 2 && current_path[1] == L':')) {
      continue;
    }

    if (!CreateDirectoryW(current_path.c_str(), NULL)) {
      DWORD error = GetLastError();
      if (error != ERROR_ALREADY_EXISTS) {
        return false;
      }
    }
  }

  // Create the final directory
  if (!current_path.empty() &&
      !(current_path.size() == 2 && current_path[1] == L':')) {
    if (!CreateDirectoryW(current_path.c_str(), NULL)) {
      DWORD error = GetLastError();
      if (error != ERROR_ALREADY_EXISTS) {
        return false;
      }
    }
  }

  return true;
}

/**
 * @brief Get the current working directory
 * @return Current working directory as UTF-8 string
 */
export std::string current_path() {
  wchar_t buffer[MAX_PATH];
  DWORD len = GetCurrentDirectoryW(MAX_PATH, buffer);
  if (len == 0 || len > MAX_PATH) {
    return "";
  }
  int utf8_len = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL,
                                      NULL);
  std::string result(utf8_len - 1, 0);
  WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &result[0], utf8_len, NULL, NULL);
  return result;
}

/**
 * @brief Get the current working directory (wide string version)
 * @return Current working directory as wide string
 */
export std::wstring current_path_w() {
  wchar_t buffer[MAX_PATH];
  DWORD len = GetCurrentDirectoryW(MAX_PATH, buffer);
  if (len == 0 || len > MAX_PATH) {
    return L"";
  }
  return std::wstring(buffer);
}

/**
 * @brief Get the executable path from argv[0]
 * @param argv0 argv[0] from main function
 * @return Executable path as UTF-8 string
 */
export std::string get_executable_path(const char* argv0) {
  int path_length = MultiByteToWideChar(CP_UTF8, 0, argv0, -1, NULL, 0);
  std::wstring wself_path(path_length, 0);
  MultiByteToWideChar(CP_UTF8, 0, argv0, -1, &wself_path[0], path_length);
  int utf8_len =
      WideCharToMultiByte(CP_UTF8, 0, wself_path.c_str(), -1, NULL, 0, NULL,
                          NULL);
  std::string result(utf8_len - 1, 0);
  WideCharToMultiByte(CP_UTF8, 0, wself_path.c_str(), -1, &result[0],
                      utf8_len, NULL, NULL);
  return result;
}

/**
 * @brief Get the executable name (stem) from argv[0]
 * @param argv0 argv[0] from main function
 * @return Executable name as UTF-8 string
 */
export std::string get_executable_name(const char* argv0) {
  int path_length = MultiByteToWideChar(CP_UTF8, 0, argv0, -1, NULL, 0);
  std::wstring wself_path(path_length, 0);
  MultiByteToWideChar(CP_UTF8, 0, argv0, -1, &wself_path[0], path_length);

  LPWSTR file_stem = PathFindFileNameW(wself_path.c_str());
  if (file_stem) {
    LPWSTR dot = PathFindExtensionW(file_stem);
    if (dot) {
      *dot = L'\0';
    }
  }

  int file_stem_length =
      WideCharToMultiByte(CP_UTF8, 0, file_stem, -1, NULL, 0, NULL, NULL);
  std::string self_name;
  self_name.resize(file_stem_length - 1);  // Exclude null terminator
  WideCharToMultiByte(CP_UTF8, 0, file_stem, -1, &self_name[0],
                      file_stem_length, NULL, NULL);
  return self_name;
}

/**
 * @brief Join path components
 * @param base Base path
 * @param relative Relative path component
 * @return Joined path
 */
export std::string join(const std::string& base, const std::string& relative) {
  std::wstring wbase(base.begin(), base.end());
  std::wstring wrelative(relative.begin(), relative.end());
  wchar_t result[MAX_PATH];
  if (PathCombineW(result, wbase.c_str(), wrelative.c_str())) {
    int len = WideCharToMultiByte(CP_UTF8, 0, result, -1, NULL, 0, NULL, NULL);
    std::string utf8_result(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, result, -1, &utf8_result[0], len, NULL,
                        NULL);
    return utf8_result;
  }
  return "";
}

/**
 * @brief Join path components (wide string version)
 * @param base Base path
 * @param relative Relative path component
 * @return Joined path
 */
export std::wstring join(const std::wstring& base,
                         const std::wstring& relative) {
  wchar_t result[MAX_PATH];
  if (PathCombineW(result, base.c_str(), relative.c_str())) {
    return std::wstring(result);
  }
  return L"";
}

}  // namespace path

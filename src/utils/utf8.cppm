/// @Author: caomengxuan666
/// @contributors:
///   - contributor1 <email1@example.com>
///   - contributor2 <email2@example.com>
///   - contributor3 <email3@example.com>
///   - description
/// @Description: UTF-8 utilities
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
module;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

export module utils:utf8;

import std;

/**
 * @brief Convert UTF-8 string to wide string
 * @param utf8 UTF-8 string
 * @return Wide string
 */
export std::wstring utf8_to_wstring(const std::string_view& utf8) {
    if (utf8.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
    std::wstring wide(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), wide.data(), size_needed);
    return wide;
}

/**
 * @brief Convert wide string to UTF-8 string
 * @param wide Wide string
 * @return UTF-8 string
 */
export std::string wstring_to_utf8(const std::wstring_view& wide) {
    if (wide.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
    std::string utf8(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), utf8.data(), size_needed, nullptr, nullptr);
    return utf8;
}

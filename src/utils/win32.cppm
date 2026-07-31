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
 */
module;

#include "pch/pch.h"

export module utils:win32;

import std;
import :utf8;

export auto quote_windows_command_arg(std::wstring_view arg) -> std::wstring {
  if (arg.empty()) return L"\"\"";

  const bool need_quote =
      arg.find_first_of(L" \t\"") != std::wstring_view::npos;
  if (!need_quote) return std::wstring(arg);

  std::wstring out = L"\"";
  size_t backslashes = 0;
  for (wchar_t c : arg) {
    if (c == L'\\') {
      ++backslashes;
    } else if (c == L'"') {
      out.append(backslashes * 2 + 1, L'\\');
      out.push_back(L'"');
      backslashes = 0;
    } else {
      out.append(backslashes, L'\\');
      backslashes = 0;
      out.push_back(c);
    }
  }
  out.append(backslashes * 2, L'\\');
  out.push_back(L'"');
  return out;
}

export auto append_windows_command_arg(std::wstring& command_line,
                                       std::wstring_view arg) -> void {
  if (!command_line.empty()) command_line.push_back(L' ');
  command_line += quote_windows_command_arg(arg);
}

export auto build_windows_command_line(std::span<const std::string_view> args)
    -> std::wstring {
  std::wstring out;
  for (auto arg : args) {
    append_windows_command_arg(out, utf8_to_wstring(std::string(arg)));
  }
  return out;
}

export struct Win32ErrorTextOptions {
  bool file_exists = false;
  bool bad_exe_format_as_permission = false;
  bool privilege_not_held_as_not_permitted = false;
  bool invalid_name_as_missing = false;
};

export auto win32_posix_error_text(unsigned long error,
                                   Win32ErrorTextOptions options = {})
    -> std::string {
  switch (error) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
      return "No such file or directory";
    case ERROR_INVALID_NAME:
      if (options.invalid_name_as_missing) return "No such file or directory";
      break;
    case ERROR_FILE_EXISTS:
    case ERROR_ALREADY_EXISTS:
      if (options.file_exists) return "File exists";
      break;
    case ERROR_ACCESS_DENIED:
      return "Permission denied";
    case ERROR_BAD_EXE_FORMAT:
      if (options.bad_exe_format_as_permission) return "Permission denied";
      break;
    case ERROR_PRIVILEGE_NOT_HELD:
      if (options.privilege_not_held_as_not_permitted) {
        return "Operation not permitted";
      }
      break;
    case ERROR_INVALID_PARAMETER:
      return "Invalid argument";
    default:
      break;
  }
  return std::system_category().message(static_cast<int>(error));
}

export auto win32_system_error_text(unsigned long error) -> std::string {
  LPWSTR buffer = nullptr;
  DWORD size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                  FORMAT_MESSAGE_FROM_SYSTEM |
                                  FORMAT_MESSAGE_IGNORE_INSERTS,
                              nullptr, static_cast<DWORD>(error), 0,
                              reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
  if (size == 0 || buffer == nullptr) {
    return std::system_category().message(static_cast<int>(error));
  }

  std::wstring text(buffer, size);
  LocalFree(buffer);
  while (!text.empty() && (text.back() == L'\r' || text.back() == L'\n' ||
                           text.back() == L' ' || text.back() == L'\t')) {
    text.pop_back();
  }
  return wstring_to_utf8(text);
}

/// @Author: caomengxuan666
/// @contributors:
///   - contributor1 <email1@example.com>
///   - contributor2 <email2@example.com>
///   - contributor3 <email3@example.com>
///   - description
/// @Description: Console utilities
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
module;

#define WIN32_LEAN_AND_MEAN
#include "pch/pch.h"

export module utils:console;

import std;

/// @brief ANSI escape sequences for terminal text coloring.
/// These follow the default GNU `ls --color=auto` scheme and are compatible
/// with modern terminals (Windows Terminal, iTerm2, GNOME Terminal, etc.).
/// Usage: prefix filename with color constant, suffix with COLOR_RESET.
export constexpr auto COLOR_RESET = L"\033[0m";    ///< Reset all attributes
export constexpr auto COLOR_DIR = L"\033[01;34m";  ///< Directories (bold blue)
export constexpr auto COLOR_EXEC =
    L"\033[01;32m";  ///< Executables (bold green)
export constexpr auto COLOR_LINK =
    L"\033[01;36m";                             ///< Symbolic links (bold cyan)
export constexpr auto COLOR_FILE = L"\033[0m";  ///< Regular files (default)
export constexpr auto COLOR_ARCHIVE =
    L"\033[01;31m";  ///< Archives: .zip, .tar, .7z (bold red)
export constexpr auto COLOR_SCRIPT =
    L"\033[01;33m";  ///< Scripts: .ps1, .sh, .py (bold yellow)
export constexpr auto COLOR_SOURCE =
    L"\033[01;36m";  ///< Source code: .cpp, .rs, .ts (bold cyan)
export constexpr auto COLOR_MEDIA =
    L"\033[01;35m";  ///< Media: .jpg, .mp4 (bold magenta)

/**
 * @brief Check if output is to console (not pipe/file)
 * @return true if output is console, false if pipe/file
 */
export bool isOutputConsole() {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD mode;
  return GetConsoleMode(hOut, &mode);  // Pipe/file returns false
}

/**
 * @brief Convert wide string to UTF-8 narrow string
 * @param wstr Wide string to convert
 * @return UTF-8 narrow string
 */
export std::string wstringToUtf8(const std::wstring &wstr) {
  if (wstr.empty()) {
    return "";
  }

  int bufSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                                    static_cast<int>(wstr.size()), nullptr, 0,
                                    nullptr, nullptr);

  if (bufSize <= 0) {
    return "";
  }

  std::string utf8Str(bufSize, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()),
                      &utf8Str[0], bufSize, nullptr, nullptr);

  return utf8Str;
}

/**
 * @brief Safe print function (auto adapt console/pipe)
 * @param wstr Wide string to print
 */
export void safePrint(const std::wstring &wstr) {
  if (isOutputConsole()) {
    // Console: wide char output (Chinese friendly)
    wprintf(L"%ls", wstr.c_str());
  } else {
    // Pipe/file: UTF-8 narrow char output (findstr friendly)
    printf("%s", wstringToUtf8(wstr).c_str());
  }
}

/**
 * @brief Safe print with newline (auto adapt console/pipe)
 * @param wstr Wide string to print
 */
export void safePrintLn(const std::wstring &wstr) { safePrint(wstr + L"\n"); }

/**
 * @brief Safe error print (stderr, auto adapt console/pipe, Chinese friendly)
 * @param wstr Wide string to print to stderr
 */
export void safeErrorPrint(const std::wstring &wstr) {
  if (isOutputConsole()) {
    // For console output: wide char output (Chinese friendly)
    fwprintf(stderr, L"%ls", wstr.c_str());
  } else {
    // For pipe/file output: UTF-8 narrow char output (pipe friendly)
    fprintf(stderr, "%s", wstringToUtf8(wstr).c_str());
  }
}

/**
 * @brief Safe error print with newline (stderr, auto adapt)
 * @param wstr Wide string to print to stderr
 */
export void safeErrorPrintLn(const std::wstring &wstr) {
  safeErrorPrint(wstr + L"\n");
}

/**
 * @brief Check if terminal supports color
 * @return true if terminal supports color, false otherwise
 */
export bool isTerminalSupportsColor() {
  // No color in pipe/file
  if (!isOutputConsole()) {
    return false;
  }

  DWORD consoleMode;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole == INVALID_HANDLE_VALUE) {
    return false;
  }
  if (!GetConsoleMode(hConsole, &consoleMode)) {
    return false;
  }

  // Check if Windows Terminal/CMD supports ANSI color
  return (consoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

/**
 * @brief Get terminal width
 * @return Terminal width in columns
 */
export int getTerminalWidth() {
  if (!isOutputConsole()) {
    return 80;
  }  // Default width for pipe/file

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole == INVALID_HANDLE_VALUE) {
    return 80;
  }
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
    return 80;
  }

  return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

/**
 * @brief Smart set console mode (only enable wide char for console)
 * @return true if successful, false on error
 */
export bool setupConsoleForUnicode() {
  // Only set wide char mode for console (not pipe/file)
  if (!isOutputConsole()) {
    // Pipe/file: set UTF-8 code page
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    return true;
  }

  // Console: enable wide char mode for Chinese
  bool success = true;
  if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) {
    success = false;
  }

  if (_setmode(_fileno(stderr), _O_U16TEXT) == -1) {
    success = false;
  }

  // Enable ANSI color support for Windows Terminal/CMD
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole != INVALID_HANDLE_VALUE) {
    DWORD consoleMode;
    if (GetConsoleMode(hConsole, &consoleMode)) {
      SetConsoleMode(hConsole,
                     consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
  }

  return success;
}

/// @brief Calculate display width of a wide string in terminal columns.
/// Handles Unicode fullwidth/halfwidth characters (e.g., Chinese = 2 cols).
/// Returns number of *columns*, not code points.
/// @brief Calculate the terminal display width of a wide string.
/// Fullwidth characters (e.g., Chinese, Japanese) count as 2 columns.
/// Halfwidth characters (e.g., ASCII) count as 1 column.
export size_t string_display_width(const std::wstring &s) {
  size_t width = 0;
  for (wchar_t c : s) {
    // Check for fullwidth Unicode ranges
    if ((c >= 0x3000 && c <= 0x303F) ||    // CJK Symbols and Punctuation
        (c >= 0xFF00 && c <= 0xFFEF) ||    // Fullwidth forms
        (c >= 0x4E00 && c <= 0x9FFF) ||    // CJK Unified Ideographs
        (c >= 0x3400 && c <= 0x4DBF) ||    // CJK Extension A
        (c >= 0x20000 && c <= 0x2A6DF) ||  // CJK Extension B
        (c >= 0x2A700 && c <= 0x2B73F) ||  // CJK Extension C
        (c >= 0x2B740 && c <= 0x2B81F) ||  // CJK Extension D
        (c >= 0x2B820 && c <= 0x2CEAF) ||  // CJK Extension E/F/G
        (c >= 0xAC00 && c <= 0xD7AF) ||    // Hangul Syllables
        (c >= 0x1100 && c <= 0x11FF)) {
      // Hangul Jamo
      width += 2;
    } else {
      width += 1;
    }
  }
  return width;
}

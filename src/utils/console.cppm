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
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#include <cstdio>

export module utils:console;

import std;

// Color definitions for terminal output
export constexpr auto COLOR_RESET = L"\033[0m";
export constexpr auto COLOR_DIR = L"\033[01;34m";
export constexpr auto COLOR_LINK = L"\033[01;36m";
export constexpr auto COLOR_EXEC = L"\033[01;32m";
export constexpr auto COLOR_FILE = L"\033[0m";

/**
 * @brief Check if terminal supports color
 * @return true if terminal supports color, false otherwise
 */
export bool isTerminalSupportsColor() {
    DWORD consoleMode;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return false;
    if (!GetConsoleMode(hConsole, &consoleMode)) return false;
    return true;
}

/**
 * @brief Get terminal width
 * @return Terminal width in columns
 */
export int getTerminalWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return 80;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return 80;
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

/**
 * @brief Set console to wide character mode to support Chinese characters
 * @return true if successful, false on error
 */
export bool setConsoleToWideCharMode() {
  // Set stdout to wide character mode
  if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) {
    return false;
  }

  // Set stderr to wide character mode
  if (_setmode(_fileno(stderr), _O_U16TEXT) == -1) {
    return false;
  }

  return true;
}

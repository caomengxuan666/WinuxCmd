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
#include <windows.h>

#include <cstdio>
#include <fcntl.h>
#include <io.h>

export module utils:console;

import std;

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

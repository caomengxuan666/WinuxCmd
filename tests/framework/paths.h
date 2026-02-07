#pragma once

#include <filesystem>
#include <string>

// Windows only for now
std::filesystem::path get_current_exe_dir();

struct ProjectPaths {
  static std::filesystem::path detect_bin_dir() {
#ifdef WINUXCMD_BIN_DIR
    return std::filesystem::path(WINUXCMD_BIN_DIR);
#else
    return get_current_exe_dir().parent_path();
#endif
  }

  static std::filesystem::path exe(const std::wstring& name) {
    return detect_bin_dir() / name;
  }
};

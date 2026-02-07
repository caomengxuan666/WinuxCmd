#include <windows.h>

#include <stdexcept>

#include "framework/paths.h"

std::filesystem::path get_current_exe_dir() {
  wchar_t buf[MAX_PATH];
  DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
  if (len == 0 || len == MAX_PATH) {
    throw std::runtime_error("GetModuleFileNameW failed");
  }
  return std::filesystem::path(buf).parent_path();
}

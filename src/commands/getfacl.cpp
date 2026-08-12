/*
 *  Copyright © 2026 WinuxCmd
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: getfacl.cpp
 *  - CopyrightYear: 2026
 */
/// @Description: Read Windows file ACLs in a stable text format.
/// @Version: 0.1.0
/// @License: MIT

#include <aclapi.h>

#include "core/command_macros.h"
#include "pch/pch.h"

#pragma comment(lib, "advapi32.lib")

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr GETFACL_OPTIONS = std::array{
    OPTION("-p", "--absolute-names", "do not strip leading path separators"),
    OPTION("-n", "--numeric", "print numeric IDs instead of account names")};

namespace getfacl_pipeline {

struct Config {
  bool absolute_names = false;
  bool numeric = false;
};

auto display_path(std::string path, const Config& cfg) -> std::string {
  if (cfg.absolute_names) return path;
  while (!path.empty() && (path.front() == '/' || path.front() == '\\')) {
    path.erase(path.begin());
  }
  return path;
}

auto account_display(PSID sid, const Config& cfg) -> std::string {
  if (sid == nullptr) return {};
  std::string id = win32_account_id_from_sid(sid);
  if (cfg.numeric) return id.empty() ? "0" : id;
  std::string name = win32_account_name_from_sid(sid);
  return name.empty() ? (id.empty() ? "UNKNOWN" : id) : name;
}

auto access_mask_to_rwx(ACCESS_MASK mask) -> std::string {
  const bool read = (mask & (FILE_GENERIC_READ | GENERIC_READ | FILE_READ_DATA |
                             FILE_READ_ATTRIBUTES | FILE_READ_EA)) != 0;
  const bool write =
      (mask & (FILE_GENERIC_WRITE | GENERIC_WRITE | FILE_WRITE_DATA |
               FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA |
               DELETE | WRITE_DAC | WRITE_OWNER)) != 0;
  const bool exec =
      (mask & (FILE_GENERIC_EXECUTE | GENERIC_EXECUTE | FILE_EXECUTE)) != 0;
  std::string out;
  out.push_back(read ? 'r' : '-');
  out.push_back(write ? 'w' : '-');
  out.push_back(exec ? 'x' : '-');
  return out;
}

auto print_ace(ACL* acl, DWORD index, const Config& cfg) -> bool {
  void* raw_ace = nullptr;
  if (!GetAce(acl, index, &raw_ace) || raw_ace == nullptr) return false;

  auto* header = reinterpret_cast<ACE_HEADER*>(raw_ace);
  ACCESS_MASK mask = 0;
  PSID sid = nullptr;
  std::string prefix;

  if (header->AceType == ACCESS_ALLOWED_ACE_TYPE) {
    auto* ace = reinterpret_cast<ACCESS_ALLOWED_ACE*>(raw_ace);
    mask = ace->Mask;
    sid = &ace->SidStart;
    prefix = "user";
  } else if (header->AceType == ACCESS_DENIED_ACE_TYPE) {
    auto* ace = reinterpret_cast<ACCESS_DENIED_ACE*>(raw_ace);
    mask = ace->Mask;
    sid = &ace->SidStart;
    prefix = "deny";
  } else {
    return true;
  }

  safePrintLn(prefix + ":" + account_display(sid, cfg) + ":" +
              access_mask_to_rwx(mask));
  return true;
}

auto print_acl_for_file(const std::string& path, const Config& cfg) -> int {
  std::wstring wpath = utf8_to_wstring(path);
  PSECURITY_DESCRIPTOR security_desc = nullptr;
  PSID owner_sid = nullptr;
  PSID group_sid = nullptr;
  ACL* dacl = nullptr;

  const DWORD status = GetNamedSecurityInfoW(
      const_cast<wchar_t*>(wpath.c_str()), SE_FILE_OBJECT,
      OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION |
          DACL_SECURITY_INFORMATION,
      &owner_sid, &group_sid, &dacl, nullptr, &security_desc);
  if (status != ERROR_SUCCESS) {
    if (security_desc != nullptr) LocalFree(security_desc);
    safeErrorPrintLn("getfacl: " + path + ": " +
                     win32_posix_error_text(status));
    return 1;
  }

  safePrintLn("# file: " + display_path(path, cfg));
  safePrintLn("# owner: " + account_display(owner_sid, cfg));
  safePrintLn("# group: " + account_display(group_sid, cfg));

  if (dacl == nullptr) {
    safePrintLn("user::rwx");
    safePrintLn("group::rwx");
    safePrintLn("other::rwx");
  } else {
    for (DWORD i = 0; i < dacl->AceCount; ++i) {
      if (!print_ace(dacl, i, cfg)) {
        if (security_desc != nullptr) LocalFree(security_desc);
        safeErrorPrintLn("getfacl: " + path + ": cannot read ACL entry");
        return 1;
      }
    }
  }

  if (security_desc != nullptr) LocalFree(security_desc);
  return 0;
}

auto run(const CommandContext<GETFACL_OPTIONS.size()>& ctx) -> int {
  Config cfg{.absolute_names = ctx.get<bool>("-p", false) ||
                               ctx.get<bool>("--absolute-names", false),
             .numeric = ctx.get<bool>("-n", false) ||
                        ctx.get<bool>("--numeric", false)};
  if (ctx.positionals.empty()) {
    safeErrorPrintLn("getfacl: missing file operand");
    return 1;
  }

  int status = 0;
  for (auto file : ctx.positionals) {
    if (print_acl_for_file(std::string(file), cfg) != 0) status = 1;
  }
  return status;
}

}  // namespace getfacl_pipeline

REGISTER_COMMAND(getfacl, "getfacl", "getfacl [OPTION]... FILE...",
                 "Print Windows file ACLs in a stable text format.",
                 "  getfacl file.txt\n"
                 "  getfacl --numeric file.txt",
                 "chown(1), chgrp(1), chmod(1)", "WinuxCmd",
                 "Copyright © 2026 WinuxCmd", GETFACL_OPTIONS) {
  return getfacl_pipeline::run(ctx);
}

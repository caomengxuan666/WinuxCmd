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
 *  - File: users.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for users.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
// include other header after pch.h
#include <wtsapi32.h>  // For WTSEnumerateSessionsW, WTSQuerySessionInformationW

#include "core/command_macros.h"

#pragma comment(lib, "wtsapi32.lib")

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr USERS_OPTIONS = std::array{
    // [EXT] option
    OPTION("", "", "print who is currently logged in", STRING_TYPE)
    // users has no options
};

namespace users_pipeline {
namespace cp = core::pipeline;

struct Config {
  // No options
};

auto build_config(const CommandContext<USERS_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  return cfg;
}

auto run(const Config& cfg) -> int {
  // Enumerate all terminal sessions on the local machine
  PWTS_SESSION_INFOW pSessionInfo = nullptr;
  DWORD sessionCount = 0;

  if (!WTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo,
                             &sessionCount)) {
    // Fallback to current user if enumeration fails
    WCHAR username[256];
    DWORD username_size = 256;

    if (!GetUserNameW(username, &username_size)) {
      return 1;
    }

    std::wstring ws(username);
    std::string user_str = wstring_to_utf8(ws);
    safePrintLn(user_str);
    return 0;
  }

  // Collect unique usernames from active sessions
  std::vector<std::string> users;
  for (DWORD i = 0; i < sessionCount; ++i) {
    if (pSessionInfo[i].State != WTSActive) {
      continue;
    }

    LPWSTR buffer = nullptr;
    DWORD bufferSize = 0;
    if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,
                                    pSessionInfo[i].SessionId, WTSUserName,
                                    &buffer, &bufferSize) &&
        buffer != nullptr && bufferSize > sizeof(wchar_t)) {
      std::wstring ws(buffer);
      std::string user = wstring_to_utf8(ws);
      if (!user.empty()) {
        // Avoid duplicates
        bool duplicate = false;
        for (const auto& existing : users) {
          if (existing == user) {
            duplicate = true;
            break;
          }
        }
        if (!duplicate) {
          users.push_back(std::move(user));
        }
      }
      WTSFreeMemory(buffer);
    }
  }

  WTSFreeMemory(pSessionInfo);

  if (users.empty()) {
    // Fallback to current user if no active sessions found
    WCHAR username[256];
    DWORD username_size = 256;

    if (!GetUserNameW(username, &username_size)) {
      return 1;
    }

    std::wstring ws(username);
    std::string user_str = wstring_to_utf8(ws);
    safePrintLn(user_str);
    return 0;
  }

  for (const auto& user : users) {
    safePrintLn(user);
  }

  return 0;
}

}  // namespace users_pipeline

REGISTER_COMMAND(
    users, "users", "users [OPTION]...",
    "Print the user names of users currently logged in to the current host.",
    "  users", "who(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
    USERS_OPTIONS) {
  using namespace users_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"users");
    return 1;
  }

  return run(*cfg_result);
}

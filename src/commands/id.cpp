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
 *  - File: id.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
/// @contributors:
///   - caomengxuan666 <2507560089@qq.com>
/// @Description: Implementation for id.
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd

#include "pch/pch.h"
// include other header after pch.h
#include "core/command_macros.h"

import std;
import core;
import utils;
import container;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

auto constexpr ID_OPTIONS = std::array{
    OPTION("-a", "", "ignore, for compatibility with other versions",
           BOOL_TYPE),
    OPTION("-g", "--group", "print only the effective group ID", BOOL_TYPE),
    OPTION("-G", "--groups", "print all group IDs", BOOL_TYPE),
    OPTION("-n", "--name", "print a name instead of a number", BOOL_TYPE),
    OPTION("-r", "--real", "print the real ID instead of the effective ID",
           BOOL_TYPE),
    OPTION("-u", "--user", "print only the effective user ID", BOOL_TYPE),
    OPTION("-Z", "--context",
           "print only the security context (not implemented)", BOOL_TYPE),
    OPTION("", "--zero", "delimit entries with NUL, not whitespace",
           BOOL_TYPE)};

namespace id_pipeline {
namespace cp = core::pipeline;

using AccountInfo = Win32AccountInfo;

struct ProcessIdentity {
  AccountInfo user;
  SmallVector<AccountInfo, 16> groups;
};

struct Config {
  bool print_group = false;
  bool print_groups = false;
  bool print_name = false;
  bool print_real = false;
  bool print_user = false;
  bool print_context = false;
  bool zero = false;
  SmallVector<std::string, 64> users;
};

auto current_identity() -> std::optional<ProcessIdentity> {
  HANDLE raw_token = nullptr;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &raw_token))
    return std::nullopt;
  UniqueHandle token(raw_token);

  ProcessIdentity identity;
  auto user_data = win32_token_information(token.get(), TokenUser);
  if (user_data.empty()) return std::nullopt;

  auto* token_user = reinterpret_cast<TOKEN_USER*>(user_data.data());
  identity.user = win32_account_from_sid(token_user->User.Sid);
  if (identity.user.name.empty()) {
    identity.user.name = win32_current_username();
  }

  identity.groups.push_back(identity.user);
  auto groups_data = win32_token_information(token.get(), TokenGroups);
  if (!groups_data.empty()) {
    auto* token_groups = reinterpret_cast<TOKEN_GROUPS*>(groups_data.data());
    for (DWORD i = 0; i < token_groups->GroupCount; ++i) {
      AccountInfo group = win32_account_from_sid(token_groups->Groups[i].Sid);
      if (group.id.empty()) continue;
      bool duplicate = false;
      for (const auto& existing : identity.groups) {
        if (existing.id == group.id) {
          duplicate = true;
          break;
        }
      }
      if (!duplicate) identity.groups.push_back(std::move(group));
    }
  }

  return identity;
}

auto build_config(const CommandContext<ID_OPTIONS.size()>& ctx)
    -> cp::Result<Config> {
  Config cfg;
  cfg.print_group =
      ctx.get<bool>("--group", false) || ctx.get<bool>("-g", false);
  cfg.print_groups =
      ctx.get<bool>("--groups", false) || ctx.get<bool>("-G", false);
  cfg.print_name = ctx.get<bool>("--name", false) || ctx.get<bool>("-n", false);
  cfg.print_real = ctx.get<bool>("--real", false) || ctx.get<bool>("-r", false);
  cfg.print_user = ctx.get<bool>("--user", false) || ctx.get<bool>("-u", false);
  cfg.print_context =
      ctx.get<bool>("--context", false) || ctx.get<bool>("-Z", false);
  cfg.zero = ctx.get<bool>("--zero", false);

  for (auto arg : ctx.positionals) {
    cfg.users.push_back(std::string(arg));
  }

  return cfg;
}

auto format_account(const AccountInfo& account, bool use_name) -> std::string {
  if (use_name) return account.name.empty() ? account.id : account.name;
  return account.id;
}

auto format_full_account(const AccountInfo& account) -> std::string {
  std::string out = account.id;
  if (!account.name.empty()) {
    out += "(";
    out += account.name;
    out += ")";
  }
  return out;
}

auto join_groups(const SmallVector<AccountInfo, 16>& groups, bool use_name,
                 std::string_view delimiter) -> std::string {
  std::string out;
  for (size_t i = 0; i < groups.size(); ++i) {
    if (i > 0) out.append(delimiter);
    out += format_account(groups[i], use_name);
  }
  return out;
}

auto run(const Config& cfg) -> int {
  const int only_count =
      static_cast<int>(cfg.print_user) + static_cast<int>(cfg.print_group) +
      static_cast<int>(cfg.print_groups) + static_cast<int>(cfg.print_context);
  const bool default_format = only_count == 0;

  if (only_count > 1) {
    safeErrorPrintLn("id: cannot print only of more than one choice");
    return 1;
  }
  if (default_format && (cfg.print_real || cfg.print_name)) {
    safeErrorPrintLn(
        "id: cannot print only names or real IDs in default format");
    return 1;
  }
  if (default_format && cfg.zero) {
    safeErrorPrintLn("id: option --zero not permitted in default format");
    return 1;
  }
  if (cfg.print_context) {
    safeErrorPrintLn(
        "id: --context (-Z) works only on an SELinux-enabled kernel");
    return 1;
  }

  auto identity = current_identity();
  if (!identity) {
    safeErrorPrintLn("id: cannot get process identity");
    return 1;
  }

  if (!cfg.users.empty()) {
    bool ok = true;
    for (const auto& user : cfg.users) {
      if (!identity->user.name.empty() && user != identity->user.name &&
          user != identity->user.id) {
        safeErrorPrintLn("id: " + user + ": no such user");
        ok = false;
        continue;
      }

      Config one = cfg;
      one.users.clear();
      const int status = run(one);
      if (status != 0) ok = false;
    }
    return ok ? 0 : 1;
  }

  const std::string term =
      cfg.zero ? std::string(1, static_cast<char>(0)) : "\n";
  const std::string list_delim =
      cfg.zero ? std::string(1, static_cast<char>(0)) : " ";

  if (cfg.print_user) {
    safePrint(format_account(identity->user, cfg.print_name) + term);
    return 0;
  }

  if (cfg.print_group) {
    safePrint(format_account(identity->user, cfg.print_name) + term);
    return 0;
  }

  if (cfg.print_groups) {
    safePrint(join_groups(identity->groups, cfg.print_name, list_delim) + term);
    return 0;
  }

  std::string out = "uid=" + format_full_account(identity->user) +
                    " gid=" + format_full_account(identity->user) + " groups=";
  for (size_t i = 0; i < identity->groups.size(); ++i) {
    if (i > 0) out += ",";
    out += format_full_account(identity->groups[i]);
  }
  safePrint(out + term);
  return 0;
}

}  // namespace id_pipeline

REGISTER_COMMAND(
    id, "id", "id [OPTION]... [USER]",
    "Print user and group information for the specified USER,\n"
    "or (when USER omitted) for the current user.\n"
    "\n"
    "Note: This is a Windows implementation. Windows doesn't have\n"
    "POSIX UIDs/GIDs, so this command provides limited functionality.\n"
    "It mainly displays the username.",
    "  id\n"
    "  id -u\n"
    "  id -g\n"
    "  id -G",
    "groups(1), whoami(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
    ID_OPTIONS) {
  using namespace id_pipeline;

  auto cfg_result = build_config(ctx);
  if (!cfg_result) {
    cp::report_error(cfg_result, L"id");
    return 1;
  }

  return run(*cfg_result);
}

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
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: touch.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */

#include "core/command_macros.h"
#include "pch/pch.h"

import std;
import core;
import utils;

using cmd::meta::OptionMeta;
using cmd::meta::OptionType;

/**
 * @brief TOUCH command options definition
 *
 * This array defines all the options supported by the touch command.
 * Each option is described with its short form, long form, and description.
 * The implementation status is also indicated for each option.
 *
 * @par Options:
 *
 * - @a -a: Change only the access time [IMPLEMENTED]
 * - @a -c, @a --no-create: Do not create any files [IMPLEMENTED]
 * - @a -d, @a --date: Parse STRING and use it instead of current time [NOT SUPPORT]
 * - @a -f: (ignored) [IMPLEMENTED]
 * - @a -h, @a --no-dereference: Affect symbolic link instead of referenced file [NOT SUPPORT]
 * - @a -m: Change only the modification time [IMPLEMENTED]
 * - @a -r, @a --reference: Use this file's times instead of current time [IMPLEMENTED]
 * - @a -t: Use [[CC]YY]MMDDhhmm[.ss] instead of current time [NOT SUPPORT]
 * - @a --time: Change the specified time (access/atime/use/modify/mtime) [IMPLEMENTED]
 */
auto constexpr TOUCH_OPTIONS = std::array{
    OPTION("-a", "", "change only the access time"),
    OPTION("-c", "--no-create", "do not create any files"),
    OPTION("-d", "--date",
           "parse STRING and use it instead of current time [NOT SUPPORT]",
           STRING_TYPE),
    OPTION("-f", "", "(ignored)"),
    OPTION("-h", "--no-dereference",
           "affect symbolic link instead of referenced file [NOT SUPPORT]"),
    OPTION("-m", "", "change only the modification time"),
    OPTION("-r", "--reference", "use this file's times instead of current time",
           STRING_TYPE),
    OPTION("-t", "",
           "use [[CC]YY]MMDDhhmm[.ss] instead of current time [NOT SUPPORT]",
           STRING_TYPE),
    OPTION("", "--time",
           "change the specified time (access/atime/use/modify/mtime)",
           STRING_TYPE)};

namespace touch_pipeline {
namespace cp = core::pipeline;

struct TimePair {
  FILETIME atime{};
  FILETIME mtime{};
};

auto read_times_from_file(const std::wstring& wpath)
    -> std::optional<TimePair> {
  HANDLE h =
      CreateFileW(wpath.c_str(), FILE_READ_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (h == INVALID_HANDLE_VALUE) return std::nullopt;

  FILETIME c{}, a{}, m{};
  bool ok = GetFileTime(h, &c, &a, &m) != 0;
  CloseHandle(h);
  if (!ok) return std::nullopt;

  return TimePair{a, m};
}

auto apply_touch_one(const std::string& path,
                     const CommandContext<TOUCH_OPTIONS.size()>& ctx,
                     bool update_access, bool update_modify, bool no_create,
                     const std::optional<TimePair>& ref_times) -> bool {
  std::wstring wpath = utf8_to_wstring(path);

  DWORD create_mode = no_create ? OPEN_EXISTING : OPEN_ALWAYS;
  HANDLE h =
      CreateFileW(wpath.c_str(), FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, create_mode, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (h == INVALID_HANDLE_VALUE) {
    DWORD e = GetLastError();
    if (no_create && (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND)) {
      return true;
    }
    safeErrorPrint("touch: cannot touch '");
    safeErrorPrint(path);
    safeErrorPrint("': No such file or directory\n");
    return false;
  }

  FILETIME c{}, cur_a{}, cur_m{};
  if (!GetFileTime(h, &c, &cur_a, &cur_m)) {
    CloseHandle(h);
    safeErrorPrint("touch: cannot touch '");
    safeErrorPrint(path);
    safeErrorPrint("'\n");
    return false;
  }

  FILETIME set_a = cur_a;
  FILETIME set_m = cur_m;

  if (ref_times.has_value()) {
    if (update_access) set_a = ref_times->atime;
    if (update_modify) set_m = ref_times->mtime;
  } else {
    FILETIME now{};
    GetSystemTimeAsFileTime(&now);
    if (update_access) set_a = now;
    if (update_modify) set_m = now;
  }

  FILETIME* pa = update_access ? &set_a : nullptr;
  FILETIME* pm = update_modify ? &set_m : nullptr;

  bool ok = SetFileTime(h, nullptr, pa, pm) != 0;
  CloseHandle(h);

  if (!ok) {
    safeErrorPrint("touch: cannot touch '");
    safeErrorPrint(path);
    safeErrorPrint("'\n");
    return false;
  }

  return true;
}

auto process_command(const CommandContext<TOUCH_OPTIONS.size()>& ctx)
    -> cp::Result<bool> {
  if (ctx.positionals.empty()) return std::unexpected("missing file operand");

  bool flag_a = ctx.get<bool>("-a", false);
  bool flag_m = ctx.get<bool>("-m", false);

  std::string time_word = ctx.get<std::string>("--time", "");
  if (!time_word.empty()) {
    if (time_word == "access" || time_word == "atime" || time_word == "use") {
      flag_a = true;
      flag_m = false;
    } else if (time_word == "modify" || time_word == "mtime") {
      flag_a = false;
      flag_m = true;
    }
  }

  bool update_access = flag_a;
  bool update_modify = flag_m;
  if (!flag_a && !flag_m) {
    update_access = true;
    update_modify = true;
  }

  bool no_create =
      ctx.get<bool>("--no-create", false) || ctx.get<bool>("-c", false);

  // parsed but currently not implemented on Windows in this command
  (void)ctx.get<std::string>("--date", "");
  (void)ctx.get<std::string>("-t", "");
  (void)ctx.get<bool>("--no-dereference", false);
  (void)ctx.get<bool>("-h", false);
  (void)ctx.get<bool>("-f", false);

  std::optional<TimePair> ref_times = std::nullopt;
  std::string ref_path = ctx.get<std::string>("--reference", "");
  if (ref_path.empty()) ref_path = ctx.get<std::string>("-r", "");
  if (!ref_path.empty()) {
    ref_times = read_times_from_file(utf8_to_wstring(ref_path));
    if (!ref_times.has_value()) {
      safeErrorPrint("touch: failed to get attributes of '");
      safeErrorPrint(ref_path);
      safeErrorPrint("'\n");
      return std::unexpected("reference file error");
    }
  }

  bool all_ok = true;
  for (auto p : ctx.positionals) {
    if (!apply_touch_one(std::string(p), ctx, update_access, update_modify,
                         no_create, ref_times)) {
      all_ok = false;
    }
  }

  return all_ok;
}
}  // namespace touch_pipeline

REGISTER_COMMAND(touch, "touch", "touch [OPTION]... FILE...",
                 "Update the access and modification times of each FILE to the "
                 "current time.\n"
                 "A FILE argument that does not exist is created empty, unless "
                 "-c is supplied.",
                 "  touch file.txt\n"
                 "  touch -a file.txt\n"
                 "  touch -m file.txt\n"
                 "  touch -c missing.txt\n"
                 "  touch -r ref.txt target.txt",
                 "stat(1), date(1)", "WinuxCmd", "Copyright © 2026 WinuxCmd",
                 TOUCH_OPTIONS) {
  using namespace touch_pipeline;
  auto result = process_command(ctx);
  if (!result) {
    cp::report_error(result, L"touch");
    return 1;
  }
  return *result ? 0 : 1;
}

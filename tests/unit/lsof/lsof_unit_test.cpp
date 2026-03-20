/*
 *  Copyright (c) 2026 WinuxCmd
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: lsof_unit_test.cpp
 *  - CopyrightYear: 2026
 */

#include "framework/winuxtest.h"

TEST(lsof, lsof_help) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"winuxcmd.exe", {L"lsof", L"--help"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("Usage:") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("OPTIONS") != std::string::npos);
}

TEST(lsof, lsof_basic_runs) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"winuxcmd.exe", {L"lsof"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("COMMAND") != std::string::npos);
}

TEST(lsof, lsof_filter_self_pid) {
  TempDir tmp;

  wchar_t pid_buf[32] = {};
  _snwprintf_s(pid_buf, std::size(pid_buf), _TRUNCATE, L"%lu",
               GetCurrentProcessId());

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"winuxcmd.exe", {L"lsof", L"--pid", pid_buf, L"--no-headers"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
}

TEST(lsof, lsof_invalid_option) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"winuxcmd.exe", {L"lsof", L"--definitely-invalid"});

  auto r = p.run();

  EXPECT_NE(r.exit_code, 0);
}

TEST(lsof, lsof_field_mode_runs) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"winuxcmd.exe", {L"lsof", L"-F"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
}

TEST(lsof, lsof_internet_filter_runs) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"winuxcmd.exe", {L"lsof", L"-i", L"--no-headers"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
}

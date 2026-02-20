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
 *  - File: kill_unit_test.cpp
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(kill, kill_list_signals) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"kill.exe", {L"-l"});

  TEST_LOG_CMD_LIST("kill.exe", L"-l");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -l output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  // Should list signals
  EXPECT_TRUE(r.stdout_text.find("HUP") != std::string::npos ||
              r.stdout_text.find("KILL") != std::string::npos);
}

TEST(kill, kill_list_specific_signal) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"kill.exe", {L"-l", L"KILL"});

  TEST_LOG_CMD_LIST("kill.exe", L"-l", L"KILL");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -l KILL output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  // Should show signal number for KILL
  EXPECT_TRUE(r.stdout_text.find("9") != std::string::npos);
}

TEST(kill, kill_invalid_pid) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"kill.exe", {L"999999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"999999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe invalid pid output", r.stdout_text);

  EXPECT_NE(r.exit_code, 0);
  // Should fail for non-existent PID
}

TEST(kill, kill_no_arguments) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"kill.exe", {});

  TEST_LOG_CMD_LIST("kill.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe no arguments output", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should fail when no PID provided
  EXPECT_TRUE(r.stderr_text.find("usage") != std::string::npos);
}

TEST(kill, kill_system_process) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"kill.exe", {L"4"});  // System process (should be protected)

  TEST_LOG_CMD_LIST("kill.exe", L"4");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe system process output", r.stderr_text);

  // Should fail when trying to kill system process
  EXPECT_TRUE(r.exit_code != 0 || r.stderr_text.find("cannot kill") != std::string::npos);
}
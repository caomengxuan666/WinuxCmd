/*
 *  Copyright © 2026 WinuxCmd
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
<<<<<<< HEAD
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
=======
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
<<<<<<< HEAD
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
=======
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
 *
 *  - File: kill_unit_test.cpp
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

<<<<<<< HEAD
TEST(kill, list_signals) {
  Pipeline p;
=======
TEST(kill, kill_list_signals) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
  p.add(L"kill.exe", {L"-l"});

  TEST_LOG_CMD_LIST("kill.exe", L"-l");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -l output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
<<<<<<< HEAD
  // Verify the output contains common signal names
  EXPECT_TRUE(r.stdout_text.find("HUP") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("INT") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("KILL") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("TERM") != std::string::npos);
}

TEST(kill, list_signals_long) {
  Pipeline p;
  p.add(L"kill.exe", {L"--list"});

  TEST_LOG_CMD_LIST("kill.exe", L"--list");
=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
<<<<<<< HEAD
  TEST_LOG("kill.exe --list output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("HUP") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("TERM") != std::string::npos);
}

TEST(kill, list_signals_table) {
  Pipeline p;
  p.add(L"kill.exe", {L"-L"});

  TEST_LOG_CMD_LIST("kill.exe", L"-L");
=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
<<<<<<< HEAD
  TEST_LOG("kill.exe -L output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  // Verify table format contains headers
  EXPECT_TRUE(r.stdout_text.find("Signal") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("Name") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("Description") != std::string::npos);
  // Verify some signal entries
  EXPECT_TRUE(r.stdout_text.find("KILL") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("TERM") != std::string::npos);
}

TEST(kill, list_signals_table_long) {
  Pipeline p;
  p.add(L"kill.exe", {L"--table"});

  TEST_LOG_CMD_LIST("kill.exe", L"--table");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe --table output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("Signal") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("Name") != std::string::npos);
}

TEST(kill, invalid_pid) {
  Pipeline p;
  p.add(L"kill.exe", {L"99999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"99999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe 99999999 stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should report error about invalid or non-existent process
  EXPECT_TRUE(r.stderr_text.find("kill:") != std::string::npos);
}

TEST(kill, invalid_signal_name) {
  Pipeline p;
  p.add(L"kill.exe", {L"-s", L"INVALID", L"1234"});

  TEST_LOG_CMD_LIST("kill.exe", L"-s", L"INVALID", L"1234");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -s INVALID stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should report unknown signal error
  EXPECT_TRUE(r.stderr_text.find("unknown signal") != std::string::npos ||
              r.stderr_text.find("invalid signal") != std::string::npos);
}

TEST(kill, invalid_signal_number) {
  Pipeline p;
  p.add(L"kill.exe", {L"-s", L"999", L"1234"});

  TEST_LOG_CMD_LIST("kill.exe", L"-s", L"999", L"1234");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -s 999 stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should report invalid signal number error
  EXPECT_TRUE(r.stderr_text.find("invalid signal") != std::string::npos);
}

TEST(kill, no_pid_specified) {
  Pipeline p;
=======
  TEST_LOG("kill.exe invalid pid output", r.stdout_text);

  EXPECT_NE(r.exit_code, 0);
  // Should fail for non-existent PID
}

TEST(kill, kill_no_arguments) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22
  p.add(L"kill.exe", {});

  TEST_LOG_CMD_LIST("kill.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
<<<<<<< HEAD
  TEST_LOG("kill.exe (no args) stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should report missing PID error
  EXPECT_TRUE(r.stderr_text.find("no process ID") != std::string::npos ||
              r.stderr_text.find("PID") != std::string::npos);
}

TEST(kill, invalid_pid_format) {
  Pipeline p;
  p.add(L"kill.exe", {L"abc"});

  TEST_LOG_CMD_LIST("kill.exe", L"abc");
=======
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
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
<<<<<<< HEAD
  TEST_LOG("kill.exe abc stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should report invalid PID error
  EXPECT_TRUE(r.stderr_text.find("invalid PID") != std::string::npos);
}

TEST(kill, kill_current_process) {
  // Test killing the current process (should succeed)
  Pipeline p;
  
  // Get current process ID
  DWORD current_pid = GetCurrentProcessId();
  std::wstring pid_str = std::to_wstring(current_pid);
  
  // Use signal 0 (check if process exists)
  p.add(L"kill.exe", {L"-s", L"0", pid_str});

  TEST_LOG_CMD_LIST("kill.exe", L"-s", L"0", pid_str.c_str());

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -s 0 output", r.stdout_text);

  // Signal 0 should succeed (just checks if process exists)
  EXPECT_EQ(r.exit_code, 0);
}

TEST(kill, kill_with_sigterm) {
  // Test with -15 (SIGTERM) flag on non-existent process
  Pipeline p;
  p.add(L"kill.exe", {L"-15", L"99999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"-15", L"99999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -15 stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  EXPECT_TRUE(r.stderr_text.find("kill:") != std::string::npos);
}

TEST(kill, kill_with_sigkill) {
  // Test with -9 (SIGKILL) flag on non-existent process
  Pipeline p;
  p.add(L"kill.exe", {L"-9", L"99999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"-9", L"99999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -9 stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  EXPECT_TRUE(r.stderr_text.find("kill:") != std::string::npos);
}

TEST(kill, multiple_pids) {
  // Test killing multiple PIDs (all invalid)
  Pipeline p;
  p.add(L"kill.exe", {L"99999998", L"99999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"99999998", L"99999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe multiple PIDs stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should report errors for both PIDs
  EXPECT_TRUE(r.stderr_text.find("99999998") != std::string::npos);
  EXPECT_TRUE(r.stderr_text.find("99999999") != std::string::npos);
}

TEST(kill, signal_by_name) {
  // Test using signal name instead of number
  Pipeline p;
  p.add(L"kill.exe", {L"-s", L"TERM", L"99999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"-s", L"TERM", L"99999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -s TERM stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should attempt to kill with SIGTERM (but PID doesn't exist)
  EXPECT_TRUE(r.stderr_text.find("kill:") != std::string::npos);
}

TEST(kill, signal_with_sig_prefix) {
  // Test using signal name with SIG prefix
  Pipeline p;
  p.add(L"kill.exe", {L"-s", L"SIGKILL", L"99999999"});

  TEST_LOG_CMD_LIST("kill.exe", L"-s", L"SIGKILL", L"99999999");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("kill.exe -s SIGKILL stderr", r.stderr_text);

  EXPECT_NE(r.exit_code, 0);
  // Should attempt to kill with SIGKILL (but PID doesn't exist)
  EXPECT_TRUE(r.stderr_text.find("kill:") != std::string::npos);
}
=======
  TEST_LOG("kill.exe system process output", r.stderr_text);

  // Should fail when trying to kill system process
  EXPECT_TRUE(r.exit_code != 0 || r.stderr_text.find("cannot kill") != std::string::npos);
}
>>>>>>> cfe75a2379e6cc46c4d539fe09ae96767cae7b22

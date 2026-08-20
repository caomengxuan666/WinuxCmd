/*
 *  Copyright © 2026 WinuxCmd
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
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: time_unit_test.cpp
 *  - CopyrightYear: 2026
 */

#include "framework/winuxtest.h"

TEST(time_cmd, time_preserves_exit_code) {
  Pipeline p;
  p.add(L"time.exe", {L"cmd.exe", L"/c", L"exit", L"7"});

  TEST_LOG_CMD_LIST("time.exe", L"cmd.exe", L"/c", L"exit", L"7");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("time.exe output", r.stderr_text);

  EXPECT_EQ(r.exit_code, 7);
  EXPECT_FALSE(r.stderr_text.empty());
  EXPECT_NE(r.stderr_text.find("real"), std::string::npos);
  EXPECT_NE(r.stderr_text.find("user"), std::string::npos);
  EXPECT_NE(r.stderr_text.find("sys"), std::string::npos);
}

TEST(time_cmd, time_posix_format) {
  Pipeline p;
  p.add(L"time.exe", {L"-p", L"cmd.exe", L"/c", L"exit", L"0"});

  TEST_LOG_CMD_LIST("time.exe", L"-p", L"cmd.exe", L"/c", L"exit", L"0");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("time.exe -p output", r.stderr_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stderr_text.empty());
  EXPECT_NE(r.stderr_text.find("real "), std::string::npos);
  EXPECT_NE(r.stderr_text.find("user "), std::string::npos);
  EXPECT_NE(r.stderr_text.find("sys "), std::string::npos);
}

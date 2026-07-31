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
 *  - File: yes_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(yes, yes_default) {
  Pipeline p;
  p.set_env(L"WINUXCMD_YES_REPEAT_LIMIT", L"5");
  p.add(L"yes.exe", {});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "y\ny\ny\ny\ny\n");
}

TEST(yes, yes_custom_string) {
  Pipeline p;
  p.set_env(L"WINUXCMD_YES_REPEAT_LIMIT", L"3");
  p.add(L"yes.exe", {L"hello"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "hello\nhello\nhello\n");
}

TEST(yes, yes_joins_all_arguments_with_spaces) {
  Pipeline p;
  p.set_env(L"WINUXCMD_YES_REPEAT_LIMIT", L"3");
  p.add(L"yes.exe", {L"a", L"bar", L"c"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "a bar c\na bar c\na bar c\n");
}

TEST(yes, yes_version_succeeds) {
  Pipeline p;
  p.add(L"yes.exe", {L"--version"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_NE(r.stdout_text.find("yes (WinuxCmd)"), std::string::npos);
  EXPECT_TRUE(r.stderr_text.empty());
}

TEST(yes, yes_double_dash_keeps_version_literal) {
  Pipeline p;
  p.set_env(L"WINUXCMD_YES_REPEAT_LIMIT", L"3");
  p.add(L"yes.exe", {L"--", L"--version"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "--version\n--version\n--version\n");
  EXPECT_TRUE(r.stderr_text.empty());
}

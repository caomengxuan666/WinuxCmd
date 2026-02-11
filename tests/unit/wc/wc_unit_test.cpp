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
 *  - File: wc_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(wc, wc_direct_input) {
  Pipeline p;
  p.set_stdin("hello\nworld\n");
  p.add(L"wc.exe", {L"-l"});

  TEST_LOG_CMD_LIST("wc.exe", L"-l");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  std::cout << "wc.exe -l direct test:" << std::endl;
  TEST_LOG("Output", r.stdout_text);

  Pipeline p2;
  p2.set_stdin("hello\nworld\n");
  p2.add(L"wc.exe", {});

  TEST_LOG_CMD_LIST("wc.exe");

  auto r2 = p2.run();

  TEST_LOG_EXIT_CODE(r2);
  TEST_LOG("wc.exe (no args) output", r2.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "2\n");
  EXPECT_EQ_TEXT(r2.stdout_text, "2 2 12\n");
}

TEST(wc, wc_with_options) {
  Pipeline p;
  p.set_stdin("hello\nworld\n");
  p.add(L"wc.exe", {L"-c"});

  TEST_LOG_CMD_LIST("wc.exe", L"-c");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("wc.exe -c output", r.stdout_text);

  Pipeline p2;
  p2.set_stdin("hello\nworld\n");
  p2.add(L"wc.exe", {L"-w"});

  TEST_LOG_CMD_LIST("wc.exe", L"-w");

  auto r2 = p2.run();

  TEST_LOG_EXIT_CODE(r2);
  TEST_LOG("wc.exe -w output", r2.stdout_text);

  Pipeline p3;
  p3.set_stdin("hello\nworld\n");
  p3.add(L"wc.exe", {L"-m"});

  TEST_LOG_CMD_LIST("wc.exe", L"-m");

  auto r3 = p3.run();

  TEST_LOG_EXIT_CODE(r3);
  TEST_LOG("wc.exe -m output", r3.stdout_text);

  Pipeline p4;
  p4.set_stdin("hello\nworld\n");
  p4.add(L"wc.exe", {L"-L"});

  TEST_LOG_CMD_LIST("wc.exe", L"-L");

  auto r4 = p4.run();

  TEST_LOG_EXIT_CODE(r4);
  TEST_LOG("wc.exe -L output", r4.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "12\n");
  EXPECT_EQ_TEXT(r2.stdout_text, "2\n");
  EXPECT_EQ_TEXT(r3.stdout_text, "12\n");
  EXPECT_EQ_TEXT(r4.stdout_text, "5\n");
}

TEST(wc, wc_combined_options) {
  Pipeline p;
  p.set_stdin("hello\nworld\n");
  p.add(L"wc.exe", {L"-l", L"-w", L"-c"});

  TEST_LOG_CMD_LIST("wc.exe", L"-l", L"-w", L"-c");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("wc.exe -l -w -c output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "2 2 12\n");
}

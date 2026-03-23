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
 *  - File: seq_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(seq, seq_single_arg) {
  Pipeline p;
  p.add(L"seq.exe", {L"5"});

  TEST_LOG_CMD_LIST("seq.exe", L"5");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("seq output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "1\n2\n3\n4\n5\n");
}

TEST(seq, seq_two_args) {
  Pipeline p;
  p.add(L"seq.exe", {L"1", L"5"});

  TEST_LOG_CMD_LIST("seq.exe", L"1", L"5");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("seq two args output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "1\n2\n3\n4\n5\n");
}

TEST(seq, seq_three_args) {
  Pipeline p;
  p.add(L"seq.exe", {L"1", L"2", L"10"});

  TEST_LOG_CMD_LIST("seq.exe", L"1", L"2", L"10");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("seq three args output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "1\n3\n5\n7\n9\n");
}

TEST(seq, seq_separator) {
  Pipeline p;
  p.add(L"seq.exe", {L"-s", L" ", L"1", L"5"});

  TEST_LOG_CMD_LIST("seq.exe", L"-s", L" ", L"1", L"5");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("seq separator output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "1 2 3 4 5\n");
}

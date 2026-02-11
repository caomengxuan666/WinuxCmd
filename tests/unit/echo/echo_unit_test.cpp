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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: echo_unit_test.cpp
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(echo, echo_basic) {
  Pipeline p;
  p.add(L"echo.exe", {L"hello", L"world"});

  TEST_LOG_CMD_LIST("echo.exe", L"hello", L"world");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "hello world\n");
}

TEST(echo, echo_no_newline) {
  Pipeline p;
  p.add(L"echo.exe", {L"-n", L"hello", L"world"});

  TEST_LOG_CMD_LIST("echo.exe", L"-n", L"hello", L"world");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "hello world");
}

TEST(echo, echo_uppercase) {
  Pipeline p;
  p.add(L"echo.exe", {L"-u", L"hello", L"world"});

  TEST_LOG_CMD_LIST("echo.exe", L"-u", L"hello", L"world");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "HELLO WORLD\n");
}

TEST(echo, echo_repeat) {
  Pipeline p;
  p.add(L"echo.exe", {L"--repeat", L"3", L"test"});

  TEST_LOG_CMD_LIST("echo.exe", L"--repeat", L"3", L"test");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "test\ntest\ntest\n");
}

TEST(echo, echo_escapes) {
  Pipeline p;
  p.add(L"echo.exe", {L"-e", L"line1\\nline2\\t tabbed"});

  TEST_LOG_CMD_LIST("echo.exe", L"-e", L"line1\\nline2\\t tabbed");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "line1\nline2\t tabbed\n");
}

TEST(echo, echo_suppress_escapes) {
  Pipeline p;
  p.add(L"echo.exe", {L"-E", L"line1\\nline2"});

  TEST_LOG_CMD_LIST("echo.exe", L"-E", L"line1\\nline2");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "line1\\nline2\n");
}

TEST(echo, echo_complex) {
  Pipeline p;
  p.add(L"echo.exe", {L"-n", L"-e", L"Hello\\tWorld!\\n"});

  TEST_LOG_CMD_LIST("echo.exe", L"-n", L"-e", L"Hello\\tWorld!\\n");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Output", r.stdout_text);

  EXPECT_EQ_TEXT(r.stdout_text, "Hello\tWorld!\n");
}

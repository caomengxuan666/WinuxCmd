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
 *  - File: tee_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(tee, tee_basic) {
  TempDir tmp;
  tmp.write("input.txt", "hello\nworld\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"input.txt"});
  p.add(L"tee.exe", {L"output.txt"});

  TEST_LOG_CMD_LIST("cat.exe", L"input.txt");
  TEST_LOG_CMD_LIST("tee.exe", L"output.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("tee output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "hello\nworld\n");

  // Check file was created
  std::string file_content = tmp.read("output.txt");
  EXPECT_EQ_TEXT(file_content, "hello\nworld\n");
}

TEST(tee, tee_append) {
  TempDir tmp;
  tmp.write("input.txt", "hello\n");
  tmp.write("output.txt", "initial\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"input.txt"});
  p.add(L"tee.exe", {L"-a", L"output.txt"});

  TEST_LOG_CMD_LIST("cat.exe", L"input.txt");
  TEST_LOG_CMD_LIST("tee.exe", L"-a", L"output.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("tee output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "hello\n");

  // Check file was appended to
  std::string file_content = tmp.read("output.txt");
  EXPECT_EQ_TEXT(file_content, "initial\nhello\n");
}

TEST(tee, tee_multiple_files) {
  TempDir tmp;
  tmp.write("input.txt", "hello\nworld\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"input.txt"});
  p.add(L"tee.exe", {L"output1.txt", L"output2.txt"});

  TEST_LOG_CMD_LIST("cat.exe", L"input.txt");
  TEST_LOG_CMD_LIST("tee.exe", L"output1.txt", L"output2.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("tee output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "hello\nworld\n");

  // Check both files were created
  std::string file1_content = tmp.read("output1.txt");
  std::string file2_content = tmp.read("output2.txt");
  EXPECT_EQ_TEXT(file1_content, "hello\nworld\n");
  EXPECT_EQ_TEXT(file2_content, "hello\nworld\n");
}

TEST(tee, tee_no_files) {
  Pipeline p;
  p.set_stdin("hello\nworld\n");
  p.add(L"tee.exe", {});

  TEST_LOG_CMD_LIST("tee.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("tee output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "hello\nworld\n");
}

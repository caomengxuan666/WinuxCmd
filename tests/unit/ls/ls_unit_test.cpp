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
 *  - File: ls_unit_test.cpp
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(ls, ls_basic) {
  TempDir tmp;
  tmp.write("file1.txt", "content1");
  tmp.write("file2.txt", "content2");

  TEST_LOG_FILE_CONTENT("file1.txt", "content1");
  TEST_LOG_FILE_CONTENT("file2.txt", "content2");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"ls.exe", {});

  TEST_LOG_CMD_LIST("ls.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("ls.exe output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  // Verify the output contains the expected files
  EXPECT_TRUE(r.stdout_text.find("file1.txt") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("file2.txt") != std::string::npos);
}

TEST(ls, ls_long_format) {
  TempDir tmp;
  tmp.write("file1.txt", "content1");

  TEST_LOG_FILE_CONTENT("file1.txt", "content1");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"ls.exe", {L"-l"});

  TEST_LOG_CMD_LIST("ls.exe", L"-l");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("ls.exe -l output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  // Verify the output contains the expected file in long format
  EXPECT_TRUE(r.stdout_text.find("file1.txt") != std::string::npos);
}

TEST(ls, ls_all) {
  TempDir tmp;
  tmp.write("file.txt", "content");
  tmp.write(".hidden.txt", "hidden content");

  TEST_LOG_FILE_CONTENT("file.txt", "content");
  TEST_LOG_FILE_CONTENT(".hidden.txt", "hidden content");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"ls.exe", {L"-a"});

  TEST_LOG_CMD_LIST("ls.exe", L"-a");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("ls.exe -a output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  // Verify the output contains both regular and hidden files
  EXPECT_TRUE(r.stdout_text.find("file.txt") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find(".hidden.txt") != std::string::npos);
}

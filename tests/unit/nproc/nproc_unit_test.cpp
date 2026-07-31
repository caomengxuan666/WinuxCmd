/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software", to
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
 *  - File: nproc_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(nproc, nproc_basic) {
  Pipeline p;
  p.add(L"nproc.exe", {});

  TEST_LOG_CMD_LIST("nproc.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("nproc output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
  int num = std::stoi(r.stdout_text);
  EXPECT_GT(num, 0);
}

TEST(nproc, nproc_all) {
  Pipeline p;
  p.add(L"nproc.exe", {L"--all"});

  TEST_LOG_CMD_LIST("nproc.exe", L"--all");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("nproc output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
  int num = std::stoi(r.stdout_text);
  EXPECT_GT(num, 0);
}

TEST(nproc, nproc_ignore) {
  Pipeline p;
  p.add(L"nproc.exe", {L"--ignore", L"1"});

  TEST_LOG_CMD_LIST("nproc.exe", L"--ignore", L"1");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("nproc output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
  int num = std::stoi(r.stdout_text);
  EXPECT_TRUE(num >= 1);
}

TEST(nproc, nproc_ignore_subtracts_processing_units) {
  Pipeline base;
  base.add(L"nproc.exe", {});
  auto base_result = base.run();
  EXPECT_EQ(base_result.exit_code, 0);
  if (base_result.exit_code != 0) return;
  int base_count = std::stoi(base_result.stdout_text);

  Pipeline ignored;
  ignored.add(L"nproc.exe", {L"--ignore", L"1"});
  auto ignored_result = ignored.run();

  EXPECT_EQ(ignored_result.exit_code, 0);
  int ignored_count = std::stoi(ignored_result.stdout_text);
  EXPECT_EQ(ignored_count, std::max(1, base_count - 1));
}

TEST(nproc, nproc_ignore_equals_form_subtracts_processing_units) {
  Pipeline base;
  base.add(L"nproc.exe", {});
  auto base_result = base.run();
  EXPECT_EQ(base_result.exit_code, 0);
  if (base_result.exit_code != 0) return;
  int base_count = std::stoi(base_result.stdout_text);

  Pipeline ignored;
  ignored.add(L"nproc.exe", {L"--ignore=1"});
  auto ignored_result = ignored.run();

  EXPECT_EQ(ignored_result.exit_code, 0);
  int ignored_count = std::stoi(ignored_result.stdout_text);
  EXPECT_EQ(ignored_count, std::max(1, base_count - 1));
}

TEST(nproc, nproc_rejects_extra_operand_like_gnu) {
  Pipeline p;
  p.add(L"nproc.exe", {L"extra"});
  auto r = p.run();

  EXPECT_NE(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.empty());
  EXPECT_TRUE(r.stderr_text.find("extra operand") != std::string::npos);
}

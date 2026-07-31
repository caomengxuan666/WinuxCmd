/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(test_bracket, test_bracket_file_exists) {
  TempDir tmp;
  tmp.write("test.txt", "hello");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"[.exe", {L"-f", L"test.txt", L"]"});

  TEST_LOG_CMD_LIST("[.exe", L"-f", L"test.txt", L"]");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);

  EXPECT_EQ(r.exit_code, 0);
}

TEST(test_bracket, test_bracket_numeric_false_expression) {
  Pipeline p;
  p.add(L"[.exe", {L"12", L"-eq", L"13", L"]"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
}

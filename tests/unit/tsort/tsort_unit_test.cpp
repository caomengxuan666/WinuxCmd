/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(tsort, tsort_basic) {
  Pipeline p;
  p.set_stdin("a b\nb c\n");
  p.add(L"tsort.exe", {});

  TEST_LOG_CMD_LIST("tsort.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("tsort output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
}

TEST(tsort, tsort_respects_dependency_edges) {
  Pipeline p;
  p.set_stdin("c b\nb a\n");
  p.add(L"tsort.exe", {});
  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "c\nb\na\n");
}

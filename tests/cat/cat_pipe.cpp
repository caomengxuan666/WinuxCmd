#include "framework/winuxtest.h"

TEST(cat_pipe_wc) {
  Pipeline p;

  p.set_stdin("hello\nworld\n");
  p.add(L"cat.exe", {});
  p.add(L"wc.exe", {L"-l"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "2\n");
}

WC_TEST_MAIN();

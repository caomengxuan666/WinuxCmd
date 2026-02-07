#include "framework/winuxtest.h"

TEST(cat_basic_file) {
  TempDir tmp;
  tmp.write("a.txt", "hello\nworld\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"a.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "hello\nworld\n");
}

WC_TEST_MAIN();

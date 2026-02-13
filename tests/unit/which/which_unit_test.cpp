#include "framework/winuxtest.h"

TEST(which, which_finds_first_match) {
  TempDir tmp;
  tmp.write("tool.exe", "");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_env(L"PATH", tmp.wpath());
  p.add(L"which.exe", {L"tool"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("tool.exe") != std::string::npos);
}

TEST(which, which_all_lists_multiple) {
  TempDir tmp;
  tmp.write("a.exe", "");
  tmp.write("a.cmd", "");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_env(L"PATH", tmp.wpath());
  p.add(L"which.exe", {L"-a", L"a"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("a.exe") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("a.cmd") != std::string::npos);
}

TEST(which, which_missing_returns_nonzero) {
  TempDir tmp;
  tmp.write("present.exe", "");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_env(L"PATH", tmp.wpath());
  p.add(L"which.exe", {L"absent"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
}

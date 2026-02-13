#include "framework/winuxtest.h"

TEST(cut, cut_basic_fields_default_tab) {
  TempDir tmp;
  tmp.write("a.txt", "a\tb\tc\n1\t2\t3\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cut.exe", {L"-f", L"1,3", L"a.txt"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "a\tc\n1\t3\n");
}

TEST(cut, cut_with_delimiter_and_range) {
  TempDir tmp;
  tmp.write("a.txt", "x,y,z\nm,n,o\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cut.exe", {L"-d", L",", L"-f", L"2-3", L"a.txt"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "y,z\nn,o\n");
}

TEST(cut, cut_only_delimited_skips) {
  TempDir tmp;
  tmp.write("a.txt", "no_delim\nhas:delim\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cut.exe", {L"-d", L":", L"-f", L"2", L"-s", L"a.txt"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "delim\n");
}

TEST(cut, cut_zero_terminated) {
  TempDir tmp;
  std::string data = std::string("a:b\0c:d", 7);
  tmp.write_bytes("a.txt", std::vector<char>(data.begin(), data.end()));

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cut.exe", {L"-z", L"-d", L":", L"-f", L"2", L"a.txt"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, std::string("b\0d\0", 4));
}

TEST(cut, cut_unsupported_bytes) {
  TempDir tmp;
  tmp.write("a.txt", "abc\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cut.exe", {L"-b", L"1", L"a.txt"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 2);
}

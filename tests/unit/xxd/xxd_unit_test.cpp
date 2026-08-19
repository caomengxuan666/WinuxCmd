/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(xxd, default_file_format) {
  TempDir tmp;
  tmp.write("input.txt", "hello");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"xxd.exe", {L"input.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text,
                 "00000000: 6865 6c6c 6f                             hello\n");
}

TEST(xxd, custom_columns) {
  TempDir tmp;
  tmp.write("input.txt", "hell");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"xxd.exe", {L"-c", L"4", L"input.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "00000000: 6865 6c6c  hell\n");
}

TEST(xxd, plain_columns) {
  TempDir tmp;
  tmp.write("input.txt", "abcdefgh");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"xxd.exe", {L"-p", L"-c", L"4", L"input.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "61626364\n65666768\n");
}

TEST(xxd, plain_uppercase) {
  TempDir tmp;
  tmp.write_bytes("input.bin", {'\xAB', '\xCD', '\xEF'});

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"xxd.exe", {L"-p", L"-u", L"input.bin"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "ABCDEF\n");
}

TEST(xxd, reverse_plain) {
  TempDir tmp;
  tmp.write("hex.txt", "68656c6c6f");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"xxd.exe", {L"-r", L"-p", L"hex.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "hello");
}

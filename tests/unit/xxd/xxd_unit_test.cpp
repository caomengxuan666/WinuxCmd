/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(xxd, xxd_from_stdin_matches_vim_default_format) {
  Pipeline p;
  p.set_stdin("hello");
  p.add(L"xxd.exe", {});

  TEST_LOG_CMD_LIST("xxd.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("xxd output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text,
                 "00000000: 6865 6c6c 6f                             hello\n");
}

TEST(xxd, xxd_full_line_has_no_extra_padding_or_blank_line) {
  Pipeline p;
  p.set_stdin("0123456789abcdef");
  p.add(L"xxd.exe", {});

  TEST_LOG_CMD_LIST("xxd.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("xxd output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(
      r.stdout_text,
      "00000000: 3031 3233 3435 3637 3839 6162 6364 6566  0123456789abcdef\n");
}

TEST(xxd, xxd_reverse_decodes_hex_dump) {
  TempDir tmp;
  tmp.write("dump.txt", "00000000: 6865 6c6c 6f                             hello\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"xxd.exe", {L"-r", L"dump.txt"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "hello");
}

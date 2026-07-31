/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(od, od_basic) {
  Pipeline p;
  p.set_stdin("hello");
  p.add(L"od.exe", {L"-c"});

  TEST_LOG_CMD_LIST("od.exe", L"-c");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("od output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_hex_bytes_address_none_honors_limit) {
  Pipeline p;
  p.set_stdin("000000 alpha beta\n");
  p.add(L"od.exe", {L"-An", L"-tx1", L"-N16"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text,
                 " 30 30 30 30 30 30 20 61 6c 70 68 61 20 62 65 74\n");
}

TEST(od, od_hex_short_pads_partial_final_field_like_gnu) {
  Pipeline p;
  p.set_stdin("abc");
  p.add(L"od.exe", {L"-An", L"-tx2"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, " 6261 0063\n");
}

TEST(od, od_hex_ascii_trailer_only_when_z_suffix_requested) {
  Pipeline p;
  p.set_stdin("abcdef");
  p.add(L"od.exe", {L"-An", L"-tx1z", L"-N6"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(
      r.stdout_text,
      " 61 62 63 64 65 66                                >abcdef<\n");
}

TEST(od, od_hex) {
  Pipeline p;
  p.set_stdin("hello");
  p.add(L"od.exe", {L"-x"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  // Hex output should contain hex digits
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_octal) {
  Pipeline p;
  p.set_stdin("hello");
  p.add(L"od.exe", {L"-o"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_decimal) {
  Pipeline p;
  p.set_stdin("hello");
  p.add(L"od.exe", {L"-d"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_address_octal) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-A", L"o"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  // Octal addresses should start with 0
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_address_hex) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-A", L"x"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_address_decimal) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-A", L"d"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_address_none) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-A", L"n"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  // No address mode should not show addresses
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_skip_bytes) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-j", L"5"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_read_bytes) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-N", L"5"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(od, od_width) {
  Pipeline p;
  p.set_stdin("hello world");
  p.add(L"od.exe", {L"-w", L"4"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

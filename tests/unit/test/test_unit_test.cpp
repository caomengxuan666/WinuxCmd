/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(test, test_file_exists) {
  TempDir tmp;
  tmp.write("test.txt", "hello");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"test.exe", {L"-f", L"test.txt"});

  TEST_LOG_CMD_LIST("test.exe", L"-f", L"test.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("test stdout", r.stdout_text);
  TEST_LOG("test stderr", r.stderr_text);

  EXPECT_EQ(r.exit_code, 0);
}

TEST(test, test_file_not_exists) {
  Pipeline p;
  p.add(L"test.exe", {L"-f", L"nonexistent.txt"});

  TEST_LOG_CMD_LIST("test.exe", L"-f", L"nonexistent.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);

  EXPECT_EQ(r.exit_code, 1);
}

TEST(test, test_file_time_and_identity_operators) {
  TempDir tmp;
  tmp.write("old.txt", "old");
  tmp.write("new.txt", "new");
  std::error_code ec;
  auto old_time = std::filesystem::last_write_time(tmp.path / "old.txt", ec);
  std::filesystem::last_write_time(tmp.path / "new.txt",
                                   old_time + std::chrono::seconds(2), ec);

  Pipeline newer;
  newer.set_cwd(tmp.wpath());
  newer.add(L"test.exe", {L"new.txt", L"-nt", L"old.txt"});
  EXPECT_EQ(newer.run().exit_code, 0);

  Pipeline older;
  older.set_cwd(tmp.wpath());
  older.add(L"test.exe", {L"old.txt", L"-ot", L"new.txt"});
  EXPECT_EQ(older.run().exit_code, 0);

  Pipeline same;
  same.set_cwd(tmp.wpath());
  same.add(L"test.exe", {L"old.txt", L"-ef", L"old.txt"});
  EXPECT_EQ(same.run().exit_code, 0);
}

TEST(test, test_logical_precedence_and_parentheses) {
  Pipeline p;
  p.add(L"test.exe", {L"1", L"-eq", L"2", L"-o", L"3", L"-eq", L"3", L"-a",
                      L"4", L"-eq", L"4"});
  EXPECT_EQ(p.run().exit_code, 0);

  Pipeline grouped;
  grouped.add(L"test.exe", {L"(", L"1", L"-eq", L"2", L"-o", L"3", L"-eq", L"3",
                            L")", L"-a", L"4", L"-eq", L"4"});
  EXPECT_EQ(grouped.run().exit_code, 0);
}

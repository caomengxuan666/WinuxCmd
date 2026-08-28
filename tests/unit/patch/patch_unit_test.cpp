/*
 *  Copyright © 2026 [caomengxuan666]
 */
#include "framework/winuxtest.h"

TEST(patch, patch_basic) {
  TempDir tmp;
  tmp.write("file.txt", "hello\n");
  std::string patch_data =
      "--- file.txt\n+++ file.txt\n@@ -1,1 +1,1 @@\n-hello\n+world\n";

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_stdin(patch_data);
  p.add(L"patch.exe", {});

  TEST_LOG_CMD_LIST("patch.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("patch stdout", r.stdout_text);
  TEST_LOG("patch stderr", r.stderr_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(tmp.read("file.txt"), "world\n");
}

TEST(patch, patch_multiple_hunks_use_cumulative_offsets) {
  TempDir tmp;
  tmp.write("file.txt", "a\nb\nc\nd\n");
  std::string patch_data =
      "--- file.txt\t2026-08-13 00:00:00\n"
      "+++ file.txt\t2026-08-13 00:00:01\n"
      "@@ -1,1 +1,2 @@\n"
      " a\n"
      "+x\n"
      "@@ -3,1 +4,1 @@\n"
      "-c\n"
      "+z\n";
  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_stdin(patch_data);
  p.add(L"patch.exe", {});
  auto r = p.run();
  TEST_LOG("patch multi stdout", r.stdout_text);
  TEST_LOG("patch multi stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(tmp.read("file.txt"), "a\nx\nb\nz\nd\n");
}

TEST(patch, patch_directory_force_fuzz_and_quiet_options) {
  TempDir tmp;
  tmp.mkdir("src");
  tmp.write("src/file.txt", "hello\n");
  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_stdin("--- file.txt\n+++ file.txt\n@@ -1,1 +1,1 @@\n-hello\n+world\n");
  p.add(L"patch.exe",
        {L"--directory", L"src", L"--force", L"--fuzz=1", L"--quiet"});
  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.empty());
  EXPECT_TRUE(r.stderr_text.empty());
  EXPECT_EQ_TEXT(tmp.read("src/file.txt"), "world\n");
}

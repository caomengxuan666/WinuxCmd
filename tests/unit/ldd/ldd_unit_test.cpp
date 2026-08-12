#include "framework/winuxtest.h"

TEST(ldd, lists_imports_for_winuxcmd_exe) {
  Pipeline p;
  p.add(L"ldd.exe", {std::wstring(WINUXCMD_BIN_DIR) + L"/winuxcmd.exe"});

  auto r = p.run();

  ASSERT_EQ(r.exit_code, 0);
  ASSERT_FALSE(r.stdout_text.empty());
  EXPECT_CONTAINS(r.stdout_text, ".dll");
  EXPECT_CONTAINS(r.stdout_text, "=>");
}

TEST(ldd, name_mode_prints_only_dll_names) {
  Pipeline p;
  p.add(L"ldd.exe",
        {L"--name", std::wstring(WINUXCMD_BIN_DIR) + L"/winuxcmd.exe"});

  auto r = p.run();

  ASSERT_EQ(r.exit_code, 0);
  ASSERT_FALSE(r.stdout_text.empty());
  EXPECT_CONTAINS(r.stdout_text, ".dll");
  EXPECT_NOT_CONTAINS(r.stdout_text, "=>");
}

TEST(ldd, rejects_non_pe_file) {
  TempDir tmp;
  auto file = tmp.write_file("plain.txt", "not pe");

  Pipeline p;
  p.add(L"ldd.exe", {file.wstring()});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_CONTAINS(r.stderr_text, "not a PE executable");
}

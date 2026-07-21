/*
 *  Copyright (c) 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 */

#include "framework/winuxtest.h"

namespace {

auto build_winuxcmd_path() -> std::filesystem::path {
  return std::filesystem::path(WINUXCMD_BIN_DIR) / L"winuxcmd.exe";
}

auto current_arch_key() -> std::string {
#if defined(_M_ARM64) || defined(__aarch64__)
  return "windows-arm64";
#else
  return "windows-x64";
#endif
}

auto widen_ascii(const std::string& text) -> std::wstring {
  return std::wstring(text.begin(), text.end());
}

auto file_url(std::filesystem::path path) -> std::string {
  auto text = path.string();
  std::ranges::replace(text, '\\', '/');
  return "file://" + text;
}

auto same_file(const std::filesystem::path& a, const std::filesystem::path& b)
    -> bool {
  HANDLE ha =
      CreateFileW(a.wstring().c_str(), FILE_READ_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  HANDLE hb =
      CreateFileW(b.wstring().c_str(), FILE_READ_ATTRIBUTES,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (ha == INVALID_HANDLE_VALUE || hb == INVALID_HANDLE_VALUE) {
    if (ha != INVALID_HANDLE_VALUE) CloseHandle(ha);
    if (hb != INVALID_HANDLE_VALUE) CloseHandle(hb);
    return false;
  }
  BY_HANDLE_FILE_INFORMATION ia{};
  BY_HANDLE_FILE_INFORMATION ib{};
  bool ok = GetFileInformationByHandle(ha, &ia) != 0 &&
            GetFileInformationByHandle(hb, &ib) != 0;
  CloseHandle(ha);
  CloseHandle(hb);
  return ok && ia.dwVolumeSerialNumber == ib.dwVolumeSerialNumber &&
         ia.nFileIndexHigh == ib.nFileIndexHigh &&
         ia.nFileIndexLow == ib.nFileIndexLow;
}

}  // namespace

TEST(wpm, wpm_hardlink_entrypoint_reports_version) {
  Pipeline p;
  p.add(L"wpm.exe", {L"version"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("wpm 0.2.0") != std::string::npos);
}

TEST(wpm, wpm_links_list_includes_internal_tool) {
  Pipeline p;
  p.add(L"winuxcmd.exe", {L"wpm", L"links", L"list"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("wpm") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("ls") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("jq") == std::string::npos);
}

TEST(wpm, wpm_index_status_uses_builtin_index_offline) {
  TempDir tmp;
  Pipeline p;
  p.add(L"winuxcmd.exe", {L"wpm", L"index", L"status", L"--root", tmp.wpath()});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("WPM index") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("packages:") != std::string::npos);
}

TEST(wpm, wpm_links_rebuild_creates_internal_tool_hardlink) {
  TempDir tmp;
  auto root_exe = tmp.path / L"winuxcmd.exe";
  std::filesystem::copy_file(build_winuxcmd_path(), root_exe,
                             std::filesystem::copy_options::overwrite_existing);

  Pipeline p;
  p.add(L"winuxcmd.exe",
        {L"wpm", L"links", L"rebuild", L"--root", tmp.wpath()});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  auto wpm_exe = tmp.path / L"wpm.exe";
  EXPECT_TRUE(std::filesystem::exists(wpm_exe));
  EXPECT_TRUE(same_file(root_exe, wpm_exe));
}

TEST(wpm, wpm_links_rebuild_removes_legacy_jq_hardlink) {
  TempDir tmp;
  auto root_exe = tmp.path / L"winuxcmd.exe";
  auto legacy_jq = tmp.path / L"jq.exe";
  std::filesystem::copy_file(build_winuxcmd_path(), root_exe,
                             std::filesystem::copy_options::overwrite_existing);
  bool linked = CreateHardLinkW(legacy_jq.wstring().c_str(),
                                root_exe.wstring().c_str(), nullptr) != 0;
  EXPECT_TRUE(linked);
  if (!linked) return;

  Pipeline p;
  p.add(L"winuxcmd.exe",
        {L"wpm", L"links", L"rebuild", L"--root", tmp.wpath()});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(std::filesystem::exists(legacy_jq));
  EXPECT_TRUE(std::filesystem::exists(tmp.path / L"wpm.exe"));
}

TEST(wpm, wpm_index_update_uses_local_file_source) {
  TempDir tmp;
  const auto index_path = tmp.path / L"fixture-index.json";
  tmp.write("fixture-index.json",
            "{\n"
            "  \"schema\": 1,\n"
            "  \"name\": \"fixture\",\n"
            "  \"version\": \"fixture-1\",\n"
            "  \"packages\": [\n"
            "    {\"name\":\"fixture-tool\",\"version\":\"1.0.0\","
            "\"description\":\"Local fixture\",\"kind\":\"external\","
            "\"artifacts\":{}}\n"
            "  ]\n"
            "}\n");

  Pipeline add;
  add.add(L"winuxcmd.exe",
          {L"wpm", L"source", L"add", L"fixture",
           widen_ascii(file_url(index_path)), L"--root", tmp.wpath()});
  auto add_result = add.run();
  EXPECT_EQ(add_result.exit_code, 0);

  Pipeline use;
  use.add(L"winuxcmd.exe",
          {L"wpm", L"source", L"use", L"fixture", L"--root", tmp.wpath()});
  auto use_result = use.run();
  EXPECT_EQ(use_result.exit_code, 0);

  Pipeline update;
  update.add(L"winuxcmd.exe",
             {L"wpm", L"index", L"update", L"--root", tmp.wpath()});
  auto update_result = update.run();
  EXPECT_EQ(update_result.exit_code, 0);
  EXPECT_TRUE(update_result.stdout_text.find("index updated from fixture") !=
              std::string::npos);

  Pipeline info;
  info.add(L"winuxcmd.exe",
           {L"wpm", L"info", L"fixture-tool", L"--root", tmp.wpath()});
  auto info_result = info.run();
  EXPECT_EQ(info_result.exit_code, 0);
  EXPECT_TRUE(info_result.stdout_text.find("Version: 1.0.0") !=
              std::string::npos);
}

TEST(wpm, wpm_install_downloads_local_exe_with_sha256) {
  TempDir tmp;
  const auto artifact_path = tmp.path / L"source" / L"jq.exe";
  const auto index_path = tmp.path / L"fixture-install-index.json";
  const auto root_exe = tmp.path / L"winuxcmd.exe";
  const auto legacy_jq = tmp.path / L"jq.exe";
  std::filesystem::copy_file(build_winuxcmd_path(), root_exe,
                             std::filesystem::copy_options::overwrite_existing);
  bool linked = CreateHardLinkW(legacy_jq.wstring().c_str(),
                                root_exe.wstring().c_str(), nullptr) != 0;
  EXPECT_TRUE(linked);
  if (!linked) return;
  tmp.write("source/jq.exe", "external exe\n");

  std::string index_json =
      "{\n"
      "  \"schema\": 1,\n"
      "  \"name\": \"fixture\",\n"
      "  \"version\": \"fixture-install\",\n"
      "  \"packages\": [\n"
      "    {\n"
      "      \"name\": \"jq\",\n"
      "      \"version\": \"1.0.0\",\n"
      "      \"description\": \"Local fixture executable\",\n"
      "      \"kind\": \"external\",\n"
      "      \"artifacts\": {\n"
      "        \"" +
      current_arch_key() +
      "\": {\n"
      "          \"type\": \"exe\",\n"
      "          \"sha256\": "
      "\"5140f4f6bf8b5691b7bccc1c4f00a2027dae00b2110d38a1e090af291226f322\",\n"
      "          \"urls\": [\"" +
      file_url(artifact_path) +
      "\"],\n"
      "          \"files\": [{\"from\":\"jq.exe\",\"to\":\"jq.exe\"}]\n"
      "        }\n"
      "      }\n"
      "    }\n"
      "  ]\n"
      "}\n";
  tmp.write("fixture-install-index.json", index_json);

  Pipeline add;
  add.add(L"winuxcmd.exe",
          {L"wpm", L"source", L"add", L"fixture",
           widen_ascii(file_url(index_path)), L"--root", tmp.wpath()});
  EXPECT_EQ(add.run().exit_code, 0);

  Pipeline use;
  use.add(L"winuxcmd.exe",
          {L"wpm", L"source", L"use", L"fixture", L"--root", tmp.wpath()});
  EXPECT_EQ(use.run().exit_code, 0);

  Pipeline update;
  update.add(L"winuxcmd.exe",
             {L"wpm", L"index", L"update", L"--root", tmp.wpath()});
  EXPECT_EQ(update.run().exit_code, 0);

  Pipeline install;
  install.add(L"winuxcmd.exe",
              {L"wpm", L"install", L"jq", L"--root", tmp.wpath()});
  auto install_result = install.run();
  EXPECT_EQ(install_result.exit_code, 0);
  if (install_result.exit_code != 0) return;
  EXPECT_TRUE(install_result.stdout_text.find("installed jq") !=
              std::string::npos);
  EXPECT_EQ(tmp.read("jq.exe"), "external exe\n");
  EXPECT_FALSE(same_file(root_exe, legacy_jq));
}

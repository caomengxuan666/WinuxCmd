/*
 *  Copyright © 2026 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: cat_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(cat, cat_basic_file) {
  TempDir tmp;
  tmp.write("a.txt", "hello\nworld\n");

  TEST_LOG_FILE_CONTENT("a.txt", "hello\nworld\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"a.txt"});

  TEST_LOG_CMD_LIST("cat.exe", L"a.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("cat.exe output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "hello\nworld\n");
}

TEST(cat, cat_reads_utf8_filename) {
  TempDir tmp;
  const std::wstring name = L"\x6D4B\x8BD5.txt";
  {
    std::ofstream out(tmp.path / name, std::ios::binary);
    out << "utf8\n";
  }

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {name});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "utf8\n");
}

TEST(cat, cat_solo_test) {
  Pipeline p;
  p.set_stdin("hello\nworld\n");
  p.add(L"cat.exe", {});

  TEST_LOG_CMD_LIST("cat.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);

  TEST_LOG_HEX("cat.exe output", r.stdout_text);
  TEST_LOG("cat.exe output visible", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
}

TEST(cat, cat_pipe_wc) {
  Pipeline p;
  p.set_stdin("hello\nworld\n");
  p.add(L"cat.exe", {});
  p.add(L"wc.exe", {L"-l"});

  std::cout << "Pipeline steps:" << std::endl;
  std::cout << "  1. cat.exe (stdin -> stdout)" << std::endl;
  std::cout << "  2. wc.exe -l (stdin -> stdout)" << std::endl;

  TEST_LOG_CMD_LIST("cat.exe");
  TEST_LOG_CMD_LIST("wc.exe", L"-l");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("Pipeline output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "2\n");
}

TEST(cat, cat_wildcard) {
  TempDir tmp;
  tmp.write("file1.txt", "content1\n");
  tmp.write("file2.txt", "content2\n");
  tmp.write("other.log", "log content\n");

  TEST_LOG_FILE_CONTENT("file1.txt", "content1\n");
  TEST_LOG_FILE_CONTENT("file2.txt", "content2\n");
  TEST_LOG_FILE_CONTENT("other.log", "log content\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"*.txt"});

  TEST_LOG_CMD_LIST("cat.exe", L"*.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("cat.exe *.txt output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("content1") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("content2") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("log content") == std::string::npos);
}

TEST(cat, cat_trusts_shell_owned_glob_arguments) {
  TempDir tmp;
  tmp.write("file.txt", "content\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.set_env(L"WINUX_SHELL_GLOB", L"done");
  p.add(L"cat.exe", {L"*.txt"});

  auto r = p.run();

  EXPECT_NE(r.exit_code, 0);
  EXPECT_TRUE(r.stderr_text.find("*.txt") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("content") == std::string::npos);
}

TEST(cat, cat_wildcard_question_mark) {
  TempDir tmp;
  tmp.write("file1.txt", "content1\n");
  tmp.write("file2.txt", "content2\n");
  tmp.write("file10.txt", "content10\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"file?.txt"});

  TEST_LOG_CMD_LIST("cat.exe", L"file?.txt");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("cat.exe file?.txt output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("content1") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("content2") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("content10") == std::string::npos);
}

TEST(cat, cat_wildcard_char_class) {
  TempDir tmp;
  tmp.write("a.txt", "aaa\n");
  tmp.write("b.txt", "bbb\n");
  tmp.write("c.log", "ccc\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"[ab]*"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("aaa") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("bbb") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("ccc") == std::string::npos);
}

TEST(cat, cat_wildcard_char_class_in_parent_directory_segment) {
  TempDir tmp;
  std::filesystem::create_directories(tmp.path / "d1");
  std::filesystem::create_directories(tmp.path / "d2");
  std::filesystem::create_directories(tmp.path / "d3");
  tmp.write("d1/a.txt", "aaa\n");
  tmp.write("d2/b.txt", "bbb\n");
  tmp.write("d3/c.txt", "ccc\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"d[12]\\*.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("aaa") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("bbb") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("ccc") == std::string::npos);
}

TEST(cat, cat_char_class_prefers_glob_over_same_spelling_literal_path) {
  TempDir tmp;
  tmp.write("[ab].txt", "literal\n");
  tmp.write("a.txt", "aaa\n");
  tmp.write("b.txt", "bbb\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"[ab].txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("aaa") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("bbb") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("literal") == std::string::npos);
}

TEST(cat, cat_directory_input_reports_is_a_directory) {
  TempDir tmp;
  tmp.mkdir("indir");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"indir"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_TRUE(r.stderr_text.find("cat: indir: Is a directory") !=
              std::string::npos);
}

TEST(cat, cat_directory_input_accepts_trailing_separator_diagnostic) {
  TempDir tmp;
  tmp.mkdir("indir");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"indir/"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_TRUE(r.stderr_text.find("cat: indir/: Is a directory") !=
              std::string::npos);
}

TEST(cat, cat_file_with_trailing_separator_reports_not_directory) {
  TempDir tmp;
  tmp.write("file.txt", "payload\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"file.txt/"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_TRUE(r.stdout_text.empty());
  EXPECT_TRUE(r.stderr_text.find("cat: file.txt/: Not a directory") !=
              std::string::npos);
}

TEST(cat, cat_missing_input_reports_gnu_shaped_diagnostic) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"missing.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_TRUE(r.stdout_text.empty());
  EXPECT_TRUE(
      r.stderr_text.find("cat: missing.txt: No such file or directory") !=
      std::string::npos);
}

TEST(cat, cat_number_uses_gnu_tab_separator) {
  TempDir tmp;
  tmp.write("n.txt", "alpha\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"-n", L"n.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "     1\talpha\n");
}

TEST(cat, cat_number_preserves_final_unterminated_line) {
  TempDir tmp;
  tmp.write("tail.txt", "tail");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"-n", L"tail.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "     1\ttail");
}

TEST(cat, cat_show_ends_crlf_outputs_caret_m) {
  TempDir tmp;
  tmp.write("crlf.txt", "alpha\r\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"-E", L"crlf.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "alpha^M$\n");
}

TEST(cat, cat_squeeze_blank_keeps_space_only_lines) {
  TempDir tmp;
  tmp.write("blank.txt", "\n\n \n\n\nx\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"-s", L"blank.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "\n \n\nx\n");
}

TEST(cat, cat_u_is_ignored_compatibility_option) {
  TempDir tmp;
  tmp.write("u.txt", "alpha\nbeta\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"-u", L"u.txt"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "alpha\nbeta\n");
}

TEST(cat, cat_show_all_quotes_nonprinting_like_gnu) {
  TempDir tmp;
  tmp.write_bytes(
      "visible.bin",
      std::vector<char>{static_cast<char>(0x41), static_cast<char>(0x09),
                        static_cast<char>(0x42), static_cast<char>(0x0D),
                        static_cast<char>(0x0A), static_cast<char>(0x7F),
                        static_cast<char>(0x80), static_cast<char>(0xFF),
                        static_cast<char>(0x0A), static_cast<char>(0x0A),
                        static_cast<char>(0x5A)});

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"cat.exe", {L"-A", L"visible.bin"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "A^IB^M$\n^?M-^@M-^?$\n$\nZ");
}

/*
 *  Copyright © 2026 [caomengxuan666]
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
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  - File: date_unit_test.cpp
 *  - Username: Administrator
 *  - CopyrightYear: 2026
 */
#include "framework/winuxtest.h"

TEST(date, date_basic) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {});

  TEST_LOG_CMD_LIST("date.exe");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(date, date_relative_day_is_accepted) {
  Pipeline p;
  p.add(L"date.exe", {L"-u", L"--date", L"+1 day", L"+%s"});
  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(date, date_format) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"+%Y-%m-%d"});

  TEST_LOG_CMD_LIST("date.exe", L"+%Y-%m-%d");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date format output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
  // Check format YYYY-MM-DD
  EXPECT_TRUE(r.stdout_text.find('-') != std::string::npos);
}

TEST(date, date_utc) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u"});

  TEST_LOG_CMD_LIST("date.exe", L"-u");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date UTC output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(date, date_rfc2822) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-R"});

  TEST_LOG_CMD_LIST("date.exe", L"-R");

  auto r = p.run();

  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date RFC2822 output", r.stdout_text);

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_FALSE(r.stdout_text.empty());
}

TEST(date, date_date_option_formats_fixed_utc_time) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"--date", L"2024-01-02 03:04:05 UTC",
                      L"+%Y-%m-%dT%H:%M:%S%z"});

  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "2024-01-02T03:04:05+0000\n");
}

TEST(date, date_common_format_specifiers) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"--date", L"@0", L"+%F %T %s %j %u"});

  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "1970-01-01 00:00:00 0 001 4\n");
}

TEST(date, date_iso8601_seconds) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"--date", L"@0", L"--iso-8601=seconds"});

  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "1970-01-01T00:00:00+00:00\n");
}

TEST(date, date_rfc3339_seconds) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe",
        {L"-u", L"--date", L"2024-01-02 03:04:05 UTC", L"--rfc-3339=seconds"});

  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "2024-01-02 03:04:05+00:00\n");
}

TEST(date, date_reference_uses_file_modification_time) {
  TempDir tmp;

  Pipeline touch;
  touch.set_cwd(tmp.wpath());
  touch.add(L"touch.exe",
            {L"-d", L"2024-01-02 03:04:05 UTC", L"reference.txt"});
  EXPECT_EQ(touch.run().exit_code, 0);

  Pipeline date;
  date.set_cwd(tmp.wpath());
  date.add(L"date.exe",
           {L"-u", L"-r", L"reference.txt", L"+%Y-%m-%dT%H:%M:%S%z"});

  auto r = date.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "2024-01-02T03:04:05+0000\n");

  Pipeline long_date;
  long_date.set_cwd(tmp.wpath());
  long_date.add(L"date.exe",
                {L"-u", L"--reference=reference.txt", L"+%Y-%m-%dT%H:%M:%S%z"});

  auto long_result = long_date.run();
  EXPECT_EQ(long_result.exit_code, 0);
  EXPECT_EQ(long_result.stdout_text, "2024-01-02T03:04:05+0000\n");
}

TEST(date, date_rfc_email_fixed_utc_time) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"-R", L"--date", L"2024-01-02 03:04:05 UTC"});

  auto r = p.run();
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "Tue, 02 Jan 2024 03:04:05 +0000\n");
}

TEST(date, date_invalid_positional_operand_is_rejected) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"notadate"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date invalid positional stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("invalid date"), std::string::npos);
  EXPECT_NE(r.stderr_text.find("notadate"), std::string::npos);
}

TEST(date, date_extra_operand_is_rejected) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  // Invalid POSIX date so no system clock mutation can occur even if the
  // operand parsing ever changed.
  p.add(L"date.exe", {L"99999999999999", L"99999999999999"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date extra operand stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("extra operand"), std::string::npos);
}

TEST(date, date_operand_without_plus_with_date_option) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-d", L"now", L"xyz"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date lacks plus stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("lacks a leading '+'"), std::string::npos);
}

TEST(date, date_multiple_output_formats_are_rejected) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-R", L"+%Y"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date multiple formats stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("multiple output formats specified"),
            std::string::npos);
}

TEST(date, date_mutually_exclusive_date_sources) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-d", L"now", L"-r", L"somefile"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date exclusive stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("mutually exclusive"), std::string::npos);
}

TEST(date, date_set_and_print_options_conflict) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-s", L"now", L"-d", L"now"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date set/print conflict stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("may not be used together"), std::string::npos);
}

TEST(date, date_resolution_prints_nanosecond_tick) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"--resolution"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date resolution output", r.stdout_text);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "0.000000100\n");
}

TEST(date, date_invalid_posix_clock_operand) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  // Month 13 -> invalid MMDDhhmm[[CC]YY][.ss] POSIX set-date syntax.
  p.add(L"date.exe", {L"130203042026"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date invalid posix stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("invalid date"), std::string::npos);
}

TEST(date, date_utc_parses_zoneless_string_as_utc) {
  Pipeline p;
  p.add(L"date.exe",
        {L"-u", L"-d", L"2024-01-02 03:04:05", L"+%Y-%m-%dT%H:%M:%S"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date -u zoneless stdout", r.stdout_text);
  // [GNU] with -u a date string without a timezone is interpreted as UTC.
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "2024-01-02T03:04:05\n");
}

TEST(date, date_local_parses_zoneless_string_as_local) {
  Pipeline p;
  p.add(L"date.exe", {L"-d", L"2024-01-02 03:04:05", L"+%Y-%m-%dT%H:%M:%S%z"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date local zoneless stdout", r.stdout_text);
  // Without -u the same string must render the local wall-clock fields
  // (the numeric offset is whatever the machine's timezone is).
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text.substr(0, 11), "2024-01-02T");
  EXPECT_EQ(r.stdout_text.substr(11, 9), "03:04:05+");
}

TEST(date, date_file_directory_reports_read_error) {
  TempDir tmp;
  std::filesystem::create_directories(tmp.path / "adir");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"-f", L"adir", L"+%Y-%m-%d"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date -f dir stderr", r.stderr_text);
  // [GNU] date: adir: read error: Is a directory, exit 1 (uutils #14434).
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("read error: Is a directory"),
            std::string::npos);
  EXPECT_NE(r.stderr_text.find("adir"), std::string::npos);
}

TEST(date, date_file_missing_reports_open_error) {
  TempDir tmp;

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"--file=no-such-file.txt", L"+%Y-%m-%d"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date -f missing stderr", r.stderr_text);
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_NE(r.stderr_text.find("No such file or directory"),
            std::string::npos);
  EXPECT_NE(r.stderr_text.find("no-such-file.txt"), std::string::npos);
}

TEST(date, date_file_formats_each_line_and_continues_after_invalid) {
  TempDir tmp;
  tmp.write("dates.txt",
            "2024-01-02 03:04:05\nnot a date\n2025-06-07 08:09:10\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"-f", L"dates.txt", L"+%Y-%m-%d"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date -f mixed stdout", r.stdout_text);
  TEST_LOG("date -f mixed stderr", r.stderr_text);
  // [GNU] batch mode reports the invalid line, keeps processing, and exits 1.
  EXPECT_EQ(r.exit_code, 1);
  EXPECT_EQ(r.stdout_text, "2024-01-02\n2025-06-07\n");
  EXPECT_NE(r.stderr_text.find("invalid date 'not a date'"),
            std::string::npos);
}

TEST(date, date_file_valid_file_prints_every_line) {
  TempDir tmp;
  tmp.write("dates.txt", "2024-01-02 03:04:05\n2025-06-07 08:09:10\n");

  Pipeline p;
  p.set_cwd(tmp.wpath());
  p.add(L"date.exe", {L"-u", L"-f", L"dates.txt", L"+%Y-%m-%dT%H:%M:%S"});

  auto r = p.run();
  TEST_LOG_EXIT_CODE(r);
  TEST_LOG("date -f valid stdout", r.stdout_text);
  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ(r.stdout_text, "2024-01-02T03:04:05\n2025-06-07T08:09:10\n");
}

#include "framework/winuxtest.h"

TEST(getopt, getopt_parses_short_options_after_double_dash) {
  Pipeline p;
  p.add(L"getopt.exe", {L"ab:c", L"--", L"-a", L"-b", L"value", L"file"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "'-a' '-b' 'value' '--' 'file'\n");
}

TEST(getopt, getopt_supports_attached_required_argument) {
  Pipeline p;
  p.add(L"getopt.exe", {L"ab:c", L"--", L"-abvalue", L"file"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "'-a' '-b' 'value' '--' 'file'\n");
}

TEST(getopt, getopt_reports_unknown_option) {
  Pipeline p;
  p.add(L"getopt.exe", {L"ab:", L"--", L"-z"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 1);
  EXPECT_CONTAINS(r.stderr_text, "invalid option");
}

TEST(getopt, getopt_unquoted_mode_outputs_plain_tokens) {
  Pipeline p;
  p.add(L"getopt.exe", {L"-u", L"ab:", L"--", L"-b", L"value"});

  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_EQ_TEXT(r.stdout_text, "-b value --\n");
}

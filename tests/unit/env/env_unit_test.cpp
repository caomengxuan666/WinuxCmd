#include "framework/winuxtest.h"

TEST(env, env_lists_current) {
  Pipeline p;
  p.set_env(L"FOO", L"BAR");
  p.add(L"env.exe", {});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("FOO=BAR") != std::string::npos);
}

TEST(env, env_ignore_environment_and_set) {
  Pipeline p;
  p.set_env(L"SHOULD_NOT", L"SEE");
  p.add(L"env.exe", {L"-i", L"X=1"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("X=1") != std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("SHOULD_NOT") == std::string::npos);
}

TEST(env, env_unset_variable) {
  Pipeline p;
  p.set_env(L"KEEP", L"1");
  p.set_env(L"DROP", L"1");
  p.add(L"env.exe", {L"-u", L"DROP", L"KEEP=2"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 0);
  EXPECT_TRUE(r.stdout_text.find("DROP=") == std::string::npos);
  EXPECT_TRUE(r.stdout_text.find("KEEP=2") != std::string::npos);
}

TEST(env, env_command_not_supported) {
  Pipeline p;
  p.add(L"env.exe", {L"FOO=1", L"cmd"});
  auto r = p.run();

  EXPECT_EQ(r.exit_code, 2);
}

#pragma once
#include <string>
#include <vector>

struct CommandResult {
  int exit_code = -1;
  std::string stdout_text;
  std::string stderr_text;
};

CommandResult run_command(const std::wstring& exe,
                          const std::vector<std::wstring>& args,
                          const std::string& stdin_data = {});

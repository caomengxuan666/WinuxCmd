#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>

struct CommandResult;

struct PipelineCommand {
  std::wstring exe;
  std::vector<std::wstring> args;
};

class Pipeline {
 public:
  void add(const std::wstring &exe, const std::vector<std::wstring> &args);

  void set_stdin(std::string data);

  void set_cwd(const std::wstring &dir);
  void set_env(const std::wstring &k, const std::wstring &v);

  CommandResult run();

 private:
  std::vector<PipelineCommand> cmds_;
  std::optional<std::string> stdin_data_;
  std::optional<std::wstring> cwd_;
  std::map<std::wstring, std::wstring> env_;
};

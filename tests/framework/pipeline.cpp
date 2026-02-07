#include "pipeline.h"

#include <windows.h>

#include <stdexcept>
#include <thread>
#include <vector>

#include "process_win32.h"

// ---------------- Pipeline API ----------------

void Pipeline::add(const std::wstring &exe,
                   const std::vector<std::wstring> &args) {
  cmds_.push_back({exe, args});
}

void Pipeline::set_stdin(std::string data) { stdin_data_ = std::move(data); }

void Pipeline::set_cwd(const std::wstring &dir) { cwd_ = dir; }

void Pipeline::set_env(const std::wstring &k, const std::wstring &v) {
  env_[k] = v;
}

// ---------------- Windows command line quoting ----------------

static std::wstring quote_arg(const std::wstring &arg) {
  if (arg.empty()) return L"\"\"";

  bool need_quote = arg.find_first_of(L" \t\"") != std::wstring::npos;

  if (!need_quote) return arg;

  std::wstring out = L"\"";
  size_t backslashes = 0;

  for (wchar_t c : arg) {
    if (c == L'\\') {
      backslashes++;
    } else if (c == L'"') {
      out.append(backslashes * 2 + 1, L'\\');
      out.push_back(L'"');
      backslashes = 0;
    } else {
      out.append(backslashes, L'\\');
      backslashes = 0;
      out.push_back(c);
    }
  }

  out.append(backslashes * 2, L'\\');
  out.push_back(L'"');
  return out;
}

static std::wstring build_cmd(const std::wstring &exe,
                              const std::vector<std::wstring> &args) {
  std::wstring cmd = quote_arg(exe);
  for (auto &a : args) {
    cmd += L" ";
    cmd += quote_arg(a);
  }
  return cmd;
}

// ---------------- Pipeline execution ----------------

CommandResult Pipeline::run() {
  if (cmds_.empty()) throw std::runtime_error("Pipeline: no commands");

  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};

  // ----- stdin pipe (optional) -----
  HANDLE stdin_r = nullptr;
  HANDLE stdin_w = nullptr;

  if (stdin_data_) {
    if (!CreatePipe(&stdin_r, &stdin_w, &sa, 0))
      throw std::runtime_error("CreatePipe(stdin) failed");

    // parent writes, children read
    SetHandleInformation(stdin_w, HANDLE_FLAG_INHERIT, 0);
  }

  const size_t n = cmds_.size();

  std::vector<HANDLE> read_pipes(n > 1 ? n - 1 : 0);
  std::vector<HANDLE> write_pipes(n > 1 ? n - 1 : 0);
  std::vector<PROCESS_INFORMATION> procs(n);

  for (auto &p : procs) ZeroMemory(&p, sizeof(p));

  // ----- middle pipes -----
  for (size_t i = 0; i + 1 < n; ++i) {
    if (!CreatePipe(&read_pipes[i], &write_pipes[i], &sa, 0))
      throw std::runtime_error("CreatePipe(mid) failed");

    SetHandleInformation(read_pipes[i], HANDLE_FLAG_INHERIT, 0);
  }

  // ----- final stdout / stderr -----
  HANDLE final_out_r, final_out_w;
  HANDLE final_err_r, final_err_w;

  if (!CreatePipe(&final_out_r, &final_out_w, &sa, 0))
    throw std::runtime_error("CreatePipe(stdout) failed");

  if (!CreatePipe(&final_err_r, &final_err_w, &sa, 0))
    throw std::runtime_error("CreatePipe(stderr) failed");

  SetHandleInformation(final_out_r, HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(final_err_r, HANDLE_FLAG_INHERIT, 0);

  // ----- spawn processes -----
  for (size_t i = 0; i < n; ++i) {
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;

    // stdin
    if (i == 0) {
      si.hStdInput = stdin_r ? stdin_r : GetStdHandle(STD_INPUT_HANDLE);
    } else {
      si.hStdInput = read_pipes[i - 1];
    }

    // stdout
    if (i + 1 < n) {
      si.hStdOutput = write_pipes[i];
    } else {
      si.hStdOutput = final_out_w;
    }

    // stderr: all go to same pipe
    si.hStdError = final_err_w;

    auto cmdline = build_cmd(cmds_[i].exe, cmds_[i].args);

    const wchar_t *cwd = cwd_ ? cwd_->c_str() : nullptr;
    auto build_env_block = [this]() -> std::wstring {
      std::wstring block;
      for (auto &[k, v] : this->env_) {
        block += k;
        block += L"=";
        block += v;
        block.push_back(L'\0');
      }
      block.push_back(L'\0');
      return block;
    };
    std::wstring env_block;
    LPVOID env_ptr = nullptr;

    if (!env_.empty()) {
      env_block = build_env_block();
      env_ptr = (LPVOID)env_block.c_str();
    }

    if (!CreateProcessW(nullptr, cmdline.data(), nullptr, nullptr, TRUE, 0,
                        env_ptr, cwd, &si, &procs[i])) {
      DWORD err = GetLastError();
      throw std::runtime_error("CreateProcessW failed: " + std::to_string(err));
    }

    if (i > 0) CloseHandle(read_pipes[i - 1]);

    if (i + 1 < n) CloseHandle(write_pipes[i]);
  }

  // parent no longer needs these
  CloseHandle(final_out_w);
  CloseHandle(final_err_w);

  // ----- write stdin -----
  if (stdin_w) {
    DWORD written = 0;
    WriteFile(stdin_w, stdin_data_->data(), (DWORD)stdin_data_->size(),
              &written, nullptr);
    CloseHandle(stdin_w);  // EOF
  }

  if (stdin_r) CloseHandle(stdin_r);

  // ----- read stdout / stderr concurrently -----
  auto read_all = [](HANDLE h) {
    std::string out;
    char buf[4096];
    DWORD r;
    while (ReadFile(h, buf, sizeof(buf), &r, nullptr) && r > 0)
      out.append(buf, buf + r);
    return out;
  };

  std::string out, err;
  std::thread t_out([&] { out = read_all(final_out_r); });
  std::thread t_err([&] { err = read_all(final_err_r); });

  // ----- wait processes -----
  for (auto &p : procs) WaitForSingleObject(p.hProcess, INFINITE);

  DWORD exit_code = 0;
  GetExitCodeProcess(procs.back().hProcess, &exit_code);

  t_out.join();
  t_err.join();

  for (auto &p : procs) {
    CloseHandle(p.hProcess);
    CloseHandle(p.hThread);
  }

  CloseHandle(final_out_r);
  CloseHandle(final_err_r);

  return {(int)exit_code, out, err};
}

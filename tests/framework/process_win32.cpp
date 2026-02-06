#include "process_win32.h"

#include <windows.h>

static std::wstring build_cmd(const std::wstring &exe,
                              const std::vector<std::wstring> &args) {
  std::wstring cmd = L"\"" + exe + L"\"";
  for (auto &a : args) cmd += L" \"" + a + L"\"";
  return cmd;
}

static std::string read_all(HANDLE h) {
  std::string out;
  char buf[4096];
  DWORD read;
  while (ReadFile(h, buf, sizeof(buf), &read, nullptr) && read > 0)
    out.append(buf, buf + read);
  return out;
}

CommandResult run_command(const std::wstring &exe,
                          const std::vector<std::wstring> &args,
                          const std::string &stdin_data) {
  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};

  HANDLE in_r, in_w, out_r, out_w, err_r, err_w;
  CreatePipe(&in_r, &in_w, &sa, 0);
  CreatePipe(&out_r, &out_w, &sa, 0);
  CreatePipe(&err_r, &err_w, &sa, 0);

  SetHandleInformation(in_w, HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(out_r, HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(err_r, HANDLE_FLAG_INHERIT, 0);

  STARTUPINFOW si{};
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdInput = in_r;
  si.hStdOutput = out_w;
  si.hStdError = err_w;

  PROCESS_INFORMATION pi{};
  auto cmd = build_cmd(exe, args);

  CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE, 0, nullptr,
                 nullptr, &si, &pi);

  CloseHandle(in_r);
  CloseHandle(out_w);
  CloseHandle(err_w);

  if (!stdin_data.empty()) {
    DWORD written;
    WriteFile(in_w, stdin_data.data(),
              // NOLINTNEXTLINE
              (DWORD)stdin_data.size(), &written, nullptr);
  }
  CloseHandle(in_w);

  auto out = read_all(out_r);
  auto err = read_all(err_r);

  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD code;
  GetExitCodeProcess(pi.hProcess, &code);

  CloseHandle(out_r);
  CloseHandle(err_r);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return {(int)code, out, err};  // NOLINT
}

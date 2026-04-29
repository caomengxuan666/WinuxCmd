/*
 *  Copyright © 2026 WinuxCmd
 */
#include "pch/pch.h"
#include "core/command_macros.h"
import std;
import core;
import utils;
import container;

auto constexpr SHRED_OPTIONS = std::array{OPTION("-f", "--force", "force overwrite"),
                                                   OPTION("-n", "", "overwrite N times", INT_TYPE),
                                                   OPTION("-u", "", "remove after shredding")};

REGISTER_COMMAND(
    shred,
    /* cmd_name */ "shred",
    /* cmd_synopsis */ "shred [OPTION] FILE...",
    /* cmd_desc */ "Overwrite the specified FILE(s) repeatedly.",
    /* examples */ "shred -f -u secret.txt",
    /* see_also */ "rm",
    /* author */ "WinuxCmd",
    /* copyright */ "Copyright © 2026 WinuxCmd",
    /* options */ SHRED_OPTIONS) {
  if (ctx.positionals.empty()) {
    safeErrorPrintLn("shred: missing file operand");
    return 1;
  }

  bool force = ctx.get<bool>("-f", false) || ctx.get<bool>("--force", false);
  int passes = ctx.get<int>("-n", 3);
  bool remove = ctx.get<bool>("-u", false);

  for (const auto& filename : ctx.positionals) {
    std::string file_arg(filename);
    std::vector<std::string> expanded;
    if (contains_wildcard(file_arg)) {
      auto glob_result = glob_expand(file_arg);
      if (glob_result.expanded) {
        for (const auto& f : glob_result.files) {
          expanded.push_back(wstring_to_utf8(f));
        }
      } else {
        expanded.push_back(file_arg);
      }
    } else {
      expanded.push_back(file_arg);
    }
    for (const auto& exp : expanded) {
      std::wstring wfilename = utf8_to_wstring(exp);
      HANDLE hFile = CreateFileW(wfilename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

      if (hFile == INVALID_HANDLE_VALUE) {
        safeErrorPrintLn("shred: cannot open '" + exp + "'");
        continue;
      }

      LARGE_INTEGER fileSize;
      GetFileSizeEx(hFile, &fileSize);

      // Overwrite file multiple times
      for (int i = 0; i < passes; ++i) {
        std::vector<char> buffer(fileSize.QuadPart);

        // Fill with random data (simplified - using pattern)
        for (LONGLONG j = 0; j < fileSize.QuadPart; ++j) {
          buffer[j] = static_cast<char>((i + j) % 256);
        }

        LARGE_INTEGER li = {0};
        SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN);

        DWORD bytesWritten;
        WriteFile(hFile, buffer.data(), static_cast<DWORD>(fileSize.QuadPart), &bytesWritten, nullptr);
        FlushFileBuffers(hFile);
      }

      CloseHandle(hFile);

      // Remove file if requested
      if (remove) {
        std::wstring wremove = utf8_to_wstring(exp);
        DeleteFileW(wremove.c_str());
      }

      safePrintLn("shred: '" + exp + "' removed");
    }
  }

  return 0;
}

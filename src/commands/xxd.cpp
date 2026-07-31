/*
 *  Copyright © 2026 WinuxCmd
 */
#include "core/command_macros.h"
#include "pch/pch.h"
import std;
import core;
import utils;
import container;

auto constexpr XXD_OPTIONS =
    std::array{OPTION("-r", "--reverse", "reverse: convert hex to binary")};

namespace {
std::vector<unsigned char> read_stdin_bytes() {
  std::vector<unsigned char> data;
  char ch = 0;
  while (std::cin.get(ch)) {
    data.push_back(static_cast<unsigned char>(ch));
  }
  return data;
}

std::optional<std::vector<unsigned char>> read_file_bytes(
    const std::string& filename) {
  std::wstring wfilename = utf8_to_wstring(filename);
  HANDLE hFile =
      CreateFileW(wfilename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile == INVALID_HANDLE_VALUE) return std::nullopt;

  LARGE_INTEGER fileSize{};
  if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 0) {
    CloseHandle(hFile);
    return std::nullopt;
  }

  std::vector<unsigned char> buffer(static_cast<size_t>(fileSize.QuadPart));
  DWORD bytesRead = 0;
  BOOL ok = TRUE;
  if (!buffer.empty()) {
    ok = ReadFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()),
                  &bytesRead, nullptr);
  }
  CloseHandle(hFile);
  if (!ok) return std::nullopt;
  buffer.resize(bytesRead);
  return buffer;
}

std::string byte_hex(unsigned char value) {
  constexpr char digits[] = "0123456789abcdef";
  std::string out;
  out.push_back(digits[(value >> 4) & 0x0f]);
  out.push_back(digits[value & 0x0f]);
  return out;
}

std::string offset_hex(size_t offset) {
  char buf[32];
  sprintf_s(buf, sizeof(buf), "%08zx", offset);
  return std::string(buf);
}

void print_default_xxd(const std::vector<unsigned char>& data) {
  constexpr size_t kColumns = 16;
  for (size_t offset = 0; offset < data.size(); offset += kColumns) {
    size_t count = std::min(kColumns, data.size() - offset);
    safePrint(offset_hex(offset));
    safePrint(": ");

    for (size_t i = 0; i < kColumns; ++i) {
      if (i < count) {
        safePrint(byte_hex(data[offset + i]));
      } else {
        safePrint("  ");
      }
      if (i % 2 == 1) safePrint(" ");
    }

    safePrint(" ");
    for (size_t i = 0; i < count; ++i) {
      unsigned char c = data[offset + i];
      safePrint((c > 31 && c < 127) ? std::string(1, static_cast<char>(c))
                                    : ".");
    }
    safePrint("\n");
  }
}
}  // namespace

REGISTER_COMMAND(xxd,
                 /* cmd_name */ "xxd",
                 /* cmd_synopsis */ "xxd [OPTION] [FILE]",
                 /* cmd_desc */ "Make a hexdump or do the reverse.",
                 /* examples */ "xxd file.txt\nxxd -r hex.txt > file.txt",
                 /* see_also */ "od",
                 /* author */ "WinuxCmd",
                 /* copyright */ "Copyright © 2026 WinuxCmd",
                 /* options */ XXD_OPTIONS) {
  bool reverse =
      ctx.get<bool>("-r", false) || ctx.get<bool>("--reverse", false);

  if (reverse) {
    safeErrorPrintLn("xxd: reverse mode not fully implemented");
    return 1;
  }

  std::string filename =
      ctx.positionals.empty() ? "-" : std::string(ctx.positionals[0]);

  std::vector<unsigned char> data;
  if (filename == "-") {
    data = read_stdin_bytes();
  } else {
    auto file_data = read_file_bytes(filename);
    if (!file_data) {
      safeErrorPrintLn("xxd: cannot open " + filename);
      return 1;
    }
    data = std::move(*file_data);
  }

  print_default_xxd(data);
  return 0;
}

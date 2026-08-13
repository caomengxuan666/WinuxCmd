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

std::optional<unsigned char> parse_hex_byte(char high, char low) {
  auto digit = [](char c) -> int {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
  };
  int h = digit(high), l = digit(low);
  if (h < 0 || l < 0) return std::nullopt;
  return static_cast<unsigned char>((h << 4) | l);
}

std::optional<std::vector<unsigned char>> reverse_xxd(
    const std::vector<unsigned char>& input) {
  std::string text(input.begin(), input.end());
  std::vector<unsigned char> output;
  size_t line_start = 0;
  while (line_start <= text.size()) {
    size_t line_end = text.find('\n', line_start);
    if (line_end == std::string::npos) line_end = text.size();
    std::string_view line(text.data() + line_start, line_end - line_start);
    size_t colon = line.find(':');
    if (colon != std::string_view::npos) line.remove_prefix(colon + 1);
    size_t pos = 0;
    while (pos < line.size()) {
      while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) ++pos;
      size_t token_start = pos;
      while (pos < line.size() && !std::isspace(static_cast<unsigned char>(line[pos]))) ++pos;
      auto token = line.substr(token_start, pos - token_start);
      if (token.empty()) continue;
      if (token.size() % 2 != 0) break;
      std::vector<unsigned char> token_bytes;
      bool valid = true;
      for (size_t i = 0; i < token.size(); i += 2) {
        auto byte = parse_hex_byte(token[i], token[i + 1]);
        if (!byte) { valid = false; break; }
        token_bytes.push_back(*byte);
      }
      if (!valid) break;
      output.insert(output.end(), token_bytes.begin(), token_bytes.end());
    }
    if (line_end == text.size()) break;
    line_start = line_end + 1;
  }
  return output;
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
    std::string filename = ctx.positionals.empty() ? "-" : std::string(ctx.positionals[0]);
    std::vector<unsigned char> input;
    if (filename == "-") input = read_stdin_bytes();
    else {
      auto file_data = read_file_bytes(filename);
      if (!file_data) { safeErrorPrintLn("xxd: cannot open " + filename); return 1; }
      input = std::move(*file_data);
    }
    auto decoded = reverse_xxd(input);
    if (!decoded) { safeErrorPrintLn("xxd: invalid hex dump"); return 1; }
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!decoded->empty()) {
      DWORD written = 0;
      if (!WriteFile(out, decoded->data(), static_cast<DWORD>(decoded->size()),
                     &written, nullptr) || written != decoded->size()) return 1;
    }
    return 0;
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

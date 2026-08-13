/// @Author: caomengxuan666
/// @Description: File I/O utilities
/// @Version: 0.1.0
/// @License: MIT
/// @Copyright: Copyright © 2026 WinuxCmd
module;

#include "pch/pch.h"
export module utils:file_io;

import std;
import :utf8;
import :native_path;
import :win32;
import :i18n;

namespace {
constexpr size_t kReadChunkSize = 64 * 1024;

auto read_open_error(std::string_view path,
                     const native_path::ApiPathOperand& operand,
                     unsigned long error) -> std::string {
  const DWORD attrs = native_path::attributes_w(operand.extended);
  if (operand.had_trailing_separator &&
      native_path::attributes_are_regular_file(attrs)) {
    return winux::i18n::format("utils.file.error.not_directory",
                               "cannot open '{}' for reading: Not a directory",
                               path);
  }

  if (native_path::attributes_are_directory(attrs)) {
    return winux::i18n::format("utils.file.error.is_directory",
                               "cannot open '{}' for reading: Is a directory",
                               path);
  }

  return winux::i18n::format(
      "utils.file.error.open", "cannot open '{}' for reading: {}", path,
      win32_posix_error_text(error, {.invalid_name_as_missing = true}));
}

auto reserve_file_size(std::string& content, HANDLE file) -> void {
  LARGE_INTEGER file_size{};
  if (!GetFileSizeEx(file, &file_size) || file_size.QuadPart <= 0) return;

  const auto size = static_cast<unsigned long long>(file_size.QuadPart);
  if (size <= static_cast<unsigned long long>(content.max_size())) {
    content.reserve(static_cast<size_t>(size));
  }
}

auto read_handle_to_string(HANDLE file, std::string_view path)
    -> std::expected<std::string, std::string> {
  std::string content;
  reserve_file_size(content, file);

  std::array<char, kReadChunkSize> buffer{};
  for (;;) {
    DWORD bytes_read = 0;
    if (!ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()),
                  &bytes_read, nullptr)) {
      return std::unexpected(winux::i18n::format("utils.file.error.read",
                                                 "error reading '{}'", path));
    }
    if (bytes_read == 0) break;
    content.append(buffer.data(), bytes_read);
  }

  return content;
}
}  // namespace

export namespace file_io {

export auto open_binary_file(std::string_view filename) -> std::ifstream {
  auto operand = native_path::make_api_path_operand(filename);
  return std::ifstream(std::filesystem::path(operand.extended),
                       std::ios::binary);
}

export auto create_binary_file(std::string_view filename) -> std::ofstream {
  auto operand = native_path::make_api_path_operand(filename);
  return std::ofstream(std::filesystem::path(operand.extended),
                       std::ios::binary | std::ios::trunc);
}

auto read_all_stdin() -> std::expected<std::string, std::string> {
  std::string content;
  std::array<char, kReadChunkSize> buffer{};

  while (std::cin.good()) {
    std::cin.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    const auto bytes_read = std::cin.gcount();
    if (bytes_read > 0) {
      content.append(buffer.data(), static_cast<size_t>(bytes_read));
    }
  }

  if (std::cin.bad()) {
    return std::unexpected(winux::i18n::translate(
        "utils.file.error.read_stdin", "error reading from standard input"));
  }

  return content;
}

auto read_all_file(std::string_view filename)
    -> std::expected<std::string, std::string> {
  auto operand = native_path::make_api_path_operand(filename);
  if (operand.had_trailing_separator) {
    const DWORD attrs = native_path::attributes_w(operand.extended);
    if (native_path::attributes_are_regular_file(attrs)) {
      return std::unexpected(
          read_open_error(filename, operand, ERROR_DIRECTORY));
    }
  }

  HANDLE file =
      CreateFileW(operand.extended.c_str(), GENERIC_READ,
                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                  nullptr, OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return std::unexpected(read_open_error(filename, operand, GetLastError()));
  }

  UniqueHandle close_file(file);
  return read_handle_to_string(close_file.get(), filename);
}

auto read_all_input(std::string_view filename)
    -> std::expected<std::string, std::string> {
  if (filename == "-") return read_all_stdin();
  return read_all_file(filename);
}

}  // namespace file_io

/**
 * @brief Read file into lines
 * @param filename File path
 * @return Vector of lines (empty on error)
 */
export std::vector<std::string> read_file_lines(const std::string& filename) {
  std::vector<std::string> lines;

  auto content_result = file_io::read_all_file(filename);
  if (!content_result) {
    return lines;
  }

  const auto& content = *content_result;
  size_t start = 0;
  if (content.size() >= 3 && static_cast<unsigned char>(content[0]) == 0xEF &&
      static_cast<unsigned char>(content[1]) == 0xBB &&
      static_cast<unsigned char>(content[2]) == 0xBF) {
    start = 3;
  }

  std::istringstream iss(content.substr(start));
  std::string line;
  while (std::getline(iss, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    lines.push_back(line);
  }

  return lines;
}
